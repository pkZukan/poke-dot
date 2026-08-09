#include "trinity_sceneObject.h"

using namespace godot;

void TrinityObjectLayer::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityObjectLayer, Priority, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityObjectLayer, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityObjectLayer, transform, Variant::TRANSFORM3D, PROPERTY_HINT_NONE)
}

void TrinitySceneObject::_bind_methods()
{
    GETTER_SETTER_BIND(TrinitySceneObject, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, transform, Variant::TRANSFORM3D, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, keep_world_srt, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, attach_xform, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, attach_joint, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, scriptable, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, Priority, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinitySceneObject, Layers, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityObjectLayer")
    GETTER_SETTER_BIND(TrinitySceneObject, TagList, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "String")
}

void TrinitySceneObject::LoadFromBuffer(const void* buffer)
{
    auto sceneObject = Titan::TrinityScene::GetTrinitySceneObject(buffer);
    ERR_FAIL_COND_MSG(sceneObject == nullptr, "Couldn't parse TrinitySceneObject");

    set_Name(Utils::toGodotString(sceneObject->name()));
    set_transform(Utils::toGodotTransform(sceneObject->srt()));
    set_keep_world_srt(sceneObject->keep_world_srt());
    set_attach_xform(sceneObject->attach_xform());
    set_attach_joint(Utils::toGodotString(sceneObject->attach_joint()));
    set_scriptable(sceneObject->scriptable());
    set_Priority(sceneObject->priority());
    Array layersArr;
    auto layers = sceneObject->layers();
    for (int i = 0; i < layers->size(); i++)
    {
        auto layer = layers->Get(i);
        Ref<TrinityObjectLayer> oblayer;
        oblayer.instantiate();
        oblayer->set_Priority(layer->priority());
        oblayer->set_Name(Utils::toGodotString(layer->name()));
        oblayer->set_transform(Utils::toGodotTransform(layer->srt()));
        layersArr.append(oblayer);
    }
    set_Layers(layersArr);
    Array tag_listArr;
    auto tag_list = sceneObject->tag_list();
    if (tag_list)
    {
        for (int i = 0; i < tag_list->size(); i++)
        {
            auto tag = tag_list->Get(i);
            tag_listArr.append(Utils::toGodotString(tag));
        }
    }
    set_TagList(tag_listArr);
}