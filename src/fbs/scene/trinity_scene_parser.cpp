#include "trinity_scene_parser.h"

using namespace godot;

void TrinitySceneParser::_bind_methods()
{
    //TODO
}

Ref<Resource> TrinitySceneParser::FromData(String type, const void* data)
{
    using FactoryFunc = Ref<Resource>(*)(const void*);
    
    // Initialize once using a lambda
    static const HashMap<String, FactoryFunc> factories = []() {
        HashMap<String, FactoryFunc> map;
        map["SubScene"] = &CreateFromData<TRSubScene>;
        map["trinity_SceneObject"] = &CreateFromData<TrinitySceneObject>;
        map["trinity_ObjectTemplate"] = &CreateFromData<TrinityObjectTemplate>;
        map["trinity_ScenePoint"] = &CreateFromData<TrinityScenePoint>;
        map["trinity_ModelComponent"] = &CreateFromData<TrinityModelComponent>;
        map["trinity_ModelInstancerComponent"] = &CreateFromData<TrinityModelInstancerComponent>;
        map["trinity_AnimationComponent"] = &CreateFromData<TrinityAnimationComponent>;
        map["trinity_CollisionComponent"] = &CreateFromData<TrinityCollisionComponent>;
        map["trinity_PlacementRegistry"] = &CreateFromData<TrinityPlacementRegistry>;
        map["pe_FlatbuffersDataComponent"] = &CreateFromData<PeFlatbuffersDataComponent>;
        map["pe_InputEventTriggerComponent"] = &CreateFromData<PeInputEventTriggerComponent>;
        map["pe_SimpleNode"] = &CreateFromData<PeSimpleNode>;
        map["pe_TextComponent"] = &CreateFromData<PeTextComponent>;
        map["pe_UikitManagerComponent"] = &CreateFromData<PeUikitManagerComponent>;
        map["pe_UikitViewComponent"] = &CreateFromData<PeUikitViewComponent>;
        map["ti_AIPerceptualComponent"] = &CreateFromData<TiAIPerceptualComponent>;
        map["trinity_CameraEntity"] = &CreateFromData<TrinityCameraEntity>;
        map["trinity_CharacterCreationComponent"] = &CreateFromData<TrinityCharacterCreationComponent>;
        map["trinity_CharacterCreationMasterComponent"] = &CreateFromData<TrinityCharacterCreationMasterComponent>;
        map["trinity_EnvironmentParameter"] = &CreateFromData<TrinityEnvironmentParameter>;
        map["trinity_GluePlugin"] = &CreateFromData<TrinityGluePlugin>;
        map["trinity_GrassCollisionComponent"] = &CreateFromData<TrinityGrassCollisionComponent>;
        map["trinity_GroundPlaceComponent"] = &CreateFromData<TrinityGroundPlaceComponent>;
        map["trinity_LayoutCommonResourceComponent"] = &CreateFromData<TrinityLayoutCommonResourceComponent>;
        map["trinity_LayoutComponent"] = &CreateFromData<TrinityLayoutComponent>;
        map["trinity_LightApplierComponent"] = &CreateFromData<TrinityLightApplierComponent>;
        map["trinity_LightDirectApplierComponent"] = &CreateFromData<TrinityLightDirectApplierComponent>;
        map["trinity_OverrideSensorData"] = &CreateFromData<TrinityOverrideSensorData>;
        map["trinity_ParticleComponent"] = &CreateFromData<TrinityParticleComponent>;
        map["trinity_PropertySheet"] = &CreateFromData<TrinityPropertySheet>;
        map["trinity_ScriptComponent"] = &CreateFromData<TrinityScriptComponent>;
        map["trinity_StreamingPoint"] = &CreateFromData<TrinityStreamingPoint>;
        map["trinity_TerrainStreamingSetting"] = &CreateFromData<TrinityTerrainStreamingSetting>;
        map["trinity_TextureBufferComponent"] = &CreateFromData<TrinityTextureBufferComponent>;
        return map;
    }();

    if (const FactoryFunc* func = factories.getptr(type)) 
    {
        return (*func)(data);
    }

    return Ref<Resource>();
}
