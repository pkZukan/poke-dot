#include "particle_converter.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/quad_mesh.hpp>
#include <godot_cpp/classes/curve.hpp>
#include <godot_cpp/classes/gradient.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace
{
constexpr int MAX_PARTICLES_PER_EMITTER = 4096;
constexpr double INFINITE = std::numeric_limits<double>::infinity();

bool has_texture_id(const String& id)
{
    return id != "0" && id != "18446744073709551615";
}

Ref<Texture2D> alpha_mask_texture(const Ref<Texture2D>& texture)
{
    Ref<Image> image = texture->get_image();
    if (image.is_null())
    {
        return texture;
    }
    image = image->duplicate();
    if (image->is_compressed() && image->decompress() != OK)
    {
        return texture;
    }
    image->convert(Image::FORMAT_RGBAF);

    // VFX's RGB alpha input uses the color texture as a coverage mask.
    // For the supported single-texture sprites, that mask is in red.
    for (int y = 0; y < image->get_height(); ++y)
    {
        for (int x = 0; x < image->get_width(); ++x)
        {
            Color pixel = image->get_pixel(x, y);
            pixel.a = pixel.r;
            image->set_pixel(x, y, pixel);
        }
    }
    return ImageTexture::create_from_image(image);
}

Vector3 sample_keys(const Array& keys, double time, const Vector3& fallback)
{
    if (keys.is_empty())
    {
        return fallback;
    }

    Vector4 previous = keys[0];
    if (time <= previous.w)
    {
        return Vector3(previous.x, previous.y, previous.z);
    }

    for (int i = 1; i < keys.size(); ++i)
    {
        const Vector4 next = keys[i];
        if (time <= next.w)
        {
            const double span = next.w - previous.w;
            const double weight = span > 0.0 ? (time - previous.w) / span : 1.0;
            return Vector3(previous.x, previous.y, previous.z).lerp(Vector3(next.x, next.y, next.z), weight);
        }
        previous = next;
    }

    return Vector3(previous.x, previous.y, previous.z);
}

Ref<Gradient> color_gradient(const Dictionary& emitter)
{
    Ref<Gradient> gradient;
    gradient.instantiate();

    const Color color = emitter["color"];
    const Color tint = emitter["emitter_color"];
    const Array colors = emitter["color_keys"];
    const Array alphas = emitter["alpha_keys"];
    const double brightness = double(emitter["color_scale"]);
    const bool animate_color = int(emitter["color_type"]) == 2;
    const bool animate_alpha = int(emitter["alpha_type"]) == 2;

    // Merge the source color and alpha key times so independent curves stay aligned.
    std::vector<float> times = {0.0f, 1.0f};
    for (const Array& keys : {colors, alphas})
    {
        for (const Variant& entry : keys)
        {
            const Vector4 key = entry;
            times.push_back(std::clamp(float(key.w), 0.0f, 1.0f));
        }
    }
    std::sort(times.begin(), times.end());
    times.erase(std::unique(times.begin(), times.end()), times.end());

    PackedFloat32Array offsets;
    PackedColorArray values;
    for (float time : times)
    {
        const Vector3 rgb = animate_color ? sample_keys(colors, time, Vector3(color.r, color.g, color.b))
                                         : Vector3(color.r, color.g, color.b);
        const double alpha = animate_alpha ? sample_keys(alphas, time, Vector3(color.a, 0, 0)).x : color.a;
        offsets.push_back(time);
        values.push_back(Color(rgb.x * tint.r * brightness, rgb.y * tint.g * brightness,
                               rgb.z * tint.b * brightness, std::clamp(alpha * tint.a, 0.0, 1.0)));
    }

    gradient->set_offsets(offsets);
    gradient->set_colors(values);
    return gradient;
}

Ref<Curve> scale_curve(const Array& keys)
{
    Ref<Curve> curve;
    curve.instantiate();
    double maximum = 1.0;
    for (const Variant& entry : keys)
    {
        const Vector4 key = entry;
        maximum = std::max(maximum, double(key.x));
    }
    curve->set_max_value(maximum);

    for (const Variant& entry : keys)
    {
        const Vector4 key = entry;
        curve->add_point(Vector2(std::clamp(double(key.w), 0.0, 1.0), std::max(double(key.x), 0.0)),
                         0, 0, Curve::TANGENT_LINEAR, Curve::TANGENT_LINEAR);
    }
    return curve;
}
}

