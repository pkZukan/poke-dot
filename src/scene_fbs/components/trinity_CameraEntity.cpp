#include "trinity_CameraEntity.h"

using namespace godot;

void TrinityCameraEntity::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityCameraEntity, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, Activate, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, Position, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, Rotation, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, Distance, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, FovY, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, NearPlane, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, FarPlane, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, ProjectionType, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, CameraMode, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, TargetName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, UseRoll, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, Roll, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityCameraEntity, AttachTransform, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TrinityCameraEntity::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityCameraEntity(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityCameraEntity");
    set_Name(Utils::toGodotString(component->name()));
    set_Activate(component->activate());
    set_Position(component->position() ? Utils::toGodotVec3(component->position()) : Vector3());
    set_Rotation(component->rotation() ? Utils::toGodotVec3(component->rotation()) : Vector3());
    set_Distance(component->distance());
    set_FovY(component->fov_y());
    set_NearPlane(component->near_plane());
    set_FarPlane(component->far_plane());
    set_ProjectionType(component->projection_type());
    set_CameraMode(component->camera_mode());
    set_TargetName(Utils::toGodotString(component->target_name()));
    set_UseRoll(component->use_roll());
    set_Roll(component->roll());
    set_AttachTransform(component->attach_transform());
}
