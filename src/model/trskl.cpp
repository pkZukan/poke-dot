#include "trskl.h"

using namespace godot;

void BoneMatrix::_bind_methods() 
{
    GETTER_SETTER_BIND(BoneMatrix, X, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneMatrix, Y, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneMatrix, Z, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneMatrix, W, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

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
}

void BoneEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(BoneEntry, InheritPosition, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneEntry, unk0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneEntry, Matrix, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "BoneMatrix")
}

void TRSkeleton::_bind_methods() 
{
    GETTER_SETTER_BIND(TRSkeleton, TransformNodes, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TransformNode")
    GETTER_SETTER_BIND(TRSkeleton, Bones, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "BoneEntry")
    GETTER_SETTER_BIND(TRSkeleton, IKs, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "IKControl")
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
        Ref<BoneMatrix> bm;
        bm.instantiate();
        bm->set_X(Utils::toGodotVec3(matrix->x()));
        bm->set_Y(Utils::toGodotVec3(matrix->y()));
        bm->set_Z(Utils::toGodotVec3(matrix->z()));
        bm->set_W(Utils::toGodotVec3(matrix->w()));

        bone->set_Matrix(bm);        

        Bones.push_back(bone);
    }

    //Get transforms
    auto transforms = skel->transform_nodes();
    for(int i = 0; i < transforms->size(); i++)
    {
        Ref<TransformNode> tn;
        tn.instantiate();
        tn->set_Name(Utils::toGodotString(transforms->Get(i)->name()));

        Transform3D tr;
        tr.scale(Utils::toGodotVec3(transforms->Get(i)->transform()->VecScale()));
        tr.basis = Quaternion(Utils::toGodotVec3(transforms->Get(i)->transform()->VecRot()));
        tr.origin = Utils::toGodotVec3(transforms->Get(i)->transform()->VecTranslate());
        tn->set_Transform(tr);

        tn->set_ScalePivot(Utils::toGodotVec3(transforms->Get(i)->scalePivot()));
        tn->set_RotatePivot(Utils::toGodotVec3(transforms->Get(i)->rotatePivot()));
        tn->set_ParentIndex(transforms->Get(i)->parent_idx());

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