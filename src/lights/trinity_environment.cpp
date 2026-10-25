#include "trinity_environment.h"
#include "middleware/bntx.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/core/object.hpp>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <utility>

using namespace godot;

void TrinityEnvironment::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_time_of_day", "hours"), &TrinityEnvironment::set_time_of_day);
    ClassDB::bind_method(D_METHOD("get_time_of_day"), &TrinityEnvironment::get_time_of_day);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_of_day", PROPERTY_HINT_RANGE, "0,24,0.01,suffix:h"), "set_time_of_day", "get_time_of_day");

    ClassDB::bind_method(D_METHOD("set_master_light", "light"), &TrinityEnvironment::set_master_light);
    ClassDB::bind_method(D_METHOD("get_master_light"), &TrinityEnvironment::get_master_light);

    ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &TrinityEnvironment::set_enabled);
    ClassDB::bind_method(D_METHOD("is_enabled"), &TrinityEnvironment::is_enabled);

    ClassDB::bind_method(D_METHOD("set_use_native_bloom", "enabled"), &TrinityEnvironment::set_use_native_bloom);
    ClassDB::bind_method(D_METHOD("get_use_native_bloom"), &TrinityEnvironment::get_use_native_bloom);

    ClassDB::bind_method(D_METHOD("set_lighting_category", "category"), &TrinityEnvironment::set_lighting_category);
    ClassDB::bind_method(D_METHOD("get_lighting_category"), &TrinityEnvironment::get_lighting_category);

    ClassDB::bind_method(D_METHOD("set_world_environment_path", "path"), &TrinityEnvironment::set_world_environment_path);
    ClassDB::bind_method(D_METHOD("get_world_environment_path"), &TrinityEnvironment::get_world_environment_path);

    ClassDB::bind_method(D_METHOD("set_camera_path", "path"), &TrinityEnvironment::set_camera_path);
    ClassDB::bind_method(D_METHOD("get_camera_path"), &TrinityEnvironment::get_camera_path);

    ClassDB::bind_method(D_METHOD("set_layer", "name", "light", "priority", "weight", "additive"), &TrinityEnvironment::set_layer, DEFVAL(0), DEFVAL(1.0), DEFVAL(false));
    ClassDB::bind_method(D_METHOD("set_layer_weight", "name", "weight"), &TrinityEnvironment::set_layer_weight);
    ClassDB::bind_method(D_METHOD("remove_layer", "name"), &TrinityEnvironment::remove_layer);
    ClassDB::bind_method(D_METHOD("clear_layers"), &TrinityEnvironment::clear_layers);

    ClassDB::bind_method(D_METHOD("apply"), &TrinityEnvironment::apply);
    ClassDB::bind_method(D_METHOD("get_effective_state"), &TrinityEnvironment::get_effective_state);
    ClassDB::bind_method(D_METHOD("get_environment"), &TrinityEnvironment::get_environment);
    ClassDB::bind_method(D_METHOD("get_camera_attributes"), &TrinityEnvironment::get_camera_attributes);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "master_light", PROPERTY_HINT_RESOURCE_TYPE, "TRLGT"), "set_master_light", "get_master_light");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_native_bloom"), "set_use_native_bloom", "get_use_native_bloom");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lighting_category", PROPERTY_HINT_RANGE, "-1,7,1"), "set_lighting_category", "get_lighting_category");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "world_environment_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "WorldEnvironment"), "set_world_environment_path", "get_world_environment_path");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "camera_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Camera3D"), "set_camera_path", "get_camera_path");

    ADD_SIGNAL(MethodInfo("environment_applied"));
}

namespace
{
// Shader globals are shared, like the existing sky globals. Only the last
// controller to publish a probe may clear it when detaching.
uint64_t active_probe_owner = 0;

template <class T>
T *instance(uint64_t id)
{
    return id ? Object::cast_to<T>(ObjectDB::get_instance(id)) : nullptr;
}

Vector3 xyz(Vector4 v)
{
    return Vector3(v.x, v.y, v.z);
}

void rgb_to_hsv(float r, float g, float b, float &h, float &s, float &v)
{
    float k = 0.f;

    if (g < b)
    {
        std::swap(g, b);
        k = -1.f;
    }

    if (r < g)
    {
        std::swap(r, g);
        k = -2.f / 6.f - k;
    }

    const float chroma = r - (g < b ? g : b);
    h = std::fabs(k + (g - b) / (6.f * chroma + 1e-20f));
    s = chroma / (r + 1e-20f);
    v = r;
}

Vector3 hsv_to_rgb(float h, float s, float v)
{
    if (s < FLT_EPSILON)
        return Vector3(v, v, v);

    h = std::fmod(h, 1.0f) / (60.0f / 360.0f);
    int i = static_cast<int>(h);
    float f = h - static_cast<float>(i);
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i)
    {
        case 0: return Vector3(v, t, p);
        case 1: return Vector3(q, v, p);
        case 2: return Vector3(p, v, t);
        case 3: return Vector3(p, q, v);
        case 4: return Vector3(t, p, v);
        default: return Vector3(v, p, q);
    }
}

Vector4 offset_color(Vector4 current, Vector4 offset)
{
    float h, s, v;
    rgb_to_hsv(current.x, current.y, current.z, h, s, v);
    Vector3 rgb = hsv_to_rgb(h + offset.x, s + offset.y, v + offset.z);
    return Vector4(rgb.x, rgb.y, rgb.z, current.w + offset.w);
}

// Resets every stored property of `to` to the value in `from`, so properties the
// light stack stops driving fall back to the user's baseline resource.
void copy_properties(const Object *from, Object *to)
{
    Array properties = from->get_property_list();

    for (int i = 0; i < properties.size(); ++i)
    {
        Dictionary property = properties[i];
        StringName name = property["name"];

        if ((int(property["usage"]) & PROPERTY_USAGE_STORAGE) && name != StringName("resource_path") &&
            name != StringName("script"))
            to->set(name, from->get(name));
    }
}
}

