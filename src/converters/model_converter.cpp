#include "model_converter.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/cubemap.hpp>
#include <godot_cpp/classes/shader.hpp>
#include "shader_options.gen.h"
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/skin.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void TrinityModel::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("load_model", "path", "file", "variation"), &TrinityModel::load_model,
                         DEFVAL(String()));
    ClassDB::bind_method(D_METHOD("get_variations"), &TrinityModel::get_variations);
    ClassDB::bind_method(D_METHOD("get_variation"), &TrinityModel::get_variation);
    ClassDB::bind_method(D_METHOD("set_variation", "name"), &TrinityModel::set_variation);
    ClassDB::bind_method(D_METHOD("get_controls"), &TrinityModel::get_controls);
    ClassDB::bind_method(D_METHOD("get_control", "name"), &TrinityModel::get_control);
    ClassDB::bind_method(D_METHOD("set_control", "name", "index"), &TrinityModel::set_control);
    ClassDB::bind_static_method(
        "TrinityModel", D_METHOD("parse_mesh_buffer", "accessor_table", "verts", "inds", "poly_type", "start", "count"),
        &TrinityModel::parse_mesh_buffer);
}

void TrinityModel::print_bone_tree_compact(Skeleton3D* skeleton)
{
    if (!skeleton)
    {
        UtilityFunctions::push_error("print_bone_tree_compact: skeleton is null");
        return;
    }

    UtilityFunctions::print_rich("[b]" + skeleton->get_name() + " Bone Hierarchy (" +
                                 String::num_int64(skeleton->get_bone_count()) + " bones):[/b]");

    std::vector<int> printed;
    for (int i = 0; i < skeleton->get_bone_count(); i++)
    {
        if (skeleton->get_bone_parent(i) == -1)
        {
            _print_compact_recursive(skeleton, i, "", true, printed);
        }
    }
}

void TrinityModel::_print_compact_recursive(Skeleton3D* skeleton, int idx, String prefix, bool is_last,
                                            std::vector<int>& printed)
{
    if (std::find(printed.begin(), printed.end(), idx) != printed.end())
    {
        return;
    }
    printed.push_back(idx);

    String connector = is_last ? "`-- " : "|-- ";

    String line = prefix + connector + skeleton->get_bone_name(idx) + " [" + String::num_int64(idx) + "]";
    UtilityFunctions::print(line);

    std::vector<int> children;
    for (int i = 0; i < skeleton->get_bone_count(); i++)
    {
        if (skeleton->get_bone_parent(i) == idx)
        {
            children.push_back(i);
        }
    }

    for (size_t i = 0; i < children.size(); i++)
    {
        String ext = is_last ? "    " : "|   ";
        _print_compact_recursive(skeleton, children[i], prefix + ext, i == children.size() - 1, printed);
    }
}

Dictionary TrinityModel::parse_mesh_buffer(const Ref<VertexAccessors>& accessor_table, const PackedByteArray& verts,
                                           const PackedByteArray& inds, int poly_type, int start, int count)
{
    Dictionary result = parse_vertex_buffer(accessor_table, verts);
    result["Indicies"] = parse_index_buffer(inds, poly_type, start, count);
    return result;
}

