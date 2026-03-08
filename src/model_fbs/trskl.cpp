#include "trskl.h"

using namespace godot;

void IKControl::_bind_methods() 
{
    GETTER_SETTER_BIND(IKControl, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void TransformNode::_bind_methods() 
{
    GETTER_SETTER_BIND(TransformNode, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, Transform, Variant::TRANSFORM3D, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, ScalePivot, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, RotatePivot, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, ParentIndex, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, RigIndex, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, ParentName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, NodeType, Variant::INT, PROPERTY_HINT_ENUM, "Normal,Chained,Leaf")
    GETTER_SETTER_BIND(TransformNode, unk_0, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TransformNode, ignore_parent_rot, Variant::BOOL, PROPERTY_HINT_NONE)
}

void BoneEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(BoneEntry, InheritPosition, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneEntry, InfluenceSkinning, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneEntry, Matrix, Variant::TRANSFORM3D, PROPERTY_HINT_NONE)
}

void TRSkeleton::_bind_methods() 
{
    GETTER_SETTER_BIND(TRSkeleton, Version, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSkeleton, TransformNodes, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TransformNode")
    GETTER_SETTER_BIND(TRSkeleton, Bones, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "BoneEntry")
    GETTER_SETTER_BIND(TRSkeleton, IKs, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "IKControl")
    GETTER_SETTER_BIND(TRSkeleton, RigOffset, Variant::INT, PROPERTY_HINT_NONE)
}

void TRSkeleton::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRSKL file: %s", file));
    
    auto skel = Titan::Model::GetTRSKL(buf.ptr());
    ERR_FAIL_COND_MSG(skel == NULL, vformat("Couldn't load TRSKL flatbuffer"));

    //Get Bones
    auto bones = skel->bones();
    for(int i = 0; i < bones->size(); i++)
    {
        auto boneEnt = bones->Get(i);
        Ref<BoneEntry> bone;
        bone.instantiate();

        bone->set_InheritPosition(boneEnt->inherit_position());

        auto matrix = boneEnt->matrix();
        Transform3D boneMatrix(
            Basis(
                Utils::toGodotVec3(matrix->x()),
                Utils::toGodotVec3(matrix->y()),
                Utils::toGodotVec3(matrix->z())
            ),
            Utils::toGodotVec3(matrix->w())
        );
        bone->set_InfluenceSkinning(boneEnt->influence_skinning());
        bone->set_Matrix(boneMatrix);        

        Bones.push_back(bone);
    }

    //Get transforms
    auto transforms = skel->transform_nodes();
    for(int i = 0; i < transforms->size(); i++)
    {
        Ref<TransformNode> tn;
        tn.instantiate();
        tn->set_Name(Utils::toGodotString(transforms->Get(i)->name()));

        Vector3 pos = Utils::toGodotVec3(transforms->Get(i)->transform()->VecTranslate());
        Vector3 rot = Utils::toGodotVec3(transforms->Get(i)->transform()->VecRot());
        Vector3 scale = Utils::toGodotVec3(transforms->Get(i)->transform()->VecScale());
        Basis scale_basis = Basis::from_scale(scale);
        Basis rot_basis = Basis::from_euler(rot, EulerOrder::EULER_ORDER_ZYX);
        tn->set_Transform(Transform3D(scale_basis * rot_basis, pos));
        
        tn->set_ScalePivot(Utils::toGodotVec3(transforms->Get(i)->scalePivot()));
        tn->set_RotatePivot(Utils::toGodotVec3(transforms->Get(i)->rotatePivot()));
        tn->set_ParentIndex(transforms->Get(i)->parent_idx());
        tn->set_RigIndex(transforms->Get(i)->rig_idx());
        tn->set_ParentName(Utils::toGodotString(transforms->Get(i)->parent_name()));
        tn->set_NodeType(transforms->Get(i)->type());
        tn->set_unk_0(transforms->Get(i)->unk_0());
        tn->set_ignore_parent_rot(transforms->Get(i)->ignore_parent_rot());

        TransformNodes.push_back(tn);
    }

    //Get IKs
    auto ikcontrols = skel->iks();
    for(int i = 0; i < ikcontrols->size(); i++)
    {
        Ref<IKControl> ik;
        ik.instantiate();

        ik->set_Name(Utils::toGodotString(ikcontrols->Get(i)->ik_name()));

        IKs.push_back(ik);
    }

    Version = skel->version();
    RigOffset = skel->rig_offset();
}

Variant ResourceFormatLoaderTRSKL::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRSkeleton> trskl;
    trskl.instantiate();
    trskl->LoadFromFile(p_path);
    return trskl;
}

PackedStringArray ResourceFormatLoaderTRSKL::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trskl");

    return exts;
}

bool ResourceFormatLoaderTRSKL::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRSkeleton");
}