void TrinityEnvironment::_notification(int what)
{
    if (what == NOTIFICATION_READY)
        apply();

    if (what == NOTIFICATION_EXIT_TREE)
    {
        detach_targets();
        request_ready();
    }
}

void TrinityEnvironment::set_time_of_day(double hours)
{
    ERR_FAIL_COND_MSG(!std::isfinite(hours), "Time of day must be finite.");

    time_of_day = std::fmod(std::fmod(hours, 24.0) + 24.0, 24.0);
    apply();
}

void TrinityEnvironment::set_master_light(const Ref<TRLGT> &light)
{
    master_light = light;
    apply();
}

void TrinityEnvironment::set_enabled(bool value)
{
    enabled = value;
    apply();
}

void TrinityEnvironment::set_use_native_bloom(bool value)
{
    use_native_bloom = value;
    apply();
}

void TrinityEnvironment::set_lighting_category(int category)
{
    ERR_FAIL_COND(category < -1 || category > 7);

    lighting_category = category;
    apply();
}

void TrinityEnvironment::set_world_environment_path(const NodePath &path)
{
    detach_targets();
    world_environment_path = path;
    apply();
}

void TrinityEnvironment::set_camera_path(const NodePath &path)
{
    detach_targets();
    camera_path = path;
    apply();
}

void TrinityEnvironment::set_layer(const StringName &name, const Ref<TRLGT> &light, int priority, double weight, bool additive)
{
    ERR_FAIL_COND_MSG(!std::isfinite(weight) || weight < 0 || weight > 1, "Layer weight must be in [0, 1].");
    ERR_FAIL_COND(light.is_null());

    for (auto &layer : layers)
        if (layer.name == name)
        {
            layer.light = light;
            layer.priority = priority;
            layer.weight = weight;
            layer.additive = additive;
            apply();
            return;
        }

    layers.push_back({name, light, priority, weight, additive});
    apply();
}

void TrinityEnvironment::set_layer_weight(const StringName &name, double weight)
{
    ERR_FAIL_COND_MSG(!std::isfinite(weight) || weight < 0 || weight > 1, "Layer weight must be in [0, 1].");

    for (auto &layer : layers)
        if (layer.name == name)
        {
            layer.weight = weight;
            apply();
            return;
        }
}

void TrinityEnvironment::remove_layer(const StringName &name)
{
    layers.erase(
        std::remove_if(layers.begin(), layers.end(), [&](const Layer &l) { return l.name == name; }),
        layers.end());
    apply();
}

void TrinityEnvironment::clear_layers()
{
    layers.clear();
    apply();
}

// Applies one light file at a fixed transition progress (`weight`):
// override layers lerp each parameter they contain from the stack below;
// additive (offset) layers add weight * delta, and only to parameters that
// already exist below.
void TrinityEnvironment::resolve_resource(const Ref<TRLGT> &resource, double weight, bool additive)
{
    if (resource.is_null() || weight <= 0)
        return;

    Array lights = resource->get_lights();

    for (int i = 0; i < lights.size(); ++i)
    {
        Ref<TRLight> light = lights[i];

        if (light.is_null())
            continue;

        String name = light->get_Name();
        Dictionary entry = effective_state.get(name, Dictionary());
        entry["name"] = name;

        // A light keeps the type it was first defined with (normally by the master).
        if (!entry.has("type"))
            entry["type"] = light->get_Type();

        Dictionary floats = entry.get("floats", Dictionary());
        Dictionary vectors = entry.get("vectors", Dictionary());
        Dictionary textures = entry.get("textures", Dictionary());
        Dictionary texture_paths = entry.get("texture_paths", Dictionary());

        Array params = light->get_float_params();

        for (int j = 0; j < params.size(); ++j)
        {
            Ref<TRFloatParameter> p = params[j];

            if (p.is_null() || !std::isfinite(p->sample(time_of_day)))
                continue;

            String key = p->get_param_name();
            double value = p->sample(time_of_day);

            if (additive)
            {
                if (!floats.has(key))
                    continue;

                value = double(floats[key]) + value * weight;
            }
            else if (floats.has(key))
            {
                double old = floats[key];
                value = old + (value - old) * weight;
            }

            floats[key] = value;
        }

        params = light->get_vec4_params();

        for (int j = 0; j < params.size(); ++j)
        {
            Ref<TRVec4Parameter> p = params[j];

            if (p.is_null() || !p->sample(time_of_day).is_finite())
                continue;

            String key = p->get_param_name();
            Vector4 value = p->sample(time_of_day);

            if (additive)
            {
                if (!vectors.has(key))
                    continue;

                value = offset_color(vectors[key], value * weight);
            }
            else if (vectors.has(key))
                value = Vector4(vectors[key]).lerp(value, weight);

            vectors[key] = value;
        }

        params = light->get_string_params();

        for (int j = 0; j < params.size(); ++j)
        {
            Ref<TRStringParameter> p = params[j];

            if (p.is_null())
                continue;

            // Textures aren't interpolated; a layer's textures replace those below as soon as it applies.
            String file = p->get_map_file();
            textures[p->get_map_name()] = file;
            texture_paths[p->get_map_name()] = (file.is_absolute_path() ? file : resource->get_path().get_base_dir().path_join(file)).simplify_path();
        }

        Ref<BoundMatrix> matrix = light->sample_matrix(time_of_day);

        if (matrix.is_valid())
        {
            Vector3 scale = xyz(matrix->get_x());
            Vector3 translation = xyz(matrix->get_z());

            // Static rotations are YXZ Euler angles; curve-driven rotations are ZYX.
            Quaternion rotation = Basis::from_euler(
                xyz(matrix->get_y()),
                light->get_rotation_curve().is_valid() ? EULER_ORDER_ZYX : EULER_ORDER_YXZ).get_rotation_quaternion();

            if (additive)
            {
                // Offsets transition from an identity SRT and are then combined
                // with the current transform (which defaults to identity too).
                scale = Vector3(1, 1, 1).lerp(scale, weight);
                rotation = Quaternion().slerp(rotation, weight);
                translation = Vector3().lerp(translation, weight);

                scale += Vector3(entry.get("scale", Vector3(1, 1, 1)));
                rotation = Quaternion(entry.get("rotation", Quaternion())) * rotation;
                translation += Vector3(entry.get("translation", Vector3()));
            }
            else if (entry.has("rotation"))
            {
                rotation = Quaternion(entry["rotation"]).slerp(rotation, weight);
                scale = Vector3(entry["scale"]).lerp(scale, weight);
                translation = Vector3(entry["translation"]).lerp(translation, weight);
            }

            entry["scale"] = scale;
            entry["rotation"] = rotation;
            entry["translation"] = translation;
        }

        entry["floats"] = floats;
        entry["vectors"] = vectors;
        entry["textures"] = textures;
        entry["texture_paths"] = texture_paths;

        effective_state[name] = entry;
    }
}