Dictionary TrinityModel::parse_vertex_buffer(const Ref<VertexAccessors>& accessor_table, const PackedByteArray& verts)
{
    PackedVector3Array pos, norm;
    PackedVector2Array uv, uv2;
    PackedColorArray colors;
    PackedInt32Array blend_inds;
    PackedFloat32Array blend_weights, tangents;

    Array strides = accessor_table->get_Strides();
    Ref<SizeTable> stride_obj = strides[0];
    int stride = stride_obj->get_Size();
    ERR_FAIL_COND_V(stride <= 0, Dictionary());

    Ref<StreamPeerBuffer> stream_vert;
    stream_vert.instantiate();
    stream_vert->set_data_array(verts);

    Array accessors = accessor_table->get("Accessors");

    int pos_attr = -1, norm_attr = -1, uv_attr = -1, uv2_attr = -1;
    int blend_inds_attr = -1, blend_weights_attr = -1, tangent_attr = -1;
    int color_attr = -1;
    String color_type;
    String blend_weights_type;

    for (int a = 0; a < accessors.size(); a++)
    {
        Ref<Resource> attrib = accessors[a];
        int attr_pos = attrib->get("Position");
        String attr_name = attrib->get("Attribute");

        if (attr_name == "POSITION" && pos_attr == -1)
        {
            pos_attr = attr_pos;
        }
        else if (attr_name == "NORMAL" && norm_attr == -1)
        {
            norm_attr = attr_pos;
        }
        else if (attr_name == "TEXCOORD")
        {
            int layer = attrib->get("AttributeLayer");
            if (layer == 0 && uv_attr == -1)
            {
                uv_attr = attr_pos;
            }
            else if (layer == 1 && uv2_attr == -1)
            {
                uv2_attr = attr_pos;
            }
        }
        else if (attr_name == "COLOR" && color_attr == -1 && int(attrib->get("AttributeLayer")) == 0)
        {
            color_type = attrib->get("Type");
            if (color_type == "RGBA_8_UNORM" || color_type == "RGBA_16_FLOAT" || color_type == "RGBA_32_FLOAT")
            {
                color_attr = attr_pos;
            }
        }
        else if (attr_name == "BLEND_INDICES" && blend_inds_attr == -1)
        {
            blend_inds_attr = attr_pos;
        }
        else if (attr_name == "TANGENT" && tangent_attr == -1)
        {
            tangent_attr = attr_pos;
        }
        else if (attr_name == "BLEND_WEIGHTS" && blend_weights_attr == -1)
        {
            blend_weights_attr = attr_pos;
            blend_weights_type = attrib->get("Type");
        }
    }

    int curr_pos = 0;
    while (curr_pos + stride <= stream_vert->get_size())
    {
        if (pos_attr != -1)
        {
            stream_vert->seek(curr_pos + pos_attr);
            float x = stream_vert->get_float();
            float y = stream_vert->get_float();
            float z = stream_vert->get_float();
            pos.push_back(Vector3(x, y, z));
        }
        if (norm_attr != -1)
        {
            stream_vert->seek(curr_pos + norm_attr);
            float x = Utils::half_to_float(stream_vert->get_u16());
            float y = Utils::half_to_float(stream_vert->get_u16());
            float z = Utils::half_to_float(stream_vert->get_u16());
            stream_vert->get_u16();
            norm.push_back(Vector3(x, y, z).normalized());
        }
        if (uv_attr != -1)
        {
            stream_vert->seek(curr_pos + uv_attr);
            float u = stream_vert->get_float();
            float v = stream_vert->get_float();
            uv.push_back(Vector2(u, v));
        }
        if (uv2_attr != -1)
        {
            stream_vert->seek(curr_pos + uv2_attr);
            float u = stream_vert->get_float();
            float v = stream_vert->get_float();
            uv2.push_back(Vector2(u, v));
        }
        if (color_attr != -1)
        {
            stream_vert->seek(curr_pos + color_attr);
            Color color;
            for (int channel = 0; channel < 4; channel++)
            {
                if (color_type == "RGBA_8_UNORM")
                {
                    color[channel] = stream_vert->get_u8() / 255.0f;
                }
                else if (color_type == "RGBA_16_FLOAT")
                {
                    color[channel] = Utils::half_to_float(stream_vert->get_u16());
                }
                else
                {
                    color[channel] = stream_vert->get_float();
                }
            }
            colors.push_back(color);
        }
        if (blend_inds_attr != -1)
        {
            stream_vert->seek(curr_pos + blend_inds_attr);
            blend_inds.push_back(stream_vert->get_u8());
            blend_inds.push_back(stream_vert->get_u8());
            blend_inds.push_back(stream_vert->get_u8());
            blend_inds.push_back(stream_vert->get_u8());
        }
        if (tangent_attr != -1)
        {
            stream_vert->seek(curr_pos + tangent_attr);
            tangents.push_back(Utils::half_to_float(stream_vert->get_u16()));
            tangents.push_back(Utils::half_to_float(stream_vert->get_u16()));
            tangents.push_back(Utils::half_to_float(stream_vert->get_u16()));
            tangents.push_back(Utils::half_to_float(stream_vert->get_u16()));
        }
        if (blend_weights_attr != -1)
        {
            stream_vert->seek(curr_pos + blend_weights_attr);
            // Weights are normally RGBA_16_UNORM; read whatever the accessor declares.
            auto read_weight = [&]() -> float
            {
                if (blend_weights_type == "RGBA_16_UNORM")
                {
                    return stream_vert->get_u16() / 65535.0f;
                }
                if (blend_weights_type == "RGBA_8_UNORM")
                {
                    return stream_vert->get_u8() / 255.0f;
                }
                if (blend_weights_type == "RGBA_16_FLOAT")
                {
                    return Utils::half_to_float(stream_vert->get_u16());
                }
                return stream_vert->get_float();
            };
            float w1 = read_weight();
            float w2 = read_weight();
            float w3 = read_weight();
            float w4 = read_weight();
            float total = w1 + w2 + w3 + w4;
            if (total > 0.0f)
            {
                blend_weights.push_back(w1 / total);
                blend_weights.push_back(w2 / total);
                blend_weights.push_back(w3 / total);
                blend_weights.push_back(w4 / total);
            }
            else
            {
                blend_weights.push_back(1.0f);
                blend_weights.push_back(0.0f);
                blend_weights.push_back(0.0f);
                blend_weights.push_back(0.0f);
            }
        }
        curr_pos += stride;
    }

    Dictionary result;
    result["Pos"] = pos;
    result["Norm"] = norm;
    result["UV"] = uv;
    result["UV2"] = uv2;
    result["Color"] = colors;
    result["BlendInds"] = blend_inds;
    result["BlendWeights"] = blend_weights;
    result["Tangents"] = tangents;

    return result;
}

