#include "ptcl.h"

#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <cmath>

using namespace godot;

void ParticleArchive::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("load_from_file", "path"), &ParticleArchive::LoadFromFile);
    ClassDB::bind_method(D_METHOD("load_from_buffer", "bytes"), &ParticleArchive::LoadFromBuffer);
    ClassDB::bind_method(D_METHOD("get_emitter_sets"), &ParticleArchive::get_emitter_sets);
    ClassDB::bind_method(D_METHOD("get_texture_names"), &ParticleArchive::get_texture_names);
    ClassDB::bind_method(D_METHOD("get_texture", "id"), &ParticleArchive::get_texture);
    ClassDB::bind_method(D_METHOD("get_warnings"), &ParticleArchive::get_warnings);
    ClassDB::bind_method(D_METHOD("get_version"), &ParticleArchive::get_version);

    const uint32_t usage = PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY;
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "emitter_sets", PROPERTY_HINT_NONE, "", usage), "", "get_emitter_sets");
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "texture_names", PROPERTY_HINT_NONE, "", usage),
                 "", "get_texture_names");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "warnings", PROPERTY_HINT_NONE, "", usage),
                 "", "get_warnings");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "version", PROPERTY_HINT_NONE, "", usage), "", "get_version");
}

namespace
{
using namespace PtclFormat;

constexpr int MAX_SECTION_COUNT = 16384;
constexpr int MAX_EMITTER_DEPTH = 32;

bool finite_values(const Variant& value)
{
    switch (value.get_type())
    {
        case Variant::FLOAT:
            return std::isfinite(double(value));
        case Variant::VECTOR2:
            return Vector2(value).is_finite();
        case Variant::VECTOR3:
            return Vector3(value).is_finite();
        case Variant::VECTOR4:
            return Vector4(value).is_finite();
        case Variant::COLOR:
        {
            const Color color = value;
            return std::isfinite(color.r) && std::isfinite(color.g) &&
                   std::isfinite(color.b) && std::isfinite(color.a);
        }
        case Variant::ARRAY:
        {
            for (const Variant& item : Array(value))
            {
                if (!finite_values(item))
                {
                    return false;
                }
            }
            return true;
        }
        case Variant::DICTIONARY:
        {
            const Dictionary fields = value;
            for (const Variant& key : fields.keys())
            {
                if (!finite_values(fields[key]))
                {
                    return false;
                }
            }
            return true;
        }
        default:
            return true;
    }
}

class PtclParser
{
    struct Section
    {
        uint64_t position = 0;
        SectionHeader header;
    };

    Ref<StreamPeerBuffer> sp;
    uint64_t file_size;
    HashSet<uint64_t> visited;

    bool check_range(uint64_t position, uint64_t size)
    {
        if (position > file_size || size > file_size - position)
        {
            valid = false;
        }
        return valid;
    }

    Section read_section(uint64_t position, const String& expected_magic = String())
    {
        Section section;
        section.position = position;

        if (!check_range(position, SECTION_HEADER_SIZE) || visited.has(position) ||
            visited.size() >= MAX_SECTION_COUNT)
        {
            valid = false;
            return section;
        }

        visited.insert(position);
        sp->seek(position);
        section.header.Read(sp);
        if (!expected_magic.is_empty() && section.header.magic != expected_magic)
        {
            valid = false;
        }
        return section;
    }

    uint64_t resolve_offset(const Section& section, uint32_t relative_offset)
    {
        const uint64_t position = section.position + relative_offset;
        if (relative_offset == NO_SECTION || relative_offset < SECTION_HEADER_SIZE ||
            !check_range(position, 1))
        {
            valid = false;
            return 0;
        }
        return position;
    }

    Array animation_keys(const AnimationKeyTable& table, uint32_t count)
    {
        Array keys;
        if (count > table.keys.size())
        {
            valid = false;
            return keys;
        }

        for (uint32_t i = 0; i < count; ++i)
        {
            const AnimationKey& key = table.keys[i];
            keys.push_back(Vector4(key.x, key.y, key.z, key.time));
        }
        return keys;
    }