Dictionary TrinityEnvironment::find_type(const String &type) const
{
    Array names = effective_state.keys();

    for (int i = 0; i < names.size(); ++i)
    {
        Dictionary entry = effective_state[names[i]];

        if (String(entry.get("type", "")) == type)
            return entry;
    }

    return Dictionary();
}

// Selectable lights are matched by their CategoryLabel (default 0), not
// by name; a later light with the same label replaces an earlier one.
Dictionary TrinityEnvironment::find_category(int category) const
{
    Dictionary found;
    Array names = effective_state.keys();

    for (int i = 0; i < names.size(); ++i)
    {
        Dictionary entry = effective_state[names[i]];

        if (String(entry.get("type", "")) == "Selectable" && int(number(entry, "CategoryLabel", 0)) == category)
            found = entry;
    }

    return found;
}

double TrinityEnvironment::number(const Dictionary &light, const String &name, double fallback) const
{
    Dictionary values = light.get("floats", Dictionary());

    if (!values.has(name))
        return fallback;

    return values[name];
}

Color TrinityEnvironment::color(const Dictionary &light, const String &name, Color fallback, bool source_is_linear) const
{
    Dictionary values = light.get("vectors", Dictionary());

    if (!values.has(name))
        return fallback;

    Vector4 v = values[name];

    // Alpha means different things across Ikkaku parameters, so it is never used as
    // transparency/energy for native light colors (Selectable Color.w can be 0).
    // Sky/fog vectors are used as stored, while Directional/Selectable Color is
    // authored in sRGB.
    // Godot's native light properties already do that conversion; don't undo it for lights.
    Color value(v.x, v.y, v.z, 1);
    return source_is_linear ? value.linear_to_srgb() : value;
}

bool TrinityEnvironment::probe_average(const Dictionary &probe, Color &average)
{
    Dictionary paths = probe.get("texture_paths", Dictionary());
    String path = paths.get("DiffuseProbe", "");

    if (path.is_empty())
        return false;

    if (diffuse_probe_cache.has(path))
    {
        Variant cached = diffuse_probe_cache[path];

        if (cached.get_type() != Variant::COLOR)
            return false;

        average = cached;
        return true;
    }

    // Cache failures too: missing assets must not trigger repeated loads while
    // dragging the time slider. A new controller will retry after assets change.
    diffuse_probe_cache[path] = Variant();

    if (!ResourceLoader::get_singleton()->exists(path))
        return false;

    Ref<BinaryTextureArchive> archive = ResourceLoader::get_singleton()->load(path);

    if (archive.is_null())
        return false;

    auto textures = archive->get_textures();
    Ref<BinaryTexture> texture = archive->GetTexture(path.get_file().get_basename());

    if (texture.is_null() && textures.size() == 1)
        texture = textures.values()[0];

    if (texture.is_null() || texture->get_layer_image(5).is_null() || texture->get_layer_image(6).is_valid())
        return false;

    Vector4i swizzle = texture->get_channel_sources();

    if (swizzle.x < 0 || swizzle.x > 5 || swizzle.y < 0 || swizzle.y > 5 || swizzle.z < 0 || swizzle.z > 5)
        return false;

    double red = 0, green = 0, blue = 0, total = 0;
    int size = 0;

    for (int face = 0; face < 6; ++face)
    {
        Ref<Image> original = texture->get_layer_image(face);

        if (original.is_null() || original->get_width() != original->get_height() || original->get_width() <= 0)
            return false;

        if (face > 0 && original->get_width() != size)
            return false;

        size = original->get_width();

        // Decompress a private copy; do not mutate the cached texture's images.
        Ref<Image> image = original->duplicate();

        if (image->is_compressed() && image->decompress() != OK)
            return false;

        for (int y = 0; y < size; ++y)
            for (int x = 0; x < size; ++x)
            {
                Color c = image->get_pixel(x, y);

                if (!std::isfinite(c.r) || !std::isfinite(c.g) || !std::isfinite(c.b))
                    return false;

                float channels[6] = {0, 1, c.r, c.g, c.b, c.a};
                double u = 2.0 * (x + 0.5) / size - 1.0;
                double v = 2.0 * (y + 0.5) / size - 1.0;
                double weight = 1.0 / std::pow(1.0 + u*u + v*v, 1.5);

                red += MAX(0.0f, channels[swizzle.x]) * weight;
                green += MAX(0.0f, channels[swizzle.y]) * weight;
                blue += MAX(0.0f, channels[swizzle.z]) * weight;
                total += weight;
            }
    }

    // L0 of the source's already-filtered diffuse irradiance cube. This keeps its
    // energy/tint, but native uniform ambient cannot keep angular detail.
    average = Color(red / total, green / total, blue / total, 1);
    diffuse_probe_cache[path] = average;
    return true;
}