PackedInt32Array TrinityModel::parse_index_buffer(const PackedByteArray& inds, int poly_type, int start, int count)
{
    ERR_FAIL_COND_V(poly_type < 0 || poly_type > 3 || start < 0 || count < 0, PackedInt32Array());
    const int ind_size = 1 << poly_type;
    ERR_FAIL_COND_V((int64_t(start) + count) * ind_size > inds.size(), PackedInt32Array());
    PackedInt32Array indices;
    Ref<StreamPeerBuffer> stream_ind;
    stream_ind.instantiate();
    stream_ind->set_data_array(inds);
    int64_t curr_pos = int64_t(start) * ind_size;
    stream_ind->seek(curr_pos);
    const int64_t end = (int64_t(start) + count) * ind_size;
    while (curr_pos < end)
    {
        switch (poly_type)
        {
            case 0:
                indices.push_back(stream_ind->get_u8());
                break;
            case 1:
                indices.push_back(stream_ind->get_u16());
                break;
            case 2:
                indices.push_back(stream_ind->get_u32());
                break;
            case 3:
                indices.push_back((int32_t)stream_ind->get_u64());
                break;
        }
        curr_pos += ind_size;
    }
    return indices;
}

Dictionary TrinityModel::_load_materials(const String& path, const Array& material_files)
{
    Dictionary materials;
    ResourceLoader* rl = ResourceLoader::get_singleton();

    for (int i = 0; i < material_files.size(); i++)
    {
        Ref<TRMaterial> material = rl->load(path.path_join((String)material_files[i]));
        if (material.is_null())
        {
            UtilityFunctions::push_warning("Failed to load material file: ", path.path_join((String)material_files[i]));
            continue;
        }
        Array mats = material->get_Materials();
        for (int m = 0; m < mats.size(); m++)
        {
            Ref<MaterialEntry> mat = mats[m];
            Ref<ShaderMaterial> shdr = _build_shader_material(mat);
            _apply_textures(path, mat, shdr);
            _apply_params(mat, shdr);
            _apply_samplers(mat, shdr);
            materials[mat->get("Name")] = shdr;
        }
    }
    return materials;
}