    Dictionary sampler_fields(const TextureSampler& sampler) const
    {
        Dictionary fields;
        fields["id"] = String::num_uint64(sampler.texture_id);
        fields["wrap_u"] = sampler.wrap_u;
        fields["wrap_v"] = sampler.wrap_v;
        fields["filter"] = sampler.filter;
        return fields;
    }

    Dictionary emitter_fields(const EmitterRecord& record)
    {
        const EmitterStatic& animation = record.emitter_static;
        const EmitterInfo& info = record.emitter_info;
        const Emission& emission = record.emission;
        const EmitterShape& shape = record.shape_info;
        const RenderState& render = record.render_state;
        const ParticleData& particle = record.particle_data;
        const ParticleVelocity& velocity = record.particle_velocity;
        const ParticleColor& color = record.particle_color;
        const ParticleScale& scale = record.particle_scale;
        const TextureScrollAnimation& uv = animation.tex_scroll_anim0;

        Dictionary fields;
        fields["name"] = record.name;
        fields["draw"] = info.is_particle_draw != 0;
        fields["follow_type"] = info.follow_type;
        fields["position"] = Vector3(info.trans_x, info.trans_y, info.trans_z);
        fields["rotation"] = Vector3(info.rotate_x, info.rotate_y, info.rotate_z);
        fields["scale"] = Vector3(info.scale_x, info.scale_y, info.scale_z);
        fields["emitter_color"] = Color(info.color0_r, info.color0_g, info.color0_b, info.color0_a);

        fields["one_time"] = emission.is_one_time;
        fields["world_gravity"] = emission.is_world_gravity;
        fields["start_frame"] = int64_t(emission.start);
        fields["duration_frames"] = int64_t(emission.duration);
        fields["rate"] = emission.rate;
        fields["rate_random"] = emission.rate_random;
        fields["interval"] = emission.interval;
        fields["gravity"] = Vector3(emission.gravity_dir_x, emission.gravity_dir_y,
                                    emission.gravity_dir_z) * emission.gravity_scale;

        fields["shape"] = shape.volume_type;
        fields["radius"] = Vector3(shape.volume_radius_x, shape.volume_radius_y, shape.volume_radius_z);
        fields["shape_scale"] = Vector3(shape.volume_form_scale_x, shape.volume_form_scale_y,
                                        shape.volume_form_scale_z);
        fields["line_length"] = shape.line_length;

        fields["blend"] = render.blend_type;
        fields["depth_test"] = render.is_depth_test;
        fields["depth_write"] = render.is_depth_mask;
        fields["alpha_test"] = render.is_alpha_test;
        fields["alpha_threshold"] = render.alpha_threshold;
        fields["alpha_source"] = record.combiner.tex_alpha0_input_type;
        fields["fragment_mode"] = record.combiner.shader_type;

        fields["billboard"] = particle.billboard_type;
        fields["infinite_life"] = particle.infinite_life;
        fields["life_frames"] = particle.life;
        fields["life_random"] = particle.life_random;
        fields["primitive_id"] = String::num_uint64(particle.primitive_id);

        fields["radial_velocity"] = velocity.all_direction;
        fields["direction_velocity"] = velocity.designated_dir_scale;
        fields["direction"] = Vector3(velocity.designated_dir_x, velocity.designated_dir_y, velocity.designated_dir_z);
        fields["spread_radians"] = velocity.diffusion_dir_angle;
        fields["velocity_random"] = velocity.vel_random;

        fields["color_type"] = color.color0_type;
        fields["alpha_type"] = color.alpha0_type;
        fields["color"] = Color(color.color0_r, color.color0_g, color.color0_b, color.alpha0);
        fields["color_scale"] = animation.color_scale;
        fields["particle_scale"] = Vector3(scale.scale_x, scale.scale_y, scale.scale_z);
        fields["scale_random"] = Vector3(scale.scale_random_x, scale.scale_random_y, scale.scale_random_z);
        fields["initial_rotation"] = Vector3(animation.rotate_init_x, animation.rotate_init_y, animation.rotate_init_z);
        fields["rotation_random"] = Vector3(animation.rotate_init_rand_x, animation.rotate_init_rand_y,
                                            animation.rotate_init_rand_z);
        fields["angular_velocity"] = Vector3(animation.rotate_add_x, animation.rotate_add_y, animation.rotate_add_z);

        fields["color_keys"] = animation_keys(animation.color0, animation.num_color0_keys);
        fields["alpha_keys"] = animation_keys(animation.alpha0, animation.num_alpha0_keys);
        fields["scale_keys"] = animation_keys(animation.scale_anim, animation.num_scale_keys);
        fields["samplers"] = Array({sampler_fields(record.sampler0), sampler_fields(record.sampler1),
                                    sampler_fields(record.sampler2)});
        fields["uv_scroll"] = Vector2(uv.scroll_add_x, uv.scroll_add_y);
        fields["uv_offset"] = Vector2(uv.scroll_x, uv.scroll_y);
        fields["uv_scale"] = Vector2(uv.scale_x, uv.scale_y);
        fields["uv_divisions"] = Vector2(uv.uv_div_x, uv.uv_div_y);
        fields["texture_animation"] = record.texture_anim0.pattern_anim_type;
        return fields;
    }

