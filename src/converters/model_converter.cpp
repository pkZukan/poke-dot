#include "model_converter.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/skin.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void TrinityModel::_bind_methods() {
    ClassDB::bind_method(D_METHOD("load_model", "path", "file"), &TrinityModel::load_model);
    ClassDB::bind_static_method("TrinityModel",
        D_METHOD("parse_mesh_buffer", "accessor_table", "verts", "inds", "poly_type", "start", "count"),
        &TrinityModel::parse_mesh_buffer
    );
}

// ---------------------------------------------------------------------------
// parse_mesh_buffer
// ---------------------------------------------------------------------------
Dictionary TrinityModel::parse_mesh_buffer(
    const Ref<VertexAccessors>& accessor_table,
    const PackedByteArray& verts,
    const PackedByteArray& inds,
    int poly_type, int start, int count)
{
    PackedVector3Array  pos, norm;
    PackedVector2Array  uv;
    PackedInt32Array    indices, blend_inds;
    PackedFloat32Array  blend_weights;

    Array strides = accessor_table->get_Strides();
    Ref<SizeTable> stride_obj = strides[0];
    int stride = stride_obj->get_Size();

    Ref<StreamPeerBuffer> stream_vert;
    stream_vert.instantiate();
    stream_vert->set_data_array(verts);

    Ref<StreamPeerBuffer> stream_ind;
    stream_ind.instantiate();
    stream_ind->set_data_array(inds);

    Array accessors = accessor_table->get("Accessors");

    int curr_pos = 0;
    while (curr_pos < stream_vert->get_size()) 
    {
        for (int a = 0; a < accessors.size(); a++) 
        {
            Ref<Resource> attrib = accessors[a];
            int attr_pos = attrib->get("Position");
            String attr_name = attrib->get("Attribute");
            stream_vert->seek(curr_pos + attr_pos);

            if (attr_name == "POSITION") 
            {
                float x = stream_vert->get_float();
                float y = stream_vert->get_float();
                float z = stream_vert->get_float();
                pos.push_back(Vector3(x, y, z));
            } 
            else if (attr_name == "NORMAL") 
            {
                float x = Utils::half_to_float(stream_vert->get_u16());
                float y = Utils::half_to_float(stream_vert->get_u16());
                float z = Utils::half_to_float(stream_vert->get_u16());
                stream_vert->get_u16();
                norm.push_back(Vector3(x, y, z).normalized());
            } 
            else if (attr_name == "TEXCOORD") 
            {
                float u = stream_vert->get_float();
                float v = stream_vert->get_float();
                uv.push_back(Vector2(u, v));
            } 
            else if (attr_name == "BLEND_INDICES") 
            {
                blend_inds.push_back(stream_vert->get_8());
                blend_inds.push_back(stream_vert->get_8());
                blend_inds.push_back(stream_vert->get_8());
                blend_inds.push_back(stream_vert->get_8());
            } 
            else if (attr_name == "BLEND_WEIGHTS") 
            {
                float w1 = Utils::half_to_float(stream_vert->get_u16());
                float w2 = Utils::half_to_float(stream_vert->get_u16());
                float w3 = Utils::half_to_float(stream_vert->get_u16());
                float w4 = Utils::half_to_float(stream_vert->get_u16());
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
        }
        curr_pos += stride;
    }

    int ind_size = (1 << poly_type);
    curr_pos = start * ind_size;
    stream_ind->seek(curr_pos);
    int end = (start + count) * ind_size;
    while (curr_pos < end) {
        switch (poly_type) {
            case 0: indices.push_back(stream_ind->get_u8());  break;
            case 1: indices.push_back(stream_ind->get_u16()); break;
            case 2: indices.push_back(stream_ind->get_u32()); break;
            case 3: indices.push_back((int32_t)stream_ind->get_u64()); break;
        }
        curr_pos += ind_size;
    }

    Dictionary result;
    result["Pos"]          = pos;
    result["Norm"]         = norm;
    result["UV"]           = uv;
    result["Indicies"]     = indices;
    result["BlendInds"]    = blend_inds;
    result["BlendWeights"] = blend_weights;
    return result;
}

// ---------------------------------------------------------------------------
// _load_materials
// ---------------------------------------------------------------------------
Dictionary TrinityModel::_load_materials(const String& path, const Array& material_files) {
    Dictionary materials;
    ResourceLoader* rl = ResourceLoader::get_singleton();

    for (int i = 0; i < material_files.size(); i++) {
        Ref<TRMaterial> material = rl->load(path + (String)material_files[i]);
        Array mats = material->get_Materials();
        for (int m = 0; m < mats.size(); m++) {
            Ref<MaterialEntry> mat = mats[m];
            Ref<ShaderMaterial> shdr = _build_shader_material(mat);
            _apply_textures(path, mat, shdr);
            _apply_params(mat, shdr);
            materials[mat->get("Name")] = shdr;
        }
    }
    return materials;
}

// ---------------------------------------------------------------------------
// _build_shader_material
// ---------------------------------------------------------------------------
Ref<ShaderMaterial> TrinityModel::_build_shader_material(const Ref<MaterialEntry>& mat) {
    Ref<ShaderMaterial> sm;
    sm.instantiate();

    Array shaders = mat->get_Shaders();
    Ref<ShaderEntry> shader = shaders[0];
    String shader_name = shader->get_Name();

    sm->set_name(mat->get_Name());
    sm->set_shader(ResourceLoader::get_singleton()->load(
        "res://gflib/shaders/" + shader_name + ".gdshader"
    ));

    Array string_params = shader->get_StringParams();
    for (int i = 0; i < string_params.size(); i++) {
        Ref<ShaderStringParam> v = string_params[i];
        String name  = v->get_Name();
        String val   = v->get_Value();

        if (val.is_valid_int())
            sm->set_shader_parameter(name, val.to_int());
        else if (val.to_lower() == "true")
            sm->set_shader_parameter(name, true);
        else if (val.to_lower() == "false")
            sm->set_shader_parameter(name, false);
    }
    return sm;
}

// ---------------------------------------------------------------------------
// _apply_textures
// ---------------------------------------------------------------------------
void TrinityModel::_apply_textures(const String& path, const Ref<Resource>& mat, Ref<ShaderMaterial> shdr) {
    Array textures = mat->get("Textures");
    for (int i = 0; i < textures.size(); i++) {
        Ref<Resource> t = textures[i];
        String file = t->get("File");
        String tex_name = t->get("Name");

        Ref<Resource> res = ResourceLoader::get_singleton()->load(
            path + file, "", ResourceLoader::CACHE_MODE_IGNORE
        );
        if (!res.is_valid()) 
            continue;

        Ref<Image> img_data = res->get("ImageData");
        if (!img_data.is_valid()) 
            continue;

        Ref<ImageTexture> img_tex = ImageTexture::create_from_image(img_data);
        if (img_tex.is_valid())
            shdr->set_shader_parameter(tex_name, img_tex);
    }
}

// ---------------------------------------------------------------------------
// _apply_params
// ---------------------------------------------------------------------------
void TrinityModel::_apply_params(const Ref<Resource>& mat, Ref<ShaderMaterial> shdr) {
    auto apply = [&](const String& key) {
        Array params = mat->get(key);
        for (int i = 0; i < params.size(); i++) {
            Ref<Resource> p = params[i];
            shdr->set_shader_parameter(p->get("Name"), p->get("Value"));
        }
    };
    apply("FloatParams");
    apply("FloatLightParams");
    apply("Float4Params");
    apply("IntParams");
}

// ---------------------------------------------------------------------------
// _build_skeleton
// ---------------------------------------------------------------------------
Array TrinityModel::_build_skeleton(const Ref<TRSkeleton>& skel) {
    Skeleton3D* skl = memnew(Skeleton3D);
    Ref<Skin> skin;
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
        int parent_index = (int)node->get_ParentIndex();
        if (parent_index == -1) 
            continue;

        String bone_name = node->get_Name();
        Transform3D xform = node->get_Transform();
        int rig_index = node->get_RigIndex();
        int nodeType = node->get_NodeType();

        skl->add_bone(bone_name);
        skl->set_bone_rest(bone_idx, xform);
        
        node_to_bone_idx[i] = bone_idx;

        if (node_to_bone_idx.has(parent_index))
            skl->set_bone_parent(bone_idx, (int)node_to_bone_idx[parent_index]);

        if (rig_index >= 0) {
            Array bones = skel->get_Bones();
            Ref<BoneEntry> bone_entry = bones[rig_index];
            
            if (bone_entry->get_InfluenceSkinning()) 
            {
                Transform3D matrix = bone_entry->get_Matrix();
                skin->add_named_bind(bone_name, matrix);
            }
        } 
        else 
        {
            skin->add_named_bind(bone_name, Transform3D());
        }

        bone_idx++;
    }

    add_child(skl);

    Array result;
    result.push_back(skl);
    result.push_back(skin);
    return result;
}