void TrinityEnvironment::apply_probe_ambient(const Ref<Environment> &environment)
{
    RenderingServer *rs = RenderingServer::get_singleton();
    active_probe_owner = get_instance_id();
    rs->global_shader_parameter_set("TrinityDiffuseProbeEnabled", false);
    Dictionary gi = find_type("GILightCommon");
    Color lightmap_gain = color(gi, "LightMapColor", Color(1,1,1)).srgb_to_linear() *
        MAX(0.0, number(gi, "LightMapIntensity", 1));
    rs->global_shader_parameter_set("TrinityLightMapGain", Vector3(lightmap_gain.r, lightmap_gain.g, lightmap_gain.b));
    Dictionary probe = find_type("Probe");
    Color average;

    if (!probe_average(probe, average))
        return;

    Color tint = color(probe, "DiffuseProbeColor", Color(1,1,1)).srgb_to_linear();
    double energy = CLAMP(number(probe, "DiffuseProbeIntensity", 1), 0.0, 100.0);

    tint *= color(gi, "Color", Color(1,1,1)).srgb_to_linear();
    energy *= CLAMP(number(gi, "Intensity", 1), 0.0, 100.0);

    if (lighting_category >= 0)
    {
        String suffix = String::num_int64(lighting_category);
        tint *= color(gi, "Color" + suffix, Color(1,1,1)).srgb_to_linear();
        energy *= CLAMP(number(gi, "Intensity" + suffix, 1), 0.0, 100.0);
    }

    Dictionary paths = probe.get("texture_paths", Dictionary());
    Ref<Cubemap> diffuse = load_cubemap(paths.get("DiffuseProbe", ""));
    // Retail's deferred light pass divides the probe irradiance by pi (Lambert);
    // Godot's ambient and IRRADIANCE are used as-is.
    Color gain = tint * (energy / Math_PI);
    set_global_texture("TrinityDiffuseProbe", diffuse);
    rs->global_shader_parameter_set("TrinityDiffuseProbeGain", Vector3(gain.r, gain.g, gain.b));
    rs->global_shader_parameter_set("TrinityDiffuseProbeEnabled", diffuse.is_valid());

    // Native materials keep the averaged fallback. Shared Trinity shaders
    // replace it with directional irradiance through Godot's IRRADIANCE output.
    Color ambient = average * gain;
    ambient.a = 1;

    environment->set_ambient_source(Environment::AMBIENT_SOURCE_COLOR);
    environment->set_ambient_light_color(ambient.linear_to_srgb());
    // Godot also multiplies sky reflections by ambient energy. Bake the diffuse
    // gain into the fallback color so it cannot rescale the specular probe.
    environment->set_ambient_light_energy(1.0);
    environment->set_ambient_light_sky_contribution(0);
}

// Metallic surfaces get no diffuse light, so without the specular probe they render black.
// Godot only takes reflections from the Environment sky, so the probe cube is fed through
// a sky shader; the visible sky stays the SkySimple dome drawn in front of it.
void TrinityEnvironment::apply_probe_specular(const Ref<Environment> &environment)
{
    Dictionary probe = find_type("Probe");
    Dictionary paths = probe.get("texture_paths", Dictionary());
    Ref<Cubemap> cubemap = load_cubemap(paths.get("SpecularProbe", ""));

    if (cubemap.is_null())
        return;

    if (probe_sky.is_null())
    {
        Ref<Shader> shader = ResourceLoader::get_singleton()->load("res://gflib/shaders/SpecularProbe.gdshader");

        if (shader.is_null())
            return;

        probe_sky_material.instantiate();
        probe_sky_material->set_shader(shader);
        probe_sky.instantiate();
        probe_sky->set_material(probe_sky_material);
        probe_sky->set_radiance_size(Sky::RADIANCE_SIZE_128);
        // The probe tint follows time-of-day curves, so spread each re-filter over frames.
        probe_sky->set_process_mode(Sky::PROCESS_MODE_INCREMENTAL);
    }

    Color tint = color(probe, "SpecularProbeColor", Color(1,1,1)).srgb_to_linear();

    probe_sky_material->set_shader_parameter("SpecularProbe", cubemap);
    probe_sky_material->set_shader_parameter("SpecularProbeColor", Vector3(tint.r, tint.g, tint.b));
    probe_sky_material->set_shader_parameter("SpecularProbeIntensity", CLAMP(number(probe, "SpecularProbeIntensity", 1), 0.0, 100.0));
    probe_sky_material->set_shader_parameter("SpecularProbeSaturation", number(probe, "SpecularProbeSaturation", 1));

    environment->set_sky(probe_sky);
    environment->set_reflection_source(Environment::REFLECTION_SOURCE_SKY);
}

Ref<Cubemap> TrinityEnvironment::load_cubemap(const String &path)
{
    const String key = path + String("\ncube");

    if (texture_cache.has(key))
        return texture_cache[key];

    texture_cache[key] = Variant();
    ResourceLoader *loader = ResourceLoader::get_singleton();

    if (path.is_empty() || !loader->exists(path))
        return Ref<Cubemap>();

    Ref<BinaryTextureArchive> archive = loader->load(path);

    if (archive.is_null())
        return Ref<Cubemap>();

    auto textures = archive->get_textures();
    Ref<BinaryTexture> texture = archive->GetTexture(path.get_file().get_basename());

    if (texture.is_null() && textures.size() == 1)
        texture = textures.values()[0];

    if (texture.is_null() || texture->get_layers() != 6)
        return Ref<Cubemap>();

    TypedArray<Image> faces;

    for (int face = 0; face < 6; ++face)
    {
        Ref<Image> image = texture->get_layer_image(face);

        if (image.is_null())
            return Ref<Cubemap>();

        faces.push_back(image);
    }

    Ref<Cubemap> result;
    result.instantiate();

    if (result->create_from_images(faces) != OK)
        return Ref<Cubemap>();

    texture_cache[key] = result;
    return result;
}