Ref<ShaderMaterial> TrinityModel::_build_shader_material(const Ref<MaterialEntry>& mat)
{
    Ref<ShaderMaterial> sm;
    sm.instantiate();

    Array shaders = mat->get_Shaders();
    Ref<ShaderEntry> shader = shaders[0];
    String shader_name = shader->get_Name();
    // These legacy material names select front/back transparent passes of
    // the retail IkCharacter family, rather than separate retail archives.
    const String option_shader_name =
        (shader_name == "IkCharacterTransparent" || shader_name == "IkCharacterTransparentInner")
            ? String("IkCharacter")
            : (shader_name == "GrassTransparent" ? String("Transparent") : shader_name);

    // Godot fixes blending per shader, so blended materials of opaque shader
    // families use the transparent pass built from the same equations.
    const String alpha_type = mat->get_AlphaType();
    const bool blended = alpha_type == "Blend" || alpha_type == "Add" || alpha_type == "BlendPreMultiAlpha";
    String shader_file = shader_name;
    if (blended && shader_name == "Standard")
    {
        shader_file = "Transparent";
    }
    else if (blended && shader_name == "IkCharacter")
    {
        shader_file = "IkCharacterTransparent";
    }

    sm->set_name(mat->get_Name());
    sm->set_shader(ResourceLoader::get_singleton()->load("res://gflib/shaders/" + shader_file + ".gdshader"));

    Ref<Shader> godot_shader = sm->get_shader();
    if (godot_shader.is_null())
    {
        return sm;
    }
    Dictionary uniform_types;
    for (const Variant& entry : godot_shader->get_shader_uniform_list())
    {
        Dictionary uniform = entry;
        uniform_types[uniform["name"]] = uniform["type"];
    }
    auto set_option = [&](const String& name, int value)
    {
        if (!uniform_types.has(name))
        {
            return;
        }
        if (int(uniform_types[name]) == Variant::BOOL)
        {
            sm->set_shader_parameter(name, value != 0);
        }
        else
        {
            sm->set_shader_parameter(name, value);
        }
    };

    for (const auto& choice : TrinityShaderOptions::choices)
    {
        if (option_shader_name == choice.shader && choice.is_default)
        {
            set_option(choice.option, choice.value);
        }
    }
    Array string_params = shader->get_StringParams();
    for (int i = 0; i < string_params.size(); i++)
    {
        Ref<ShaderStringParam> v = string_params[i];
        const String name = v->get_Name();
        const String val = v->get_Value();
        bool resolved = false;
        for (const auto& choice : TrinityShaderOptions::choices)
        {
            if (option_shader_name == choice.shader && name == choice.option && val == choice.name)
            {
                set_option(name, choice.value);
                resolved = true;
                break;
            }
        }
        if (resolved)
        {
            continue;
        }
        if (val.is_valid_int())
        {
            set_option(name, val.to_int());
        }
        else if (val.to_lower() == "true")
        {
            set_option(name, 1);
        }
        else if (val.to_lower() == "false")
        {
            set_option(name, 0);
        }
        else if (uniform_types.has(name))
        {
            UtilityFunctions::push_warning("Unknown shader option: ", shader_name, ".", name, " = ", val);
        }
    }
    // NVN takes the blend state from the material; transparent shaders rebuild
    // it from this value (see trinity_blend.gdshaderinc).
    if (uniform_types.has("TrinityAlphaType"))
    {
        int alpha_mode = 1; // Blend
        if (alpha_type == "Opaque")
        {
            alpha_mode = 0;
        }
        else if (alpha_type == "Add")
        {
            alpha_mode = 2;
        }
        else if (alpha_type == "BlendPreMultiAlpha")
        {
            alpha_mode = 3;
        }
        sm->set_shader_parameter("TrinityAlphaType", alpha_mode);
    }
    return sm;
}

void TrinityModel::_apply_textures(const String& path, const Ref<MaterialEntry>& mat, Ref<ShaderMaterial> shdr)
{
    // Retail probe maps are cube arrays; shaders declare them as samplerCube.
    PackedStringArray cubemaps;
    Ref<Shader> shader = shdr->get_shader();
    if (shader.is_valid())
    {
        for (const Variant& entry : shader->get_shader_uniform_list())
        {
            Dictionary uniform = entry;
            if (int(uniform["type"]) == Variant::OBJECT && String(uniform["hint_string"]) == "Cubemap")
            {
                cubemaps.push_back(uniform["name"]);
            }
        }
    }

    Array textures = mat->get_Textures();
    for (int i = 0; i < textures.size(); i++)
    {
        Ref<TextureEntry> t = textures[i];
        String file = t->get_File();
        String texName = file.get_file().get_basename();
        String tex_name = t->get_Name();
        const bool is_cubemap = cubemaps.has(tex_name);

        const String source_path = path.path_join(file).simplify_path();
        const String texture_id = source_path + String("\n") + texName + String(is_cubemap ? "\ncube" : "\n0");
        const String texture_path = String("res://.gflib-textures/") + texture_id.sha256_text() + String(".texture");
        ResourceLoader* loader = ResourceLoader::get_singleton();
        Ref<Texture> texture = loader->get_cached_ref(texture_path);
        if (texture.is_null())
        {
            Ref<BinaryTextureArchive> bntx = loader->load(source_path, "", ResourceLoader::CACHE_MODE_REUSE);
            if (bntx.is_null())
            {
                continue;
            }
            TypedDictionary<String, BinaryTexture> imgs = bntx->get_textures();
            if (!imgs.has(texName))
            {
                continue;
            }
            Ref<BinaryTexture> texture_array = imgs[texName];
            if (texture_array.is_null() || texture_array->get_layers() == 0)
            {
                continue;
            }
            if (is_cubemap)
            {
                if (texture_array->get_layers() < 6)
                {
                    continue;
                }
                TypedArray<Image> faces;
                for (int face = 0; face < 6; face++)
                {
                    Ref<Image> image = texture_array->get_layer_image(face);
                    if (image.is_null())
                    {
                        break;
                    }
                    image = image->duplicate();
                    // Shaders sample the probe at blurred mip levels, but the
                    // archive loader only keeps mip 0.
                    if (image->is_compressed())
                    {
                        image->decompress();
                    }
                    image->generate_mipmaps();
                    faces.push_back(image);
                }
                if (faces.size() != 6)
                {
                    continue;
                }
                Ref<Cubemap> cubemap;
                cubemap.instantiate();
                if (cubemap->create_from_images(faces) != OK)
                {
                    continue;
                }
                cubemap->set_meta("source_archive", bntx);
                cubemap->set_path(texture_path);
                texture = cubemap;
            }
            else
            {
                Ref<Image> image = texture_array->get_layer_image(0);
                if (image.is_null())
                {
                    continue;
                }
                Ref<ImageTexture> img_tex = ImageTexture::create_from_image(image);
                if (img_tex.is_null())
                {
                    continue;
                }
                img_tex->set_meta("source_archive", bntx);
                img_tex->set_path(texture_path);
                texture = img_tex;
            }
        }
        shdr->set_shader_parameter(tex_name, texture);
    }
}

