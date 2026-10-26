#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trinity_PlacementRegistry_generated.h"
#include <utils.h>

namespace godot
{

class TrinityPlacementPosition : public Resource
{
    GDCLASS(TrinityPlacementPosition, Resource)
protected:
    static void _bind_methods();

public:
    TrinityPlacementPosition() = default;
    ~TrinityPlacementPosition() = default;

    GETTER_SETTER_DEFINE(String, ID)
    GETTER_SETTER_DEFINE(Vector3, position)
    GETTER_SETTER_DEFINE(Vector3, rotation)

private:
    String ID;
    Vector3 position;
    Vector3 rotation;
};

class TrinityPlacementObjects : public Resource
{
    GDCLASS(TrinityPlacementObjects, Resource)
protected:
    static void _bind_methods();

public:
    TrinityPlacementObjects() = default;
    ~TrinityPlacementObjects() = default;

private:
};

class TrinityPlacementObjectTemplates : public Resource
{
    GDCLASS(TrinityPlacementObjectTemplates, Resource)
protected:
    static void _bind_methods();

public:
    TrinityPlacementObjectTemplates() = default;
    ~TrinityPlacementObjectTemplates() = default;

private:
};

class TrinityPlacementSpawners : public Resource
{
    GDCLASS(TrinityPlacementSpawners, Resource)
protected:
    static void _bind_methods();

public:
    TrinityPlacementSpawners() = default;
    ~TrinityPlacementSpawners() = default;

private:
};

class TrinityPlacementRegistry : public Resource
{
    GDCLASS(TrinityPlacementRegistry, Resource)
protected:
    static void _bind_methods();

public:
    TrinityPlacementRegistry() = default;
    ~TrinityPlacementRegistry() = default;

    void LoadFromBuffer(const void* buffer);
    Ref<Resource> _LoadPlacementItem(Titan::TrinityScene::PlacementItem item, const void* data);

    GETTER_SETTER_DEFINE(Ref<Resource>, item)

private:
    Ref<Resource> item;
};

}