// The LUT is rebuilt only when the PfxCgCurve parameters change (photo filters animate them).
Ref<ImageTexture3D> TrinityEnvironment::retail_tonemap_lut(const Dictionary &global)
{
    PackedFloat64Array key;
    key.push_back(number(global, "PfxCgCurveEnabled", 0));

    for (const char *name : {"PfxCgToeStrength", "PfxCgToeLength", "PfxCgShoulderStrength",
        "PfxCgShoulderLength", "PfxCgShoulderAngle", "PfxCgCurveGamma"})
        key.push_back(number(global, name, 0));

    if (tonemap_lut.is_valid() && key == tonemap_lut_key)
        return tonemap_lut;

    RetailToneCurve curve;

    if (key[0] > 0)
        curve = RetailToneCurve::create(key[1], key[2], key[3], key[4], key[5], key[6]);

    tonemap_lut = build_retail_tonemap_lut(curve, 96, RETAIL_TONEMAP_HEADROOM);
    tonemap_lut_key = key;
    return tonemap_lut;
}

// Retail's baked top-down world shadow, applied by trinity_light.gdshaderinc beyond the
// cascade range. Its RGBA channels hold the sun at the centers of four equal steps across
// [WorldShadowTimeStart, WorldShadowTimeEnd], which wraps past midnight for moon shadows.
void TrinityEnvironment::update_world_shadow()
{
    RenderingServer *rs = RenderingServer::get_singleton();
    Dictionary global = find_type("Global");
    Dictionary directional = find_type("Directional");
    Dictionary paths = global.get("texture_paths", Dictionary());
    Ref<Texture2D> map;

    if (number(global, "WorldShadowEnable", 0) > 0)
        map = load_texture(paths.get("WorldShadowTexture", ""));

    double start = number(directional, "WorldShadowTimeStart", 4);
    double span = Math::fposmod(number(directional, "WorldShadowTimeEnd", 20) - start, 24.0);
    double local = Math::fposmod(time_of_day - start, 24.0);
    double intensity = 0;
    Vector4 weights;

    if (map.is_valid() && span > 0 && local <= span)
    {
        double step = CLAMP(local / span * 4.0 - 0.5, 0.0, 3.0);
        int index = MIN(int(step), 2);
        weights[index] = 1.0 - (step - index);
        weights[index + 1] = step - index;
        intensity = CLAMP(number(global, "WorldShadowIntensity", 1), 0.0, 1.0);
        set_global_texture("TrinityWorldShadowMap", map);
    }

    // Aligned against the field geometry: OrthoSize 500 maps the texture onto X/Z in [-1000, 0].
    double size = 2.0 * MAX(1.0, number(directional, "WorldShadowOrthoSize", 500));
    double offset_x = number(global, "WorldShadowPositionOffsetX", 0);
    double offset_z = number(global, "WorldShadowPositionOffsetZ", 0);

    rs->global_shader_parameter_set("TrinityWorldShadowWeights", weights);
    rs->global_shader_parameter_set("TrinityWorldShadowTransform",
        Vector4(1.0 / size, 1.0 / size, offset_x / size + 1.0, offset_z / size + 1.0));
    rs->global_shader_parameter_set("TrinityWorldShadowParams",
        Vector4(MAX(0.01, number(global, "WorldShadowFadeDistance", 40)), number(global, "WorldShadowFadePow", 10), intensity, 0));
}

// Failures are cached too, so a missing file isn't retried on every time-of-day step.
Ref<Texture2D> TrinityEnvironment::load_texture(const String &path)
{
    if (texture_cache.has(path))
        return texture_cache[path];

    texture_cache[path] = Variant();
    ResourceLoader *loader = ResourceLoader::get_singleton();

    if (path.is_empty() || !loader->exists(path))
        return Ref<Texture2D>();

    Ref<BinaryTextureArchive> archive = loader->load(path);

    if (archive.is_null())
        return Ref<Texture2D>();

    auto textures = archive->get_textures();
    Ref<BinaryTexture> texture = archive->GetTexture(path.get_file().get_basename());

    if (texture.is_null() && textures.size() == 1)
        texture = textures.values()[0];

    if (texture.is_null() || texture->get_layers() == 0)
        return Ref<Texture2D>();

    Ref<Image> image = texture->get_layer_image(0);

    if (image.is_null())
        return Ref<Texture2D>();

    Ref<Texture2D> result = ImageTexture::create_from_image(image);
    texture_cache[path] = result;
    return result;
}

