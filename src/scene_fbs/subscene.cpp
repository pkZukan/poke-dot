#include "subscene.h"

using namespace godot;

void TRSubScene::_bind_methods()
{
    GETTER_SETTER_BIND(TRSubScene, FileName, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TRSubScene, unk1, Variant::INT, PROPERTY_HINT_NONE)
}

void TRSubScene::LoadFromBuffer(const void* data)
{
    auto subscene = Titan::TrinityScene::GetSubScene(data);
    ERR_FAIL_COND_MSG(subscene == nullptr, "Couldn't parse subscene");

    set_FileName(Utils::toGodotString(subscene->filename()));
    set_unk1(subscene->unk_1());
}