    Dictionary read_emitter(const Section& section, int depth)
    {
        Dictionary emitter;
        const SectionHeader& header = section.header;
        const uint32_t data_end = header.attributes_offset == NO_SECTION ? header.size : header.attributes_offset;
        const uint64_t data_position = resolve_offset(section, header.data_offset);

        if (depth > MAX_EMITTER_DEPTH || data_end < header.data_offset ||
            data_end - header.data_offset < EMITTER_RECORD_SIZE || !check_range(data_position, EMITTER_RECORD_SIZE))
        {
            valid = false;
            return emitter;
        }

        sp->seek(data_position);
        EmitterRecord record;
        record.Read(sp);
        if (sp->get_position() != data_position + EMITTER_RECORD_SIZE)
        {
            valid = false;
            return emitter;
        }
        emitter = emitter_fields(record);

        Array children;
        uint64_t child_position = header.child_count ? resolve_offset(section, header.children_offset) : 0;
        for (int i = 0; valid && i < header.child_count; ++i)
        {
            const Section child = read_section(child_position, "EMTR");
            if (!valid)
            {
                break;
            }
            children.push_back(read_emitter(child, depth + 1));
            if (i + 1 < header.child_count)
            {
                child_position = resolve_offset(child, child.header.next_offset);
            }
        }
        emitter["children"] = children;

        PackedStringArray extensions;
        if (header.attributes_offset != NO_SECTION)
        {
            uint64_t attribute_position = resolve_offset(section, header.attributes_offset);
            while (valid)
            {
                const Section attribute = read_section(attribute_position);
                if (!valid)
                {
                    break;
                }
                extensions.push_back(attribute.header.magic);
                if (attribute.header.next_offset == NO_SECTION)
                {
                    break;
                }
                attribute_position = resolve_offset(attribute, attribute.header.next_offset);
            }
        }
        emitter["extensions"] = extensions;
        valid = valid && finite_values(emitter);
        return emitter;
    }

    void read_emitter_sets(const Section& section)
    {
        const SectionHeader& header = section.header;
        uint64_t set_position = header.child_count ? resolve_offset(section, header.children_offset) : 0;
        for (int i = 0; valid && i < header.child_count; ++i)
        {
            const Section set = read_section(set_position, "ESET");
            const uint64_t data_position = resolve_offset(set, set.header.data_offset);
            if (!valid || !check_range(data_position, EmitterSetInfo::SIZE))
            {
                return;
            }

            sp->seek(data_position);
            EmitterSetInfo info;
            info.Read(sp);

            Dictionary emitter_set;
            emitter_set["name"] = info.name;
            Array emitters;
            uint64_t emitter_position = set.header.child_count ? resolve_offset(set, set.header.children_offset) : 0;
            for (int j = 0; valid && j < set.header.child_count; ++j)
            {
                const Section emitter = read_section(emitter_position, "EMTR");
                if (!valid)
                {
                    break;
                }
                emitters.push_back(read_emitter(emitter, 0));
                if (j + 1 < set.header.child_count)
                {
                    emitter_position = resolve_offset(emitter, emitter.header.next_offset);
                }
            }
            emitter_set["emitters"] = emitters;
            sets.push_back(emitter_set);
            if (i + 1 < header.child_count)
            {
                set_position = resolve_offset(set, set.header.next_offset);
            }
        }
    }