void TrinityParticles::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("load_effect", "path", "emitter_set", "unit_scale", "frame_rate"),
                         &TrinityParticles::load_effect, DEFVAL(String()), DEFVAL(1.0), DEFVAL(60.0));
    ClassDB::bind_method(D_METHOD("convert", "archive", "emitter_set", "unit_scale", "frame_rate"),
                         &TrinityParticles::convert, DEFVAL(String()), DEFVAL(1.0), DEFVAL(60.0));
    ClassDB::bind_method(D_METHOD("play"), &TrinityParticles::play);
    ClassDB::bind_method(D_METHOD("stop", "clear"), &TrinityParticles::stop, DEFVAL(false));
    ClassDB::bind_method(D_METHOD("is_playing"), &TrinityParticles::is_playing);
    ClassDB::bind_method(D_METHOD("get_warnings"), &TrinityParticles::get_warnings);

    ADD_SIGNAL(MethodInfo("finished"));
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "warnings", PROPERTY_HINT_NONE, "",
                              PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_warnings");
}

Error TrinityParticles::load_effect(const String& path, const String& emitter_set, double unit_scale, double frame_rate)
{
    Ref<ParticleArchive> source = ResourceLoader::get_singleton()->load(path);
    ERR_FAIL_COND_V_MSG(source.is_null(), ERR_CANT_OPEN, "Couldn't load particle archive: " + path);
    return convert(source, emitter_set, unit_scale, frame_rate);
}

Error TrinityParticles::convert(const Ref<ParticleArchive>& source, const String& emitter_set,
                                double unit_scale, double frame_rate)
{
    ERR_FAIL_COND_V(source.is_null(), ERR_INVALID_PARAMETER);
    ERR_FAIL_COND_V(!std::isfinite(unit_scale) || unit_scale <= 0.0 ||
                    !std::isfinite(frame_rate) || frame_rate <= 0.0, ERR_INVALID_PARAMETER);

    Dictionary selected;
    for (const Variant& entry : source->get_emitter_sets())
    {
        Dictionary candidate = entry;
        if (emitter_set.is_empty() || String(candidate["name"]) == emitter_set)
        {
            selected = candidate;
            break;
        }
    }
    ERR_FAIL_COND_V_MSG(selected.is_empty(), ERR_DOES_NOT_EXIST, "Particle emitter set not found: " + emitter_set);

    clear_emitters();
    archive = source;
    warnings = source->get_warnings();
    const Array definitions = selected["emitters"];
    for (const Variant& definition : definitions)
    {
        convert_emitter(definition, unit_scale, frame_rate);
    }
    set_process(false);
    return OK;
}

void TrinityParticles::warn(const Dictionary& emitter, const String& message)
{
    warnings.push_back(String(emitter["name"]) + ": " + message);
}