void TrinityModel::_apply_params(const Ref<MaterialEntry>& mat, Ref<ShaderMaterial> shdr)
{
    auto apply = [&](const String& key)
    {
        Array params = mat->get(key);
        for (int i = 0; i < params.size(); i++)
        {
            Ref<Resource> p = params[i];
            shdr->set_shader_parameter(p->get("Name"), p->get("Value"));
        }
    };
    apply("FloatParams");
    apply("FloatLightParams");
    apply("Float4Params");
    apply("IntParams");
}

void TrinityModel::_apply_samplers(const Ref<MaterialEntry>& mat, Ref<ShaderMaterial> shdr)
{
    // A texture's sample_trinity slot is its index among the shader's 2D
    // texture uniforms, in declaration order (see trinity_sampler.gdshaderinc).
    Ref<Shader> shader = shdr->get_shader();
    if (shader.is_null())
    {
        return;
    }
    PackedStringArray slots;
    for (const Variant& entry : shader->get_shader_uniform_list())
    {
        Dictionary uniform = entry;
        if (int(uniform["type"]) == Variant::OBJECT && String(uniform["hint_string"]) == "Texture2D")
        {
            slots.push_back(uniform["name"]);
        }
    }
    if (slots.is_empty())
    {
        return;
    }

    // Missing textures use shader defaults without border-color darkening.
    PackedInt32Array repeat_u, repeat_v;
    PackedColorArray borders;
    repeat_u.resize(slots.size());
    repeat_v.resize(slots.size());
    borders.resize(slots.size());
    repeat_u.fill(7);
    repeat_v.fill(7);
    borders.fill(Color(0, 0, 0, 0));

    // TextureEntry::Slot stores a sampler ID, not a shader texture location.
    Array samps = mat->get_Samplers();
    Array textures = mat->get_Textures();
    for (int i = 0; i < textures.size(); i++)
    {
        Ref<TextureEntry> texture = textures[i];
        int slot = slots.find(texture->get_Name());
        if (slot < 0)
        {
            continue;
        }
        int sampler_id = texture->get_Slot();
        if (sampler_id < 0 || sampler_id >= samps.size())
        {
            UtilityFunctions::push_warning("Invalid sampler ID for ", texture->get_Name());
            continue;
        }
        Ref<SamplerEntry> samp = samps[sampler_id];
        repeat_u[slot] = samp->get_RepeatU();
        repeat_v[slot] = samp->get_RepeatV();
        borders[slot] = samp->get_BorderColor();
    }
    shdr->set_shader_parameter("sampler_repeat_u", repeat_u);
    shdr->set_shader_parameter("sampler_repeat_v", repeat_v);
    shdr->set_shader_parameter("sampler_border_color", borders);
}

void TrinityModel::_build_skeleton(const Ref<TRSkeleton>& skel, Skeleton3D*& skl, Ref<Skin>& skin)
{
    skl = memnew(Skeleton3D);
    skin.instantiate();

    Array transform_nodes = skel->get_TransformNodes();
    Ref<TransformNode> root_node = transform_nodes[0];
    String root_name = root_node->get_Name();

    set_name(root_name);
    skl->set_name(root_name);

    int bone_idx = 0;
    Dictionary node_to_bone_idx;

    for (int i = 0; i < transform_nodes.size(); i++)
    {
        Ref<TransformNode> node = transform_nodes[i];
        int parent_index = node->get_ParentIndex();

        String bone_name = node->get_Name();
        Transform3D xform = node->get_Transform();
        int rig_index = node->get_RigIndex();
        int nodeType = node->get_NodeType();

        skl->add_bone(bone_name);
        node_to_bone_idx[i] = bone_idx;
        skl->set_bone_rest(bone_idx, xform);

        if (parent_index >= 0 && node_to_bone_idx.has(parent_index))
        {
            skl->set_bone_parent(bone_idx, node_to_bone_idx[parent_index]);
        }

        if (rig_index >= 0)
        {
            Array bones = skel->get_Bones();
            Ref<BoneEntry> bone_entry = bones[rig_index];

            if (bone_entry->get_InfluenceSkinning())
            {
                Transform3D matrix = bone_entry->get_Matrix();
                skin->add_named_bind(bone_name, matrix);
            }

            if (bone_entry->get_InheritPosition())
            {
            }
        }

        bone_idx++;
    }

    add_child(skl);
    skl->reset_bone_poses();
}

