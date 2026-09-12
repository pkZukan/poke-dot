#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_CollisionComponent_generated.h"
#include <utils.h>

namespace godot {

class ModelShape : public Resource {
    GDCLASS(ModelShape, Resource)
protected:
	static void _bind_methods();
public:
	ModelShape(){}
	~ModelShape(){}

	GETTER_SETTER_DEFINE(String, path)

private:
    String path;
};

class SphereShape : public Resource {
    GDCLASS(SphereShape, Resource)
protected:
	static void _bind_methods();
public:
	SphereShape(){}
	~SphereShape(){}

	GETTER_SETTER_DEFINE(Vector3, pos)
	GETTER_SETTER_DEFINE(float, radius)

private:
    Vector3 pos;
    float radius;
};

class BoxShape : public Resource {
    GDCLASS(BoxShape, Resource)
protected:
	static void _bind_methods();
public:
	BoxShape(){}
	~BoxShape(){}

	GETTER_SETTER_DEFINE(Vector3, pos)
	GETTER_SETTER_DEFINE(Vector3, size)
	GETTER_SETTER_DEFINE(Vector3, rot)

private:
    Vector3 pos;
    Vector3 size;
	Vector3 rot;
};

class CapsuleShape : public Resource {
    GDCLASS(CapsuleShape, Resource)
protected:
	static void _bind_methods();
public:
	CapsuleShape(){}
	~CapsuleShape(){}

	GETTER_SETTER_DEFINE(Vector3, pos)
	GETTER_SETTER_DEFINE(float, radius)
	GETTER_SETTER_DEFINE(float, height)

private:
    Vector3 pos;
    float radius;
    float height;
};

class PencilShape : public Resource {
    GDCLASS(PencilShape, Resource)
protected:
	static void _bind_methods();
public:
	PencilShape(){}
	~PencilShape(){}

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

class TrinityBodyCollision : public Resource {
    GDCLASS(TrinityBodyCollision, Resource)
protected:
	static void _bind_methods();
public:
	TrinityBodyCollision(){}
	~TrinityBodyCollision(){}

	GETTER_SETTER_DEFINE(Ref<Resource>, shape)

private:
    Ref<Resource> shape;
};

class TrinityCharacterCollision : public Resource {
    GDCLASS(TrinityCharacterCollision, Resource)
protected:
	static void _bind_methods();
public:
	TrinityCharacterCollision(){}
	~TrinityCharacterCollision(){}

	GETTER_SETTER_DEFINE(Ref<Resource>, shape)

private:
    Ref<Resource> shape;
};

class TrinityCollisionComponent : public Resource {
    GDCLASS(TrinityCollisionComponent, Resource)
protected:
	static void _bind_methods();
public:
	TrinityCollisionComponent(){}
	~TrinityCollisionComponent(){}

	Ref<Resource> _ParseShapeType(Titan::TrinityScene::ColShapeType col_shape, const void* data);
	Ref<Resource> _ParseCollisionType(Titan::TrinityScene::Collision collision, const void* data);

    void LoadFromBuffer(const void* buffer);

	GETTER_SETTER_DEFINE(Ref<Resource>, collision)

private:
    Ref<Resource> collision;
};

}