void TrinityParticles::convert_emitter(const Dictionary& emitter, double unit_scale, double frame_rate)
{
    const Array children = emitter["children"];
    if (!children.is_empty())
    {
        warn(emitter, "Particle-spawned child emitters are not converted");
    }
    const PackedStringArray extensions = emitter["extensions"];
    for (const String& extension : extensions)
    {
        warn(emitter, "Unconverted extension " + extension);
    }

    const int billboard = emitter["billboard"];
    if (billboard >= 5 && billboard <= 7)
    {
        warn(emitter, "Ribbon/stripe and primitive meshes are not converted");
        return;
    }
    if (int(emitter["fragment_mode"]) != 0)
    {
        warn(emitter, "Screen refraction/distortion shaders are not converted");
        return;
    }

    const Array samplers = emitter["samplers"];
    for (int i = 1; i < samplers.size(); ++i)
    {
        const Dictionary sampler = samplers[i];
        if (has_texture_id(sampler["id"]))
        {
            warn(emitter, "Secondary texture combiners are not converted");
            return;
        }
    }

    if (!bool(emitter["draw"]) || double(emitter["rate"]) <= 0.0)
    {
        return;
    }
    const bool infinite_life = emitter["infinite_life"];
    if (infinite_life)
    {
        warn(emitter, "Infinite particle lifetime is emulated by repeating its authored life until stopped");
    }

    CPUParticles3D* particles = memnew(CPUParticles3D);
    particles->set_name(String(emitter["name"]).validate_node_name());
    particles->set_emitting(false);
    particles->set_cast_shadows_setting(GeometryInstance3D::SHADOW_CASTING_SETTING_OFF);
    particles->set_position(Vector3(emitter["position"]) * unit_scale);
    particles->set_rotation(emitter["rotation"]);
    particles->set_scale(emitter["scale"]);
    particles->set_use_local_coordinates(int(emitter["follow_type"]) != 1);

    const double lifetime = std::max(double(emitter["life_frames"]), 1.0) / frame_rate;
    const double duration_frames = double(emitter["duration_frames"]);
    const bool burst = bool(emitter["one_time"]) || duration_frames <= 1.0;
    const double rate = double(emitter["rate"]);
    const double interval = std::max(double(emitter["interval"]) + 1.0, 1.0);
    const double count = std::ceil(burst ? rate : rate * frame_rate * lifetime / interval);
    const int amount = int(std::clamp(count, 1.0, double(MAX_PARTICLES_PER_EMITTER)));
    if (count > MAX_PARTICLES_PER_EMITTER)
    {
        warn(emitter, "Particle capacity limited to 4096");
    }

    particles->set_amount(amount);
    particles->set_lifetime(lifetime);
    particles->set_lifetime_randomness(std::clamp(double(emitter["life_random"]) / (lifetime * frame_rate), 0.0, 1.0));
    particles->set_one_shot(burst && !infinite_life);
    particles->set_explosiveness_ratio(burst ? 1.0 : 0.0);
    particles->set_color_ramp(color_gradient(emitter));
    particles->set_gravity(Vector3(emitter["gravity"]) * unit_scale * frame_rate * frame_rate);

    const Vector3 direction = emitter["direction"];
    const double radial_speed = double(emitter["radial_velocity"]);
    const double directional_speed = double(emitter["direction_velocity"]);
    const double speed = (std::abs(radial_speed) + std::abs(directional_speed)) * unit_scale * frame_rate;
    particles->set_direction(direction.is_zero_approx() ? Vector3(0, 1, 0) : direction.normalized());
    particles->set_spread(radial_speed != 0.0 ? 180.0 : Math::rad_to_deg(double(emitter["spread_radians"])));
    particles->set_param_min(CPUParticles3D::PARAM_INITIAL_LINEAR_VELOCITY, speed);
    particles->set_param_max(CPUParticles3D::PARAM_INITIAL_LINEAR_VELOCITY, speed);
    if (radial_speed != 0.0 && directional_speed != 0.0)
    {
        warn(emitter, "Combined radial/directional velocity approximated by a spread");
    }

    const Vector3 radius = Vector3(emitter["radius"]) * Vector3(emitter["shape_scale"]) * unit_scale;
    const int shape = emitter["shape"];
    if (shape >= 4 && shape <= 7)
    {
        particles->set_emission_shape(shape == 7 ? CPUParticles3D::EMISSION_SHAPE_SPHERE
                                               : CPUParticles3D::EMISSION_SHAPE_SPHERE_SURFACE);
        const real_t sphere_radius = std::max({std::abs(radius.x), std::abs(radius.y),
                                               std::abs(radius.z), real_t(0.001)});
        particles->set_emission_sphere_radius(sphere_radius);
    }
    else if (shape == 10 || shape == 11 || shape == 14)
    {
        particles->set_emission_shape(CPUParticles3D::EMISSION_SHAPE_BOX);
        particles->set_emission_box_extents(radius.abs());
    }
    else if (shape != 0)
    {
        warn(emitter, "Emission shape approximated by a bounding box");
        particles->set_emission_shape(CPUParticles3D::EMISSION_SHAPE_BOX);
        particles->set_emission_box_extents(radius.abs());
    }

    const Array scaling = emitter["scale_keys"];
    if (!scaling.is_empty())
    {
        particles->set_param_curve(CPUParticles3D::PARAM_SCALE, scale_curve(scaling));
    }
    const Vector3 initial_rotation = emitter["initial_rotation"];
    const double angle = Math::rad_to_deg(initial_rotation.z);
    particles->set_param_min(CPUParticles3D::PARAM_ANGLE, angle);
    particles->set_param_max(CPUParticles3D::PARAM_ANGLE, angle);
    const Vector3 angular_velocity = emitter["angular_velocity"];
    const double rotation_speed = Math::rad_to_deg(angular_velocity.z) * frame_rate;
    particles->set_param_min(CPUParticles3D::PARAM_ANGULAR_VELOCITY, rotation_speed);
    particles->set_param_max(CPUParticles3D::PARAM_ANGULAR_VELOCITY, rotation_speed);

    Ref<StandardMaterial3D> material;
    material.instantiate();
    material->set_shading_mode(BaseMaterial3D::SHADING_MODE_UNSHADED);
    material->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
    material->set_flag(BaseMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
    material->set_flag(BaseMaterial3D::FLAG_DISABLE_DEPTH_TEST, !bool(emitter["depth_test"]));
    material->set_flag(BaseMaterial3D::FLAG_BILLBOARD_KEEP_SCALE, true);
    material->set_billboard_mode(billboard == 0 ? BaseMaterial3D::BILLBOARD_PARTICLES
                                               : (billboard == 8 ? BaseMaterial3D::BILLBOARD_FIXED_Y
                                                                 : BaseMaterial3D::BILLBOARD_DISABLED));
    const BaseMaterial3D::BlendMode blends[] = {BaseMaterial3D::BLEND_MODE_MIX, BaseMaterial3D::BLEND_MODE_ADD,
                                               BaseMaterial3D::BLEND_MODE_SUB, BaseMaterial3D::BLEND_MODE_MUL};
    const int blend = emitter["blend"];
    material->set_blend_mode(blends[std::clamp(blend, 0, 3)]);
    if (blend > 3)
    {
        warn(emitter, "Screen blend approximated by multiplication");
    }
    if (bool(emitter["alpha_test"]))
    {
        material->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA_SCISSOR);
        material->set_alpha_scissor_threshold(emitter["alpha_threshold"]);
    }

    if (!samplers.is_empty())
    {
        const Dictionary primary = samplers[0];
        const String id = primary["id"];
        Ref<Texture2D> texture = archive->get_texture(id);
        if (texture.is_valid())
        {
            if (int(emitter["alpha_source"]) == 0)
            {
                texture = alpha_mask_texture(texture);
            }
            material->set_texture(BaseMaterial3D::TEXTURE_ALBEDO, texture);
        }
        else if (has_texture_id(id))
        {
            warn(emitter, "Missing primary texture " + id);
        }
        material->set_flag(BaseMaterial3D::FLAG_USE_TEXTURE_REPEAT, int(primary["wrap_u"]) != 2);
        material->set_texture_filter(int(primary["filter"]) == 1 ? BaseMaterial3D::TEXTURE_FILTER_NEAREST
                                                                 : BaseMaterial3D::TEXTURE_FILTER_LINEAR_WITH_MIPMAPS);
    }
    if (int(emitter["texture_animation"]) != 0)
    {
        warn(emitter, "Texture pattern animation is not converted");
    }

    const Vector2 uv_scale = emitter["uv_scale"];
    material->set_uv1_scale(Vector3(uv_scale.x, uv_scale.y, 1));
    Ref<QuadMesh> mesh;
    mesh.instantiate();
    const Vector3 particle_scale = emitter["particle_scale"];
    mesh->set_size(Vector2(std::max(std::abs(particle_scale.x) * unit_scale, 0.001),
                          std::max(std::abs(particle_scale.y) * unit_scale, 0.001)));
    mesh->set_material(material);
    if (billboard == 2)
    {
        mesh->set_orientation(PlaneMesh::FACE_Y);
    }
    particles->set_mesh(mesh);
    add_child(particles);

    EmitterPlayback playback;
    playback.particles = particles;
    playback.material = material;
    playback.uv_offset = emitter["uv_offset"];
    playback.uv_scroll = Vector2(emitter["uv_scroll"]) * frame_rate;
    playback.start = double(emitter["start_frame"]) / frame_rate;
    playback.end = infinite_life || duration_frames >= 0x7fffffff
                       ? INFINITE : playback.start + std::max(duration_frames, 1.0) / frame_rate;
    playback.lifetime = lifetime;
    playback.burst = burst;
    emitters.push_back(playback);
}