// Uniform names match the Sky light's parameters; fallbacks match project.godot.
void TrinityEnvironment::update_sky(const Dictionary &sky_data)
{
    RenderingServer *rs = RenderingServer::get_singleton();

    static const struct { const char *name; double fallback; } floats[] = {
        {"SkyHorizonFalloff", 1.0}, {"SkyCloudRim", 1.0}, {"SkyCloudsFadeOut", -0.3},
        {"SkyTopCloudSpeed", 1.0}, {"SkyTopBreakupSpeed", 1.0}, {"SkyTopPlaneHeight", 1.0},
        {"SkyTopPlaneHorizon", 1.0}, {"SkyTopPlaneTiling", 1.0}, {"SkyTopCloudCoverage", 1.0},
        {"SkySunRadius", 1.0}, {"SkySunShine", 1.0}, {"SkySunBrightness", 1.0},
        {"SkyMoonScale", 1.0}, {"SkyMoonPhase", 1.0}, {"SkyMoonShadingRotation", 1.0},
        {"SkyMoonBrightness", 1.0}, {"SkyStarsTiling", 1.0}, {"SkyStarsHorizonLift", 1.0},
        {"SkyStarsIntensity", 0.0}, {"SkyStarsMaskTiling", 1.0}, {"SkyStarsMaskSpeed", 0.0},
        {"SkyFogIntensity", 1.0}, {"SkyFogOffset", 1.0}, {"SkyFogGradation", 1.0},
        {"ScatteringFogFactor", 0.7},
    };

    for (const auto &f : floats)
        rs->global_shader_parameter_set(f.name, number(sky_data, f.name, f.fallback));

    // Used as stored; no sRGB conversion.
    static const char *colors[] = {
        "SkyZenithColor", "SkyHorizonColor", "SkyCloudColor", "SkyCloudColorRim", "SkySunTintColor", "SkyFogColor",
    };
    Dictionary vectors = sky_data.get("vectors", Dictionary());

    for (const char *name : colors)
        rs->global_shader_parameter_set(name, Vector4(vectors.get(name, Vector4(1, 1, 1, 1))));

    // ScatteringFogColor.w scales the whole color (including w, which then gates the term).
    Vector4 scattering = vectors.get("ScatteringFogColor", Vector4(1, 1, 1, 0));
    scattering *= scattering.w;

    Dictionary global_paths = find_type("Global").get("texture_paths", Dictionary());
    Ref<Texture2D> lut = load_texture(global_paths.get("AtmosphereLUT", ""));

    if (lut.is_null())
        scattering.w = 0;

    set_global_texture("AtmosphereLUT", lut);
    rs->global_shader_parameter_set("ScatteringFogColor", scattering);

    // Pitch is in units of 90 degrees (positive = below the horizon), yaw in units of 180.
    auto direction = [](double pitch, double yaw) {
        pitch *= 0.5 * Math_PI;
        yaw *= Math_PI;
        return Vector3(std::sin(yaw) * std::cos(pitch), -std::sin(pitch), std::cos(yaw) * std::cos(pitch));
    };

    Vector3 sun = direction(number(sky_data, "SkySunPitch", 0), number(sky_data, "SkySunYaw", 0));

    if (number(sky_data, "SkySunUseDirectionalLight", 0) > 0)
    {
        // Toward the light: DirectionalMain shines along its -Z.
        Quaternion rotation = find_type("Directional").get("rotation", Quaternion());
        sun = rotation.xform(Vector3(0, 0, 1));
    }

    rs->global_shader_parameter_set("SkySunDirection", sun);
    rs->global_shader_parameter_set("SkyMoonDirection",
        direction(number(sky_data, "SkyMoonPitch", 0), number(sky_data, "SkyMoonYaw", 0)));
}

// Setting a global texture invalidates every material that samples it (tens of thousands
// in the field), so only push actual changes. Time-of-day updates happen every frame.
void TrinityEnvironment::set_global_texture(const StringName &name, const Ref<Texture> &texture)
{
    if (global_textures.has(name) && Ref<Texture>(global_textures[name]) == texture)
        return;

    global_textures[name] = texture;
    RenderingServer::get_singleton()->global_shader_parameter_set(name, texture);
}

void TrinityEnvironment::detach_targets()
{
    if (active_probe_owner == get_instance_id())
    {
        RenderingServer *rs = RenderingServer::get_singleton();
        rs->global_shader_parameter_set("TrinityDiffuseProbeEnabled", false);
        set_global_texture("TrinityDiffuseProbe", Ref<Texture>());
        rs->global_shader_parameter_set("TrinityLightMapGain", Vector3(1, 1, 1));
        active_probe_owner = 0;
    }
    WorldEnvironment *world = instance<WorldEnvironment>(bound_world_id);

    if (world)
    {
        if (world->get_environment() == applied_environment)
            world->set_environment(original_environment);

        if (world->get_camera_attributes() == applied_attributes)
            world->set_camera_attributes(original_world_attributes);
    }

    Camera3D *camera = instance<Camera3D>(bound_camera_id);

    if (camera && camera->get_attributes() == applied_attributes)
        camera->set_attributes(original_camera_attributes);

    if (auto *light = instance<DirectionalLight3D>(owned_light_id))
        light->hide();

    bound_world_id = bound_camera_id = 0;

    original_environment.unref();
    original_world_attributes.unref();
    original_camera_attributes.unref();
    applied_environment.unref();
    applied_attributes.unref();
    baseline_environment.unref();
    baseline_attributes.unref();
}

void TrinityEnvironment::apply()
{
    if (applying)
        return;

    applying = true;
    effective_state.clear();

    resolve_resource(master_light, 1.0, false);

    auto ordered = layers;
    std::stable_sort(ordered.begin(), ordered.end(), [](const Layer &a, const Layer &b) { return a.priority < b.priority; });

    for (const auto &layer : ordered)
        resolve_resource(layer.light, layer.weight, layer.additive);

    if (is_inside_tree())
    {
        if (enabled && !effective_state.is_empty())
            apply_render_state();
        else
            detach_targets();
    }

    applying = false;
    emit_signal("environment_applied");
}

