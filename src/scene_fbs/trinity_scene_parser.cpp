#include "trinity_scene_parser.h"

using namespace godot;

void TrinitySceneParser::_bind_methods()
{
    //TODO
}

Ref<Resource> TrinitySceneParser::FromData(String type, const void* data)
{
    if( type == "SubScene" )
    {
        Ref<TRSubScene> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else if( type == "trinity_SceneObject" )
    {
        Ref<TrinitySceneObject> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else if( type == "trinity_ObjectTemplate")
    {
        Ref<TrinityObjectTemplate> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else if( type == "trinity_ScenePoint")
    {
        Ref<TrinityScenePoint> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else return Ref<Resource>();
} 