    void read_textures(const Section& section)
    {
        const SectionHeader& header = section.header;
        if (header.size)
        {
            const uint64_t data_position = resolve_offset(section, header.data_offset);
            if (!check_range(data_position, header.size) || header.size < 32)
            {
                valid = false;
                return;
            }

            sp->seek(data_position);
            BNTXHeader texture_header;
            BNTXHeader::Read(sp, texture_header);
            if (!texture_header.Magic.begins_with("BNTX") || texture_header.FileSize > header.size)
            {
                valid = false;
                return;
            }

            sp->seek(data_position);
            const Array result = sp->get_data(header.size);
            bntx = result[1];
        }
        if (header.child_count == 0)
        {
            return;
        }

        const Section descriptors = read_section(resolve_offset(section, header.children_offset), "GTNT");
        uint64_t entry_position = resolve_offset(descriptors, descriptors.header.data_offset);
        const uint64_t end = entry_position + descriptors.header.size;
        if (!valid || !check_range(entry_position, descriptors.header.size))
        {
            return;
        }

        while (valid && entry_position < end)
        {
            if (end - entry_position < TextureDescriptor::HEADER_SIZE)
            {
                valid = false;
                return;
            }

            sp->seek(entry_position);
            TextureDescriptor descriptor;
            descriptor.Read(sp);
            if (descriptor.name_length > end - sp->get_position())
            {
                valid = false;
                return;
            }
            textures[String::num_uint64(descriptor.id)] = ReadFixedString(sp, descriptor.name_length);
            if (descriptor.next_offset == 0)
            {
                break;
            }
            if (descriptor.next_offset < TextureDescriptor::HEADER_SIZE + descriptor.name_length ||
                descriptor.next_offset > end - entry_position)
            {
                valid = false;
                break;
            }
            entry_position += descriptor.next_offset;
        }
    }

public:
    bool valid = true;
    Array sets;
    Dictionary textures;
    PackedByteArray bntx;
    PackedStringArray warnings;

    PtclParser(const Ref<StreamPeerBuffer>& stream, uint64_t size)
        : sp(stream), file_size(size)
    {
    }

    void parse(uint64_t section_position)
    {
        while (valid)
        {
            const Section section = read_section(section_position);
            if (!valid)
            {
                return;
            }

            const String& magic = section.header.magic;
            if (magic == "ESTA")
            {
                read_emitter_sets(section);
            }
            else if (magic == "GRTF")
            {
                read_textures(section);
            }
            else if (magic != "PRMA" && magic != "G3PR" && magic != "GRSN")
            {
                warnings.push_back("Unconverted section: " + magic);
            }

            if (section.header.next_offset == NO_SECTION)
            {
                break;
            }
            section_position = resolve_offset(section, section.header.next_offset);
        }
    }
};
}

Error ParticleArchive::LoadFromFile(const String& path)
{
    PackedByteArray bytes = FileAccess::get_file_as_bytes(path);
    ERR_FAIL_COND_V_MSG(bytes.is_empty(), ERR_FILE_CANT_OPEN, "Couldn't read PTCL: " + path);
    return LoadFromBuffer(bytes);
}

