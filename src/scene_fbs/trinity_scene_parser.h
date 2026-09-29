#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include "scene_fbs/subscene.h"
#include "scene_fbs/components/trinity_SceneObject.h"
#include "scene_fbs/components/trinity_ScenePoint.h"
#include "scene_fbs/components/trinity_ObjectTemplate.h"
#include "scene_fbs/components/trinity_ModelComponent.h"
#include "scene_fbs/components/trinity_ModelInstancerComponent.h"
#include "scene_fbs/components/trinity_AnimationComponent.h"
#include "scene_fbs/components/trinity_CollisionComponent.h"
#include "scene_fbs/components/trinity_PlacementRegistry.h"
#include "scene_fbs/components/pe_FlatbuffersDataComponent.h"
#include "scene_fbs/components/pe_InputEventTriggerComponent.h"
#include "scene_fbs/components/pe_SimpleNode.h"
#include "scene_fbs/components/pe_TextComponent.h"
#include "scene_fbs/components/pe_UikitManagerComponent.h"
#include "scene_fbs/components/pe_UikitViewComponent.h"
#include "scene_fbs/components/ti_AIPerceptualComponent.h"
#include "scene_fbs/components/trinity_CameraEntity.h"
#include "scene_fbs/components/trinity_CharacterCreationComponent.h"
#include "scene_fbs/components/trinity_CharacterCreationMasterComponent.h"
#include "scene_fbs/components/trinity_EnvironmentParameter.h"
#include "scene_fbs/components/trinity_GluePlugin.h"
#include "scene_fbs/components/trinity_GrassCollisionComponent.h"
#include "scene_fbs/components/trinity_GroundPlaceComponent.h"
#include "scene_fbs/components/trinity_LayoutCommonResourceComponent.h"
#include "scene_fbs/components/trinity_LayoutComponent.h"
#include "scene_fbs/components/trinity_LightApplierComponent.h"
#include "scene_fbs/components/trinity_LightDirectApplierComponent.h"
#include "scene_fbs/components/trinity_OverrideSensorData.h"
#include "scene_fbs/components/trinity_ParticleComponent.h"
#include "scene_fbs/components/trinity_PropertySheet.h"
#include "scene_fbs/components/trinity_ScriptComponent.h"
#include "scene_fbs/components/trinity_StreamingPoint.h"
#include "scene_fbs/components/trinity_TerrainStreamingSetting.h"
#include "scene_fbs/components/trinity_TextureBufferComponent.h"
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