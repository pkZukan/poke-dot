#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_CameraEntity_generated.h"
#include "utils.h"

namespace godot {

class TrinityCameraEntity : public Resource {
    GDCLASS(TrinityCameraEntity, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(bool, Activate)
    GETTER_SETTER_DEFINE(Vector3, Position)
    GETTER_SETTER_DEFINE(Vector3, Rotation)
    GETTER_SETTER_DEFINE(float, Distance)
    GETTER_SETTER_DEFINE(float, FovY)
    GETTER_SETTER_DEFINE(float, NearPlane)
    GETTER_SETTER_DEFINE(float, FarPlane)
    GETTER_SETTER_DEFINE(uint8_t, ProjectionType)
    GETTER_SETTER_DEFINE(uint8_t, CameraMode)
    GETTER_SETTER_DEFINE(String, TargetName)
    GETTER_SETTER_DEFINE(bool, UseRoll)
    GETTER_SETTER_DEFINE(float, Roll)
    GETTER_SETTER_DEFINE(bool, AttachTransform)
private:
    String Name;
    bool Activate = false;
    Vector3 Position;
    Vector3 Rotation;
    float Distance = 0.0f;
    float FovY = 0.0f;
    float NearPlane = 0.0f;
    float FarPlane = 0.0f;
    uint8_t ProjectionType = 0;
    uint8_t CameraMode = 0;
    String TargetName;
    bool UseRoll = false;
    float Roll = 0.0f;
    bool AttachTransform = false;
};

} // namespace godot
