#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/Ikkaku/TrinityScene/trinity_CollisionComponent_generated.h"
#include <utils.h>

namespace godot
{

class ModelShape : public Resource
{
    GDCLASS(ModelShape, Resource)
protected:
    static void _bind_methods();

public:
    ModelShape() = default;
    ~ModelShape() = default;

    GETTER_SETTER_DEFINE(String, path)
    GETTER_SETTER_DEFINE(Vector3, pos)
    GETTER_SETTER_DEFINE(Vector3, rot)
    GETTER_SETTER_DEFINE(Vector3, scale)

private:
    String path;
    Vector3 pos;
    Vector3 rot;
    Vector3 scale = Vector3(1, 1, 1);
};

class SphereShape : public Resource
{
    GDCLASS(SphereShape, Resource)
protected:
    static void _bind_methods();

public:
    SphereShape() = default;
    ~SphereShape() = default;

    GETTER_SETTER_DEFINE(Vector3, pos)
    GETTER_SETTER_DEFINE(float, radius)

private:
    Vector3 pos;
    float radius;
};

class BoxShape : public Resource
{
    GDCLASS(BoxShape, Resource)
protected:
    static void _bind_methods();

public:
    BoxShape() = default;
    ~BoxShape() = default;

    GETTER_SETTER_DEFINE(Vector3, pos)
    GETTER_SETTER_DEFINE(Vector3, size)
    GETTER_SETTER_DEFINE(Vector3, rot)

private:
    Vector3 pos;
    Vector3 size;
    Vector3 rot;
};

class CapsuleShape : public Resource
{
    GDCLASS(CapsuleShape, Resource)
protected:
    static void _bind_methods();

public:
    CapsuleShape() = default;
    ~CapsuleShape() = default;

    GETTER_SETTER_DEFINE(Vector3, pos)
    GETTER_SETTER_DEFINE(float, radius)
    GETTER_SETTER_DEFINE(float, height)

private:
    Vector3 pos;
    float radius;
    float height;
};

class PencilShape : public Resource
{
    GDCLASS(PencilShape, Resource)
protected:
    static void _bind_methods();

public:
    PencilShape() = default;
    ~PencilShape() = default;

    GETTER_SETTER_DEFINE(Vector3, pos)
    GETTER_SETTER_DEFINE(float, radius)
    GETTER_SETTER_DEFINE(float, cyl_height)
    GETTER_SETTER_DEFINE(float, tip_height)
    GETTER_SETTER_DEFINE(int, slice_count)
    GETTER_SETTER_DEFINE(Vector3, rot)

private:
    Vector3 pos;
    float radius;
    float cyl_height;
    float tip_height;
    int slice_count;
    Vector3 rot;
};

class TrinityBodyCollision : public Resource
{
    GDCLASS(TrinityBodyCollision, Resource)
protected:
    static void _bind_methods();

public:
    TrinityBodyCollision() = default;
    ~TrinityBodyCollision() = default;

    GETTER_SETTER_DEFINE(Ref<Resource>, shape)

private:
    Ref<Resource> shape;
};

class TrinityCharacterCollision : public Resource
{
    GDCLASS(TrinityCharacterCollision, Resource)
protected:
    static void _bind_methods();

public:
    TrinityCharacterCollision() = default;
    ~TrinityCharacterCollision() = default;

    GETTER_SETTER_DEFINE(Ref<Resource>, shape)

private:
    Ref<Resource> shape;
};

class TrinityCollisionComponent : public Resource
{
    GDCLASS(TrinityCollisionComponent, Resource)
protected:
    static void _bind_methods();

public:
    TrinityCollisionComponent() = default;
    ~TrinityCollisionComponent() = default;

    Ref<Resource> _ParseShapeType(Ikkaku::TrinityScene::ColShapeType col_shape, const void* data);
    Ref<Resource> _ParseCollisionType(Ikkaku::TrinityScene::Collision collision, const void* data);

    void LoadFromBuffer(const void* buffer);

    GETTER_SETTER_DEFINE(Ref<Resource>, collision)

private:
    Ref<Resource> collision;
};

}
