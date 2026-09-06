#include "trinity_PlacementRegistry.h"

#define PLACEMENT_REG "TrinityPlacementPosition,TrinityPlacementObjects,TrinityPlacementObjectTemplates,TrinityPlacementSpawners"

using namespace godot;

void TrinityPlacementPosition::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityPlacementPosition, ID, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPlacementPosition, position, Variant::VECTOR3, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityPlacementPosition, rotation, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void TrinityPlacementObjects::_bind_methods()
{
    //
}

void TrinityPlacementObjectTemplates::_bind_methods()
{
    //
}

void TrinityPlacementSpawners::_bind_methods()
{
    //
}

void TrinityPlacementRegistry::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityPlacementRegistry, item, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, PLACEMENT_REG)
}

Ref<Resource> TrinityPlacementRegistry::_LoadPlacementItem(Titan::TrinityScene::PlacementItem item, const void* data)
{
    switch (item) {
        case Titan::TrinityScene::PlacementItem::PlacementItem_PlacementPositions: 
        {
            auto src = static_cast<const Titan::TrinityScene::PlacementPosition*>(data);
            Ref<TrinityPlacementPosition> t; 
            t.instantiate();
            t->set_ID(Utils::toGodotString(src->id()));
            return t;
        }
        case Titan::TrinityScene::PlacementItem::PlacementItem_PlacementObjects: 
        {
            auto src = static_cast<const Titan::TrinityScene::PlacementObjects*>(data);
            Ref<TrinityPlacementObjects> t; 
            t.instantiate();
            //
            return t;
        }
        case Titan::TrinityScene::PlacementItem::PlacementItem_PlacementObjectTemplates: 
        {
            auto src = static_cast<const Titan::TrinityScene::PlacementObjectTemplates*>(data);
            Ref<TrinityPlacementObjectTemplates> t; 
            t.instantiate();
            //
            return t;
        }
        case Titan::TrinityScene::PlacementItem::PlacementItem_PlacementSpawners: 
        {
            auto src = static_cast<const Titan::TrinityScene::PlacementSpawners*>(data);
            Ref<TrinityPlacementSpawners> t; 
            t.instantiate();
            //
            return t;
        }
        default: 
            return Ref<Resource>();
    }
}

void TrinityPlacementRegistry::LoadFromBuffer(const void* buffer)
{
    auto reg = Titan::TrinityScene::GetTrinityPlacementRegistry(buffer);

    Ref<Resource> res = _LoadPlacementItem(Titan::TrinityScene::PlacementItem::PlacementItem_PlacementPositions, reg->item());
    
    set_item(res);
}