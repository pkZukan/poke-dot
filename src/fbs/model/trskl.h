#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/Ikkaku/Model/trskl_generated.h"
#include "utils.h"

namespace godot
{

class IKControl : public Resource
{
    GDCLASS(IKControl, Resource)
protected:
    static void _bind_methods();

public:
    IKControl() = default;
    ~IKControl() = default;

    GETTER_SETTER_DEFINE(String, Name)

private:
    String Name;
};

class TransformNode : public Resource
{
    GDCLASS(TransformNode, Resource)
protected:
    static void _bind_methods();

public:
    TransformNode() = default;
    ~TransformNode() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Transform3D, Transform)
    GETTER_SETTER_DEFINE(Vector3, ScalePivot)
    GETTER_SETTER_DEFINE(Vector3, RotatePivot)
    GETTER_SETTER_DEFINE(int, ParentIndex)
    GETTER_SETTER_DEFINE(int, RigIndex)
    GETTER_SETTER_DEFINE(String, ParentName)
    GETTER_SETTER_DEFINE(int, NodeType)
    GETTER_SETTER_DEFINE(bool, unk_0)
    GETTER_SETTER_DEFINE(bool, ignore_parent_rot)

private:
    String Name;
    Transform3D Transform;
    Vector3 ScalePivot;
    Vector3 RotatePivot;
    int ParentIndex;
    int RigIndex;
    String ParentName;
    int NodeType;
    bool unk_0;
    bool ignore_parent_rot;
};

class BoneEntry : public Resource
{
    GDCLASS(BoneEntry, Resource)
protected:
    static void _bind_methods();

public:
    BoneEntry() = default;
    ~BoneEntry() = default;

    GETTER_SETTER_DEFINE(bool, InheritPosition)
    GETTER_SETTER_DEFINE(bool, InfluenceSkinning)
    GETTER_SETTER_DEFINE(Transform3D, Matrix)

private:
    bool InheritPosition;
    bool InfluenceSkinning;
    Transform3D Matrix;
};

class TRSkeleton : public Resource
{
    GDCLASS(TRSkeleton, Resource)
protected:
    static void _bind_methods();

public:
    TRSkeleton() = default;
    ~TRSkeleton() = default;

    void LoadFromFile(String file);

    GETTER_SETTER_DEFINE(int, Version)
    GETTER_SETTER_DEFINE(Array, TransformNodes)
    GETTER_SETTER_DEFINE(Array, Bones)
    GETTER_SETTER_DEFINE(Array, IKs)
    GETTER_SETTER_DEFINE(int, RigOffset)

private:
    int Version;
    Array TransformNodes;
    Array Bones;
    Array IKs;
    int RigOffset;
};

class ResourceFormatLoaderTRSKL : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderTRSKL, ResourceFormatLoader)
protected:
    static void _bind_methods()
    {
    }

public:
    ResourceFormatLoaderTRSKL() = default;
    ~ResourceFormatLoaderTRSKL() = default;

    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual bool _handles_type(const StringName& p_type) const override;
    virtual Variant _load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                          int32_t p_cache_mode) const override;
};

}