void TrinityEnvironment::apply_render_state()
{
    WorldEnvironment *world = nullptr;
    Camera3D *camera = nullptr;

    if (!world_environment_path.is_empty())
    {
        world = Object::cast_to<WorldEnvironment>(get_node_or_null(world_environment_path));

        if (!world)
        {
            detach_targets();
            ERR_FAIL_MSG("world_environment_path must point to a WorldEnvironment.");
        }
    }

    if (!camera_path.is_empty())
    {
        camera = Object::cast_to<Camera3D>(get_node_or_null(camera_path));

        if (!camera)
        {
            detach_targets();
            ERR_FAIL_MSG("camera_path must point to a Camera3D.");
        }
    }

    if (!world)
    {
        world = instance<WorldEnvironment>(owned_world_id);

        if (!world)
        {
            world = memnew(WorldEnvironment);
            world->set_name("TrinityWorldEnvironment");
            add_child(world, false, INTERNAL_MODE_BACK);
            owned_world_id = world->get_instance_id();
        }
    }

    if (bound_world_id != world->get_instance_id() || bound_camera_id != (camera ? camera->get_instance_id() : 0))
    {
        detach_targets();

        bound_world_id = world->get_instance_id();
        original_environment = world->get_environment();
        original_world_attributes = world->get_camera_attributes();

        if (camera)
        {
            bound_camera_id = camera->get_instance_id();
            original_camera_attributes = camera->get_attributes();
        }

        baseline_environment = original_environment;

        if (baseline_environment.is_null())
            baseline_environment.instantiate();

        baseline_attributes = camera ? original_camera_attributes : original_world_attributes;

        if (baseline_attributes.is_null())
            baseline_attributes.instantiate();

        // Created once per binding so time-of-day steps don't allocate new RIDs.
        applied_environment.instantiate();
        applied_attributes.instantiate();
    }

    Ref<Environment> env = applied_environment;
    Ref<CameraAttributesPractical> attributes = applied_attributes;

    copy_properties(baseline_environment.ptr(), env.ptr());
    copy_properties(baseline_attributes.ptr(), attributes.ptr());

    env->set_glow_enabled(false);

    Dictionary global = find_type("Global");

    if (!global.is_empty())
    {
        int mode = int(number(global, "PfxTonemapMode", 0));

        // Retail composite variant 6 (EyeAdaptation=True) only samples the scene and
        // multiplies RGB by exposure_scale; it never samples the adaptation buffer.
        // Godot's auto-exposure would add brightness gain the source doesn't have.
        bool exposure_enabled = number(global, "PfxEyeAdaptationEnable", 0) > 0;
        double exposure = exposure_enabled ? MAX(0.0, number(global, "TMScale", 1.0)) : 1.0;

        // Retail ACES (AcesCurveType Filmic1) is a different curve from Godot's ACES fit,
        // which renders mid grey ~2.8x brighter and clips at 1. It's baked into the color
        // correction LUT instead, behind the Linear tonemapper with HDR headroom.
        bool retail_lut = mode == 3 && int(number(global, "PfxAcesCurveType", 0)) == 0;
        double hdr_scale = retail_lut ? 1.0 / RETAIL_TONEMAP_HEADROOM : 1.0;

        // Only None and ACES are currently verified; other source modes keep the baseline.
        if (mode == 0)
            env->set_tonemapper(Environment::TONE_MAPPER_LINEAR);
        else if (retail_lut)
        {
            env->set_tonemapper(Environment::TONE_MAPPER_LINEAR);
            env->set_adjustment_enabled(true);
            env->set_adjustment_brightness(1.0);
            env->set_adjustment_contrast(1.0);
            env->set_adjustment_saturation(1.0);
            env->set_adjustment_color_correction(retail_tonemap_lut(global));
        }
        else if (mode == 3)
            env->set_tonemapper(Environment::TONE_MAPPER_ACES);

        env->set_tonemap_exposure(exposure * hdr_scale);
        attributes->set_auto_exposure_enabled(false);

        if (use_native_bloom)
        {
            // Godot thresholds the glow after exposure, so the headroom scale applies to it too.
            env->set_glow_enabled(number(global, "PfxBloomEnable", 0) > 0);
            env->set_glow_blend_mode(Environment::GLOW_BLEND_MODE_ADDITIVE);
            env->set_glow_intensity(MAX(0.0, number(global, "PfxBloomIntensity", 0.3)));
            env->set_glow_hdr_bleed_threshold(MAX(0.0, number(global, "PfxBloomThreshold", 1.0)) * hdr_scale);
            env->set_glow_hdr_luminance_cap(MAX(0.0, number(global, "PfxBloomIntensityMax", 12.0)) * hdr_scale);
        }

        env->set_ssao_enabled(number(global, "DynamicAOEnable", 0) > 0);
        env->set_ssao_radius(CLAMP(number(global, "AOSerarchRadius", 1.0), 0.01, 16.0));
        env->set_ssao_intensity(CLAMP(number(global, "AODense", 1.0), 0.0, 16.0));
        env->set_ssao_power(CLAMP(number(global, "AOPow", 1.0), 0.0, 8.0));
        env->set_ssao_direct_light_affect(CLAMP(number(global, "AODirectLight", 0), 0.0, 1.0));

        env->set_ssr_enabled(number(global, "SSREnable", 0) > 0);

        // The source optical DOF model doesn't map to Practical's distance ramps.
        // Disabled is faithfully supported; enabled keeps the baseline.
        if (number(global, "DofEnable", 0) <= 0)
        {
            attributes->set_dof_blur_far_enabled(false);
            attributes->set_dof_blur_near_enabled(false);
        }
    }

    Dictionary sky_data = find_type("Sky");

    if (!sky_data.is_empty())
    {
        update_sky(sky_data);

        // SkyEnable selects Ikkaku sky shading; it is not a visibility flag.
        env->set_background(number(sky_data, "SkyVisible", 1) > 0 ? Environment::BG_SKY : Environment::BG_COLOR);

        // Source indirect light comes from ProbeMain/GILightCommon, not the visible sky.
        // Godot's default sources would bake the sky color into all diffuse and specular lighting.
        // apply_probe_ambient/apply_probe_specular re-enable them from the probe.
        env->set_ambient_source(Environment::AMBIENT_SOURCE_DISABLED);
        env->set_reflection_source(Environment::REFLECTION_SOURCE_DISABLED);
    }

    Dictionary fog = find_type("Fog");

    if (!fog.is_empty())
    {
        double intensity = number(fog, "FogIntensity", 0);
        double near = MAX(0.0, number(fog, "DistanceFogNear", 10));

        env->set_fog_enabled(intensity > 0);
        env->set_fog_mode(Environment::FOG_MODE_DEPTH);
        env->set_fog_depth_begin(near);
        env->set_fog_depth_end(MAX(near + 0.01, number(fog, "DistanceFogFar", 1000)));
        env->set_fog_density(CLAMP(intensity, 0.0, 1.0));
        env->set_fog_light_color(color(fog, "DistanceFogColorFar", Color(0.5, 0.6, 0.7)));
    }

    apply_probe_ambient(env);
    apply_probe_specular(env);
    update_world_shadow();

    auto *sun = instance<DirectionalLight3D>(owned_light_id);

    if (!sun)
    {
        sun = memnew(DirectionalLight3D);
        sun->set_name("TrinityDirectionalLight");
        add_child(sun, false, INTERNAL_MODE_BACK);
        owned_light_id = sun->get_instance_id();
    }

    Dictionary directional = find_type("Directional");
    // Direction (time-curved) comes from DirectionalMain; per-category color and
    // intensity (also time-curved) come from the Selectable light with that label.
    Dictionary selected = lighting_category >= 0 ? find_category(lighting_category) : Dictionary();
    Dictionary shading = selected.is_empty() ? directional : selected;

    sun->set_visible(!shading.is_empty());
    // Retail lights diffuse with Color * Intensity / pi; Godot's energy already gives albedo * energy.
    sun->set_param(Light3D::PARAM_ENERGY, MAX(0.0, number(shading, "Intensity", 0)) / Math_PI);
    sun->set_color(color(shading, "Color", Color(1, 1, 1), false));
    // Retail always renders the cascade shadows; only ReceiveShadow=0 materials skip them.
    // ShadowEnableSelfShadow only controls ignoring occluders of the receiver's own category
    // (within ShadowSelfShadowMaxDistance), which Godot's shadow maps can't express.
    sun->set_shadow(sun->get_param(Light3D::PARAM_ENERGY) > 0);

    // Cascades only cover ShadowFadeDistance; the baked world shadow covers the rest.
    // Retail fades them out with (d / distance)^ShadowFadePow, Godot linearly from
    // fade_start, so the fade starts where the retail term reaches 10%.
    double shadow_distance = MAX(1.0, number(directional, "ShadowFadeDistance", 50));
    double shadow_fade_pow = MAX(1.0, number(directional, "ShadowFadePow", 10));
    sun->set_param(Light3D::PARAM_SHADOW_MAX_DISTANCE, shadow_distance);
    sun->set_param(Light3D::PARAM_SHADOW_FADE_START, std::pow(0.1, 1.0 / shadow_fade_pow));

    // Splits follow ShadowCascadeExponent. ShadowCascadeLevel isn't mapped: Godot only
    // offers 2 or 4 cascades.
    double cascade_exponent = MAX(0.1, number(directional, "ShadowCascadeExponent", 1.6));
    sun->set_shadow_mode(DirectionalLight3D::SHADOW_PARALLEL_4_SPLITS);
    sun->set_param(Light3D::PARAM_SHADOW_SPLIT_1_OFFSET, std::pow(0.25, cascade_exponent));
    sun->set_param(Light3D::PARAM_SHADOW_SPLIT_2_OFFSET, std::pow(0.5, cascade_exponent));
    sun->set_param(Light3D::PARAM_SHADOW_SPLIT_3_OFFSET, std::pow(0.75, cascade_exponent));

    sun->set_rotation(Vector3());
    sun->set_position(Vector3());

    // Retail shades with DirectionalMain but casts shadows from ShadowTransform, which is
    // steeper (54 vs 40 degrees in master). The Godot light follows the shadow caster and
    // trinity_light.gdshaderinc swaps the shading direction back in for this light.
    Dictionary shadow = find_type("Shadow");
    Dictionary caster = shadow.has("rotation") ? shadow : directional;

    if (caster.has("rotation"))
    {
        sun->set_quaternion(caster["rotation"]);
        sun->set_position(caster["translation"]);
    }

    // Directions toward the light; a DirectionalLight3D shines along its -Z.
    bool split_direction = caster.has("rotation") && directional.has("rotation");
    Vector3 to_caster = Quaternion(caster.get("rotation", Quaternion())).xform(Vector3(0, 0, 1));
    Vector3 to_shading = Quaternion(directional.get("rotation", Quaternion())).xform(Vector3(0, 0, 1));
    RenderingServer *rs = RenderingServer::get_singleton();
    rs->global_shader_parameter_set("TrinitySunShadowDirection", Vector4(to_caster.x, to_caster.y, to_caster.z, split_direction ? 1.0 : 0.0));
    rs->global_shader_parameter_set("TrinitySunShadingDirection", to_shading);

    sun->set_param(Light3D::PARAM_SHADOW_NORMAL_BIAS, MAX(0.0, number(shading, "ShadowNormalBias", 1.0)));
    sun->set_param(Light3D::PARAM_SHADOW_BIAS, MAX(0.0, number(shading, "ShadowDepthBias", 0.1)));

    if (world->get_environment() != env)
        world->set_environment(env);

    if (world->get_camera_attributes() != attributes)
        world->set_camera_attributes(attributes);

    if (camera && camera->get_attributes() != attributes)
        camera->set_attributes(attributes);
}
