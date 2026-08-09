#include "truiv.h"

using namespace godot;

void TRUIViewChunk::_bind_methods()
{
    GETTER_SETTER_BIND(TRUIViewChunk, Type, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TRUIViewChunk, Data, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "Resource")
	GETTER_SETTER_BIND(TRUIViewChunk, Children, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRUIViewChunk")
}

void TRUIV::_bind_methods()
{
    GETTER_SETTER_BIND(TRUIV, Chunks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRUIViewChunk")
}

Ref<Resource> TRUIViewChunk::ParseChunkData(String type, const void* data)
{
    if( type == "UikitGauge" )
    {
        Ref<UIKitGauge> uikitGauge;
        uikitGauge.instantiate();
        uikitGauge->LoadFromBuffer(data);
        return uikitGauge;
    }
    else if( type == "UikitBody" )
    {
        Ref<UIKitBody> uikitBody;
        uikitBody.instantiate();
        uikitBody->LoadFromBuffer(data);
        return uikitBody;
    }
    else if( type == "UikitSwitch" )
    {
        Ref<UIKitSwitch> uikitSwitch;
        uikitSwitch.instantiate();
        uikitSwitch->LoadFromBuffer(data);
        return uikitSwitch;
    }
    else if( type == "UikitShortcut" )
    {
        Ref<UIKitShortcut> uikitShortcut;
        uikitShortcut.instantiate();
        uikitShortcut->LoadFromBuffer(data);
        return uikitShortcut;
    }
    else if( type == "UikitButton" )
    {
        Ref<UIKitButton> uikitButton;
        uikitButton.instantiate();
        uikitButton->LoadFromBuffer(data);
        return uikitButton;
    }
    else return Ref<Resource>();
}

void TRUIV::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRUIV file: %s", file));
    auto truiv = Titan::TrinityUI::GetTRUIV(buf.ptr());
    ERR_FAIL_COND_MSG(truiv == nullptr, "Couldn't parse TRUIV");

    auto chunks = truiv->chunks();
    Array chunksArray;
    for(int i = 0; i < chunks->size(); i++)
    {
        auto chunk = chunks->Get(i);
        Ref<TRUIViewChunk> viewChunk;
        viewChunk.instantiate();

        String type = Utils::toGodotString(chunk->type());
        viewChunk->set_Type(type);

        if (auto data = chunk->data()) 
        {
            auto parsedData = viewChunk->ParseChunkData(type, data->data());
            viewChunk->set_Data(parsedData);
        }

        auto children = chunk->children();
        Array childrenArray;
        for(int j = 0; j < children->size(); j++)
            childrenArray.push_back(children->Get(j));
        viewChunk->set_Children(childrenArray);
        chunksArray.push_back(viewChunk);
    }
    set_Chunks(chunksArray);
}

Variant ResourceFormatLoaderTRUIV::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRUIV> truiv;
    truiv.instantiate();
    truiv->LoadFromFile(p_path);
    return truiv;
}

PackedStringArray ResourceFormatLoaderTRUIV::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("truiv");

    return exts;
}

bool ResourceFormatLoaderTRUIV::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRUIV");
}