PackedInt32Array TrinityModel::_flip_faces(const PackedInt32Array& indices)
{
    PackedInt32Array flipped;
    flipped.resize(indices.size());
    for (int i = 0; i < indices.size(); i += 3)
    {
        flipped[i] = indices[i + 2];
        flipped[i + 1] = indices[i + 1];
        flipped[i + 2] = indices[i];
    }
    return flipped;
}

void TrinityModel::_build_meshes(const Ref<TRMesh>& mesh, const Ref<TRModelBuffer>& buff, Skeleton3D* skl,
                                 const Ref<Skin>& skin)
{
    Array descriptors = mesh->get_MeshDescriptors();
    for (int d = 0; d < descriptors.size(); d++)
    {
        Ref<MeshShape> mesh_shape = descriptors[d];

        Ref<ModelBuffer> buf = buff->get_Buffers()[d];
        PackedByteArray vert_buf = buf->get_VertexBuffers()[0];
        PackedByteArray ind_buf = buf->get_IndexBuffers()[0];

        Array mat_list = mesh_shape->get_Materials();
        Array attr_list = mesh_shape->get_Attributes();
        int poly_type = mesh_shape->get_PolygonType();

        if (mat_list.is_empty())
        {
            continue;
        }
        const Dictionary result = parse_vertex_buffer(attr_list[0], vert_buf);
        Array vertex_arrays;
        vertex_arrays.resize(Mesh::ARRAY_MAX);
        PackedVector3Array pos = result["Pos"];
        if (!pos.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_VERTEX] = pos;
        }

        PackedVector3Array norm = result["Norm"];
        if (!norm.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_NORMAL] = norm;
        }

        PackedVector2Array uv = result["UV"];
        if (!uv.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_TEX_UV] = uv;
        }

        PackedVector2Array uv2 = result["UV2"];
        if (!uv2.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_TEX_UV2] = uv2;
        }

        PackedColorArray colors = result["Color"];
        if (!colors.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_COLOR] = colors;
        }

        PackedInt32Array blend_inds = result["BlendInds"];
        if (!blend_inds.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_BONES] = blend_inds;
        }

        PackedFloat32Array blend_weights = result["BlendWeights"];
        if (!blend_weights.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_WEIGHTS] = blend_weights;
        }

        PackedFloat32Array tangents = result["Tangents"];
        if (!tangents.is_empty())
        {
            vertex_arrays[Mesh::ARRAY_TANGENT] = tangents;
        }

        for (int sub = 0; sub < mat_list.size(); sub++)
        {
            Ref<MaterialInfo> mat = mat_list[sub];
            int poly_offset = mat->get_PolyOffset();
            int poly_count = mat->get_PolyCount();
            String material_name = mat->get_MaterialName();
            String mesh_name = mesh_shape->get_Name();

            // Occluders only feed the game's depth prepass; drawn here they
            // hide whatever they enclose.
            Ref<ShaderMaterial> surface_material = _materials.get(material_name, Variant());
            if (surface_material.is_valid() && surface_material->get_shader().is_valid() &&
                surface_material->get_shader()->get_path().get_file() == "DepthPrepass.gdshader")
            {
                continue;
            }

            Array arr = vertex_arrays.duplicate();
            PackedInt32Array indices = _flip_faces(parse_index_buffer(ind_buf, poly_type, poly_offset, poly_count));
            if (!indices.is_empty())
            {
                arr[Mesh::ARRAY_INDEX] = indices;
            }

            Ref<ArrayMesh> arr_mesh;
            arr_mesh.instantiate();
            arr_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr);

            MeshInstance3D* mi = memnew(MeshInstance3D);
            add_child(mi);

            mi->set_name(mesh_name + "_" + material_name);
            mi->set_mesh(arr_mesh);
            // Materials are assigned by name so variations can swap them later.
            mi->set_meta("trinity_mesh", mesh_name);
            mi->set_meta("trinity_material", material_name);
            if (skin.is_valid())
            {
                mi->set_skin(skin);
            }
            if (skl)
            {
                mi->set_skeleton_path(NodePath("../" + skl->get_name()));
            }
        }
    }
}