Error ParticleArchive::LoadFromBuffer(const PackedByteArray& bytes)
{
    ERR_FAIL_COND_V_MSG(bytes.size() > INT32_MAX, ERR_OUT_OF_MEMORY, "PTCL exceeds the stream size limit");

    ERR_FAIL_COND_V_MSG(bytes.size() < PtclFormat::FileHeader::SIZE, ERR_FILE_CORRUPT, "Truncated VFXB header");

    Ref<StreamPeerBuffer> sp;
    sp.instantiate();
    sp->set_data_array(bytes);
    sp->set_big_endian(false);

    PtclFormat::FileHeader header;
    header.Read(sp);
    ERR_FAIL_COND_V_MSG(header.magic != "VFXB", ERR_FILE_CORRUPT, "Invalid VFXB header");
    ERR_FAIL_COND_V_MSG(header.byte_order != 0xfeff, ERR_UNAVAILABLE, "Only little-endian VFXB is supported");
    ERR_FAIL_COND_V_MSG(header.vfx_version != 22, ERR_UNAVAILABLE, "Unsupported VFXB version (expected 22)");
    ERR_FAIL_COND_V_MSG(header.file_size != bytes.size(), ERR_FILE_CORRUPT, "VFXB file size mismatch");

    PtclParser reader(sp, bytes.size());
    reader.parse(header.first_section_offset);
    ERR_FAIL_COND_V_MSG(!reader.valid, ERR_FILE_CORRUPT, "Invalid PTCL section or emitter data");

    Ref<BinaryTextureArchive> archive;
    if (!reader.bntx.is_empty())
    {
        archive.instantiate();
        if (archive->LoadImagesFromBuffer(reader.bntx) != OK)
        {
            archive.unref();
            reader.warnings.push_back("Embedded BNTX textures could not be decoded");
        }
    }

    emitter_sets = reader.sets;
    texture_names = reader.textures;
    texture_archive = archive;
    texture_cache.clear();
    warnings = reader.warnings;
    version = header.vfx_version;
    return OK;
}

Ref<Texture2D> ParticleArchive::get_texture(const String& id) const
{
    if (texture_cache.has(id))
    {
        return texture_cache[id];
    }
    if (texture_archive.is_null() || !texture_names.has(id))
    {
        return Ref<Texture2D>();
    }
    Ref<BinaryTexture> texture = texture_archive->GetTexture(texture_names[id]);
    if (texture.is_null())
    {
        return Ref<Texture2D>();
    }
    Ref<Image> image = texture->get_layer_image(0);
    if (image.is_null())
    {
        return Ref<Texture2D>();
    }

    const Vector4i sources = texture->get_channel_sources();
    if (sources != Vector4i(2, 3, 4, 5))
    {
        // Godot materials sample RGBA directly; BNTX can route any source
        // channel (or constant zero/one) to each component.
        image = image->duplicate();
        if (image->is_compressed() && image->decompress() != OK)
        {
            return Ref<Texture2D>();
        }
        image->convert(Image::FORMAT_RGBAF);

        for (int y = 0; y < image->get_height(); ++y)
        {
            for (int x = 0; x < image->get_width(); ++x)
            {
                const Color pixel = image->get_pixel(x, y);
                const float channels[] = {0.0f, 1.0f, pixel.r, pixel.g, pixel.b, pixel.a};
                Color mapped;
                for (int component = 0; component < 4; ++component)
                {
                    const int source = sources[component];
                    mapped[component] = source >= 0 && source < 6 ? channels[source] : pixel[component];
                }
                image->set_pixel(x, y, mapped);
            }
        }
    }

    Ref<ImageTexture> result = ImageTexture::create_from_image(image);
    texture_cache[id] = result;
    return result;
}

PackedStringArray ResourceFormatLoaderPTCL::_get_recognized_extensions() const
{
    return PackedStringArray({"ptcl"});
}

bool ResourceFormatLoaderPTCL::_handles_type(const StringName& type) const
{
    return type == StringName("ParticleArchive");
}

String ResourceFormatLoaderPTCL::_get_resource_type(const String& path) const
{
    return path.get_extension().to_lower() == "ptcl" ? String("ParticleArchive") : String();
}

Variant ResourceFormatLoaderPTCL::_load(const String& path, const String& original_path, bool use_sub_threads,
                                        int32_t cache_mode) const
{
    Ref<ParticleArchive> archive;
    archive.instantiate();
    Error error = archive->LoadFromFile(path);
    return error == OK ? Variant(archive) : Variant(error);
}
