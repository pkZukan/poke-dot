#include "trinity_ScenePoint.h"

using namespace godot;

void TrinityScenePoint::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityScenePoint, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityScenePoint, Point, Variant::VECTOR3, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityScenePoint, AttachParent, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TrinityScenePoint::LoadFromBuffer(const void* buffer)
{
    auto scene_point = Titan::TrinityScene::GetScenePoint(buffer);
    set_Name(Utils::toGodotString(scene_point->name()));
    set_Point(Vector3(scene_point->point()->x(), scene_point->point()->y(), scene_point->point()->z()));
    set_AttachParent(scene_point->attach_parent());
}