#include "subscene.h"

using namespace godot;

void TRSubScene::_bind_methods()
{
    GETTER_SETTER_BIND(TRSubScene, FileName, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TRSubScene, group, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSubScene, subdata, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "byte")
}

void TRSubScene::LoadFromBuffer(const void* data)
{
    auto subscene = Titan::TrinityScene::GetSubScene(data);
    ERR_FAIL_COND_MSG(subscene == nullptr, "Couldn't parse subscene");

    set_FileName(Utils::toGodotString(subscene->filename()));
    set_group(Utils::toGodotString(subscene->group()));

    auto* fb_data = subscene->data();
    if (fb_data)
    {
        subdata.resize(fb_data->size());
        memcpy(subdata.ptrw(), fb_data->data(), fb_data->size());
    }
    else
    {
        subdata.clear();
    }

    set_subdata(subdata);
}
