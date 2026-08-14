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
        map["SubScene"]                = &CreateFromData<TRSubScene>;
        map["trinity_SceneObject"]     = &CreateFromData<TrinitySceneObject>;
        map["trinity_ObjectTemplate"]   = &CreateFromData<TrinityObjectTemplate>;
        map["trinity_ScenePoint"]      = &CreateFromData<TrinityScenePoint>;
        map["trinity_ModelComponent"] = &CreateFromData<TrinityModelComponent>;
        return map;
    }();

    if (const FactoryFunc* func = factories.getptr(type)) 
    {
        return (*func)(data);
    }

    return Ref<Resource>();
}