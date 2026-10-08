#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_StreamingPoint_generated.h"
#include "utils.h"

namespace godot {

class TrinityStreamingObject : public Resource {
    GDCLASS(TrinityStreamingObject, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityStreamingObject* table);
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(PackedByteArray, NestedType)
private:
    String Name;
    PackedByteArray NestedType;
};

class TrinityStreamingPointData : public Resource {
    GDCLASS(TrinityStreamingPointData, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityStreamingPointData* table);
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Vector3, Position)
private:
    String Name;
    Vector3 Position;
};

class TrinityStreamingEntry : public Resource {
    GDCLASS(TrinityStreamingEntry, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromTable(const Titan::TrinityScene::TrinityStreamingEntry* table);
    GETTER_SETTER_DEFINE(Ref<TrinityStreamingPointData>, Point)
    GETTER_SETTER_DEFINE(Array, Objects)
private:
    Ref<TrinityStreamingPointData> Point;
    Array Objects;
};

class TrinityStreamingPoint : public Resource {
    GDCLASS(TrinityStreamingPoint, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(Array, Entries)
private:
    Array Entries;
};

} // namespace godot
