#include "pe_SimpleNode.h"

using namespace godot;

void PeSimpleNode::_bind_methods()
{
    GETTER_SETTER_BIND(PeSimpleNode, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void PeSimpleNode::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetPeSimpleNode(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse PeSimpleNode");
    set_Name(Utils::toGodotString(component->name()));
}
