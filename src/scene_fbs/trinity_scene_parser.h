#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include "scene_fbs/subscene.h"
#include "scene_fbs/components/trinity_sceneObject.h"
#include "scene_fbs/components/trinity_ScenePoint.h"
#include "scene_fbs/components/trinity_ObjectTemplate.h"
#include "utils.h"

namespace godot {

class TrinitySceneParser : public Resource {
	GDCLASS(TrinitySceneParser, Resource)
protected:
	static void _bind_methods();
public:
	TrinitySceneParser(){}
	~TrinitySceneParser(){}

	static Ref<Resource> FromData(String type, const void* data);
};

}