void TrinityParticles::clear_emitters()
{
    playing = false;
    set_process(false);
    for (EmitterPlayback& emitter : emitters)
    {
        remove_child(emitter.particles);
        memdelete(emitter.particles);
    }
    emitters.clear();
}

void TrinityParticles::play()
{
    elapsed = 0.0;
    finish_time = 0.0;
    draining = false;
    playing = !emitters.empty();

    for (EmitterPlayback& emitter : emitters)
    {
        emitter.started = false;
        emitter.stopped = false;
        emitter.particles->show();
        emitter.particles->restart();
        emitter.particles->set_emitting(false);
        finish_time = std::max(finish_time, emitter.end + emitter.lifetime);
    }
    set_process(playing);
}

void TrinityParticles::stop(bool clear)
{
    draining = true;
    finish_time = elapsed;
    for (EmitterPlayback& emitter : emitters)
    {
        emitter.stopped = true;
        emitter.particles->set_emitting(false);
        if (clear)
        {
            emitter.particles->hide();
        }
        finish_time = std::max(finish_time, elapsed + (clear ? 0.0 : emitter.lifetime));
    }
    if (clear)
    {
        playing = false;
        set_process(false);
    }
}

void TrinityParticles::_process(double delta)
{
    if (!playing)
    {
        return;
    }
    elapsed += delta;

    for (EmitterPlayback& emitter : emitters)
    {
        if (!emitter.started && !draining && elapsed >= emitter.start)
        {
            emitter.started = true;
            emitter.particles->restart();
            emitter.particles->set_emitting(true);
        }
        if (!emitter.stopped && !emitter.burst && elapsed >= emitter.end)
        {
            emitter.stopped = true;
            emitter.particles->set_emitting(false);
        }
        const Vector2 offset = emitter.uv_offset + emitter.uv_scroll * std::max(elapsed - emitter.start, 0.0);
        emitter.material->set_uv1_offset(Vector3(offset.x, offset.y, 0));
    }

    if (elapsed >= finish_time)
    {
        playing = false;
        set_process(false);
        emit_signal("finished");
    }
}
