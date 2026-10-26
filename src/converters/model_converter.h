#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/skin.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/script.hpp>
#include "middleware/bntx.h"
#include "fbs/model/trmdl.h"
#include "fbs/model/trskl.h"
#include "fbs/model/trmsh.h"
#include "fbs/model/trmmt.h"
#include "fbs/model/trmtr.h"
#include "fbs/model/trmbf.h"

namespace godot
{

class TrinityModel : public Node3D
{
    GDCLASS(TrinityModel, Node3D)

protected:
    static void _bind_methods();

public:
    void load_model(String path, String file, String variation = String());

    // TRMMT variations swap material sets; table controls apply numbered
    // presets of shader parameters and mesh visibility.
    PackedStringArray get_variations() const;

    String get_variation() const
    {
        return _variation;
    }

    void set_variation(const String& p_name);
    Dictionary get_controls() const;
    int get_control(const String& name) const;
    void set_control(const String& name, int index);

    static Dictionary parse_mesh_buffer(const Ref<VertexAccessors>& accessor_table, const PackedByteArray& verts,
                                        const PackedByteArray& inds, int poly_type, int start, int count);
    AABB BBox;

private:
    String _path;
    Array _default_material_files;
    Ref<TRModelMeta> _meta;
    String _variation;
    Dictionary _materials;
    Dictionary _control_values;

    Ref<ModelVariation> _find_variation() const;
    Ref<VariationControl> _find_control(const String& name) const;
    Array _variation_material_files() const;
    void _assign_materials();
    void _apply_control(const Ref<VariationControl>& control, int index);
    void _apply_controls();
    void _apply_visibility();
    static Dictionary parse_vertex_buffer(const Ref<VertexAccessors>& accessor_table, const PackedByteArray& verts);
    static PackedInt32Array parse_index_buffer(const PackedByteArray& inds, int poly_type, int start, int count);
    Dictionary _load_materials(const String& path, const Array& material_files);
    Ref<ShaderMaterial> _build_shader_material(const Ref<MaterialEntry>& mat);
    void _apply_textures(const String& path, const Ref<MaterialEntry>& mat, Ref<ShaderMaterial> shdr);
    void _apply_params(const Ref<MaterialEntry>& mat, Ref<ShaderMaterial> shdr);
    void _apply_samplers(const Ref<MaterialEntry>& mat, Ref<ShaderMaterial> shdr);
    void _build_skeleton(const Ref<TRSkeleton>& skel, Skeleton3D*& skl, Ref<Skin>& skin);
    PackedInt32Array _flip_faces(const PackedInt32Array& indices);
    void _build_meshes(const Ref<TRMesh>& mesh, const Ref<TRModelBuffer>& buff, Skeleton3D* skl, const Ref<Skin>& skin);

    void print_bone_tree_compact(Skeleton3D* skeleton);
    void _print_compact_recursive(Skeleton3D* skeleton, int idx, String prefix, bool is_last,
                                  std::vector<int>& printed);
};

}
