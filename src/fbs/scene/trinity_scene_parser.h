#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include "fbs/scene/subscene.h"
#include "fbs/scene/components/trinity_SceneObject.h"
#include "fbs/scene/components/trinity_ScenePoint.h"
#include "fbs/scene/components/trinity_ObjectTemplate.h"
#include "fbs/scene/components/trinity_ModelComponent.h"
#include "fbs/scene/components/trinity_ModelInstancerComponent.h"
#include "fbs/scene/components/trinity_AnimationComponent.h"
#include "fbs/scene/components/trinity_CollisionComponent.h"
#include "fbs/scene/components/trinity_PlacementRegistry.h"
#include "fbs/scene/components/pe_FlatbuffersDataComponent.h"
#include "fbs/scene/components/pe_InputEventTriggerComponent.h"
#include "fbs/scene/components/pe_SimpleNode.h"
#include "fbs/scene/components/pe_TextComponent.h"
#include "fbs/scene/components/pe_UikitManagerComponent.h"
#include "fbs/scene/components/pe_UikitViewComponent.h"
#include "fbs/scene/components/ti_AIPerceptualComponent.h"
#include "fbs/scene/components/trinity_CameraEntity.h"
#include "fbs/scene/components/trinity_CharacterCreationComponent.h"
#include "fbs/scene/components/trinity_CharacterCreationMasterComponent.h"
#include "fbs/scene/components/trinity_EnvironmentParameter.h"
#include "fbs/scene/components/trinity_GluePlugin.h"
#include "fbs/scene/components/trinity_GrassCollisionComponent.h"
#include "fbs/scene/components/trinity_GroundPlaceComponent.h"
#include "fbs/scene/components/trinity_LayoutCommonResourceComponent.h"
#include "fbs/scene/components/trinity_LayoutComponent.h"
#include "fbs/scene/components/trinity_LightApplierComponent.h"
#include "fbs/scene/components/trinity_LightDirectApplierComponent.h"
#include "fbs/scene/components/trinity_OverrideSensorData.h"
#include "fbs/scene/components/trinity_ParticleComponent.h"
#include "fbs/scene/components/trinity_PropertySheet.h"
#include "fbs/scene/components/trinity_ScriptComponent.h"
#include "fbs/scene/components/trinity_StreamingPoint.h"
#include "fbs/scene/components/trinity_TerrainStreamingSetting.h"
#include "fbs/scene/components/trinity_TextureBufferComponent.h"
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