void TrinityModel::load_model(String path, String file, String variation)
{
    ResourceLoader* rl = ResourceLoader::get_singleton();

    Ref<TRModel> mdl = rl->load(path.path_join(file));
    BBox = mdl->get_BBox();

    Ref<TRMesh> mesh;
    String meshPath = mdl->get_Meshes()[0];
    if (!meshPath.is_empty() && FileAccess::file_exists(path.path_join(meshPath)))
    {
        mesh = rl->load(path.path_join(meshPath));
    }

    Ref<TRModelBuffer> buff;
    String buffPath = mesh->get_BufferName();
    if (!buffPath.is_empty() && FileAccess::file_exists(path.path_join(buffPath)))
    {
        buff = rl->load(path.path_join(buffPath));
    }

    _path = path;
    _default_material_files = mdl->get_Materials();
    _variation = variation;
    _control_values.clear();
    const String meta_path = path.path_join(file.get_basename() + ".trmmt");
    if (FileAccess::file_exists(meta_path))
    {
        _meta = rl->load(meta_path);
    }
    if (!_variation.is_empty() && _find_variation().is_null())
    {
        UtilityFunctions::push_warning("Model ", file, " has no variation '", _variation, "'");
        _variation = String();
    }
    // "normal" is the base look; using it by default makes its controls available.
    if (_variation.is_empty() && _meta.is_valid() && _meta->find_variation("normal").is_valid())
    {
        _variation = "normal";
    }
    _materials = _load_materials(path, _variation_material_files());

    Ref<TRSkeleton> skel;
    Ref<Skin> skin;
    Skeleton3D* skl = nullptr;
    Array skel_result;
    String skelPath = mdl->get_Skeleton();
    if (!skelPath.is_empty() && FileAccess::file_exists(path.path_join(skelPath)))
    {
        skel = rl->load(path.path_join(skelPath));
        _build_skeleton(skel, skl, skin);
    }

    _build_meshes(mesh, buff, skl, skin);
    _assign_materials();
    _apply_visibility();
    _apply_controls();
}

static int default_control_index(const Ref<VariationControl>& control)
{
    if (control->get_DefaultOverride() >= 0)
    {
        return control->get_DefaultOverride();
    }
    return control->get_Rule().get("initial", 0);
}

PackedStringArray TrinityModel::get_variations() const
{
    PackedStringArray names;
    if (_meta.is_null())
    {
        return names;
    }
    Array variations = _meta->get_Variations();
    for (int i = 0; i < variations.size(); i++)
    {
        Ref<ModelVariation> variation = variations[i];
        if (variation.is_valid())
        {
            names.push_back(variation->get_Name());
        }
    }
    return names;
}

void TrinityModel::set_variation(const String& p_name)
{
    // An empty name selects the base look, as in load_model.
    String name = p_name;
    if (name.is_empty() && _meta.is_valid() && _meta->find_variation("normal").is_valid())
    {
        name = "normal";
    }
    if (name == _variation)
    {
        return;
    }
    if (!name.is_empty() && (_meta.is_null() || _meta->find_variation(name).is_null()))
    {
        UtilityFunctions::push_warning("Model has no variation '", name, "'");
        return;
    }
    _variation = name;
    _materials = _load_materials(_path, _variation_material_files());
    _assign_materials();
    _apply_visibility();
    _apply_controls();
}

Dictionary TrinityModel::get_controls() const
{
    Dictionary controls;
    Ref<ModelVariation> variation = _find_variation();
    if (variation.is_null())
    {
        return controls;
    }
    Array list = variation->get_Controls();
    for (int i = 0; i < list.size(); i++)
    {
        Ref<VariationControl> control = list[i];
        if (control.is_valid() && control->get_TableDriven())
        {
            controls[control->get_Name()] = control->get_VariationCount();
        }
    }
    return controls;
}

int TrinityModel::get_control(const String& name) const
{
    Ref<VariationControl> control = _find_control(name);
    if (control.is_null())
    {
        return -1;
    }
    return _control_values.get(name, default_control_index(control));
}

void TrinityModel::set_control(const String& name, int index)
{
    Ref<VariationControl> control = _find_control(name);
    ERR_FAIL_COND_MSG(control.is_null() || !control->get_TableDriven(), "Model has no table control '" + name + "'");
    ERR_FAIL_INDEX_MSG(index, control->get_VariationCount(), "Control '" + name + "' index out of range");
    _control_values[name] = index;
    _apply_control(control, index);
    _apply_visibility();
}

Ref<ModelVariation> TrinityModel::_find_variation() const
{
    if (_meta.is_null() || _variation.is_empty())
    {
        return Ref<ModelVariation>();
    }
    return _meta->find_variation(_variation);
}

