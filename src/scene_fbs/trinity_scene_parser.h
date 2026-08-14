#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include "scene_fbs/subscene.h"
#include "scene_fbs/components/trinity_sceneObject.h"
#include "scene_fbs/components/trinity_ScenePoint.h"
#include "scene_fbs/components/trinity_ObjectTemplate.h"
#include "scene_fbs/components/trinity_ModelComponent.h"
#include "scene_fbs/components/trinity_AnimationComponent.h"
#include "scene_fbs/components/trinity_CollisionComponent.h"
#include "utils.h"

namespace godot {

class TrinitySceneParser : public Resource {
	GDCLASS(TrinitySceneParser, Resource)
protected:
	static void _bind_methods();
public:
	TrinitySceneParser(){}
	~TrinitySceneParser(){}

    template <typename T>
    static Ref<Resource> CreateFromData(const void* data) 
    {
        Ref<T> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }

    static Ref<Resource> FromData(String type, const void* data);
};

}