// ---------------------------------------------------------------------------
// _build_meshes
// ---------------------------------------------------------------------------
PackedInt32Array TrinityModel::_flip_faces(const PackedInt32Array& indices) 
{
    PackedInt32Array flipped;
    flipped.resize(indices.size());
    for (int i = 0; i < indices.size(); i += 3) {
        flipped[i]   = indices[i + 2];
        flipped[i+1] = indices[i + 1];
        flipped[i+2] = indices[i];
    }
    return flipped;
}

void TrinityModel::_build_meshes(
    const String& path,
    const Ref<TRMesh>& mesh,
    const Ref<TRModelBuffer>& buff,
    const Dictionary& materials,
    Skeleton3D* skl,
    const Ref<Skin>& skin)
{
    Array descriptors = mesh->get_MeshDescriptors();
    for (int d = 0; d < descriptors.size(); d++) 
    {
        Ref<MeshShape> mesh_shape = descriptors[d];

        Ref<ModelBuffer> buf = buff->get_Buffers()[d];
        PackedByteArray vert_buf = buf->get_VertexBuffers()[0];
        PackedByteArray ind_buf = buf->get_IndexBuffers()[0];

        Array mat_list  = mesh_shape->get_Materials();
        Array attr_list = mesh_shape->get_Attributes();
        int poly_type = mesh_shape->get_PolygonType();

        for (int sub = 0; sub < mat_list.size(); sub++) 
        {
            Ref<MaterialInfo> mat = mat_list[sub];
            int poly_offset = mat->get_PolyOffset();
            int poly_count = mat->get_PolyCount();
            String material_name = mat->get_MaterialName();
            String mesh_name = mesh_shape->get_Name();
            
            Dictionary result = parse_mesh_buffer(attr_list[0], vert_buf, ind_buf, poly_type, poly_offset, poly_count);
            result["Indicies"] = _flip_faces(result["Indicies"]);

            Array arr;
            arr.resize(Mesh::ARRAY_MAX);
            arr[Mesh::ARRAY_VERTEX]  = result["Pos"];
            arr[Mesh::ARRAY_NORMAL]  = result["Norm"];
            arr[Mesh::ARRAY_TEX_UV]  = result["UV"];
            arr[Mesh::ARRAY_INDEX]   = result["Indicies"];
            arr[Mesh::ARRAY_BONES]   = result["BlendInds"];
            arr[Mesh::ARRAY_WEIGHTS] = result["BlendWeights"];

            Ref<ArrayMesh> arr_mesh;
            arr_mesh.instantiate();
            arr_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arr);

            MeshInstance3D* mi = memnew(MeshInstance3D);
            add_child(mi);

            mi->set_skin(skin);
            mi->set_name(mesh_name + "_" + material_name);
            mi->set_mesh(arr_mesh);
            mi->set_material_override(materials.get(material_name, Variant()));
            mi->set_skeleton_path(NodePath("../" + skl->get_name()));
        }
    }
}

// ---------------------------------------------------------------------------
// load_model
// ---------------------------------------------------------------------------
void TrinityModel::load_model(String path, String file) {
    ResourceLoader* rl = ResourceLoader::get_singleton();

    Ref<TRModel> mdl  = rl->load(path + file);
    Ref<TRMesh> mesh = rl->load(path + (String)mdl->get_Meshes()[0]);
    Ref<TRModelBuffer> buff = rl->load(path + (String)mesh->get_BufferName());
    Ref<TRSkeleton> skel = rl->load(path + (String)mdl->get_Skeleton());

    Dictionary materials = _load_materials(path, mdl->get_Materials());
    Array skel_result = _build_skeleton(skel);

    Skeleton3D* skl = Object::cast_to<Skeleton3D>(skel_result[0]);
    Ref<Skin> skn = skel_result[1];

    _build_meshes(path, mesh, buff, materials, skl, skn);
}