Ref<VariationControl> TrinityModel::_find_control(const String& name) const
{
    Ref<ModelVariation> variation = _find_variation();
    if (variation.is_null())
    {
        return Ref<VariationControl>();
    }
    Array controls = variation->get_Controls();
    for (int i = 0; i < controls.size(); i++)
    {
        Ref<VariationControl> control = controls[i];
        if (control.is_valid() && control->get_Name() == name)
        {
            return control;
        }
    }
    return Ref<VariationControl>();
}

Array TrinityModel::_variation_material_files() const
{
    // Variations without material files keep the model's own materials.
    Ref<ModelVariation> variation = _find_variation();
    if (variation.is_null() || variation->get_Materials().is_empty())
    {
        return _default_material_files;
    }
    Array files;
    for (const String& file : variation->get_Materials())
    {
        files.push_back(file);
    }
    return files;
}

void TrinityModel::_assign_materials()
{
    for (int i = 0; i < get_child_count(); i++)
    {
        MeshInstance3D* mi = Object::cast_to<MeshInstance3D>(get_child(i));
        if (!mi || !mi->has_meta("trinity_material"))
        {
            continue;
        }
        Ref<ShaderMaterial> material = _materials.get(mi->get_meta("trinity_material"), Variant());
        mi->set_material_override(material);
        // Retail field meshes have CastShadow=0 and cast through separate Shadow-shader proxies.
        // The sky dome and distant terrain are 0 too and would otherwise shadow the whole scene.
        bool cast_shadow = true;
        if (material.is_valid())
        {
            Variant value = material->get_shader_parameter("CastShadow");
            cast_shadow = !(value.get_type() == Variant::INT && int(value) == 0);
        }
        mi->set_cast_shadows_setting(cast_shadow ? GeometryInstance3D::SHADOW_CASTING_SETTING_ON
                                                 : GeometryInstance3D::SHADOW_CASTING_SETTING_OFF);
    }
}

void TrinityModel::_apply_control(const Ref<VariationControl>& control, int index)
{
    Dictionary materials = control->get_MaterialValues();
    Array material_names = materials.keys();
    for (int m = 0; m < material_names.size(); m++)
    {
        Ref<ShaderMaterial> material = _materials.get(material_names[m], Variant());
        if (material.is_null())
        {
            continue;
        }
        Dictionary params = materials[material_names[m]];
        Array param_names = params.keys();
        for (int p = 0; p < param_names.size(); p++)
        {
            Array values = params[param_names[p]];
            if (index >= values.size())
            {
                continue;
            }
            const StringName name = param_names[p];
            Variant value = values[index];
            // Match the type already on the material (vec4 uniforms, bool options).
            Variant current = material->get_shader_parameter(name);
            if (current.get_type() == Variant::VECTOR4 && value.get_type() == Variant::VECTOR3)
            {
                Vector3 v = value;
                value = Vector4(v.x, v.y, v.z, Vector4(current).w);
            }
            else if (current.get_type() == Variant::BOOL && value.get_type() == Variant::INT)
            {
                value = int(value) != 0;
            }
            material->set_shader_parameter(name, value);
        }
    }
}

void TrinityModel::_apply_controls()
{
    Ref<ModelVariation> variation = _find_variation();
    if (variation.is_null())
    {
        return;
    }
    Array controls = variation->get_Controls();
    for (int i = 0; i < controls.size(); i++)
    {
        Ref<VariationControl> control = controls[i];
        if (control.is_null() || !control->get_TableDriven())
        {
            continue;
        }
        _apply_control(control, _control_values.get(control->get_Name(), default_control_index(control)));
    }
}

void TrinityModel::_apply_visibility()
{
    Ref<ModelVariation> variation = _find_variation();
    Dictionary variation_visibility = variation.is_valid() ? variation->get_MeshVisibility() : Dictionary();
    Array controls = variation.is_valid() ? variation->get_Controls() : Array();

    for (int i = 0; i < get_child_count(); i++)
    {
        MeshInstance3D* mi = Object::cast_to<MeshInstance3D>(get_child(i));
        if (!mi || !mi->has_meta("trinity_mesh"))
        {
            continue;
        }
        const String mesh = mi->get_meta("trinity_mesh");
        bool visible = variation_visibility.get(mesh, true);
        for (int c = 0; c < controls.size() && visible; c++)
        {
            Ref<VariationControl> control = controls[c];
            if (control.is_null() || !control->get_TableDriven())
            {
                continue;
            }
            Dictionary table = control->get_MeshVisibility();
            if (!table.has(mesh))
            {
                continue;
            }
            Array values = table[mesh];
            const int index = _control_values.get(control->get_Name(), default_control_index(control));
            if (index < values.size())
            {
                visible = values[index];
            }
        }
        mi->set_visible(visible);
    }
}
