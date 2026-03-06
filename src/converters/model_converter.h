#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/skeleton3d.hpp>
#include <godot_cpp/classes/skin.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include "model_fbs/trmdl.h"
#include "model_fbs/trskl.h"
#include "model_fbs/trmsh.h"
#include "model_fbs/trmmt.h"
#include "model_fbs/trmtr.h"
#include "model_fbs/trmbf.h"

namespace godot {

class TrinityModel : public Node3D {
    GDCLASS(TrinityModel, Node3D)

protected:
    static void _bind_methods();

public:
    void load_model(String path, String file);

    static Dictionary parse_mesh_buffer(
        const Ref<VertexAccessors>& accessor_table,
        const PackedByteArray& verts,
        const PackedByteArray& inds,
        int poly_type, int start, int count
    );

private:
    Dictionary _load_materials(const String& path, const Array& material_files);
    Ref<ShaderMaterial> _build_shader_material(const Ref<MaterialEntry>& mat);
    void _apply_textures(const String& path, const Ref<Resource>& mat, Ref<ShaderMaterial> shdr);
    void _apply_params(const Ref<Resource>& mat, Ref<ShaderMaterial> shdr);
    Array _build_skeleton(const Ref<TRSkeleton>& skel);
    PackedInt32Array _flip_faces(const PackedInt32Array& indices);
    void _build_meshes(const Ref<TRMesh>& mesh, const Ref<TRModelBuffer>& buff, const Dictionary& materials, Skeleton3D* skl, const Ref<Skin>& skin);
};

} // namespace godot