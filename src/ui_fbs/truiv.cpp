#include "truiv.h"
#include <functional>

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
    else if( type == "UikitCursor" )
    {
        Ref<UIKitCursor> component;
        component.instantiate();
        component->LoadFromBuffer(data);
        return component;
    }
    else if( type == "UikitGridPanel" )
    {
        Ref<UIKitGridPanel> component;
        component.instantiate();
        component->LoadFromBuffer(data);
        return component;
    }
    else if( type == "UikitOptionGuide" )
    {
        Ref<UIKitOptionGuide> component;
        component.instantiate();
        component->LoadFromBuffer(data);
        return component;
    }
    else if( type == "UikitScrollPanel" )
    {
        Ref<UIKitScrollPanel> component;
        component.instantiate();
        component->LoadFromBuffer(data);
        return component;
    }
    else if( type == "UikitSwitchItem" )
    {
        Ref<UIKitSwitchItem> component;
        component.instantiate();
        component->LoadFromBuffer(data);
        return component;
    }
    else if( type == "UikitSwitchPanel" )
    {
        Ref<UIKitSwitchPanel> component;
        component.instantiate();
        component->LoadFromBuffer(data);
        return component;
    }
    else return Ref<Resource>();
}

void TRUIV::LoadFromFile(String file)
{
    Chunks.clear();

    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRUIV file: %s", file));

    flatbuffers::Verifier verifier(buf.ptr(), buf.size());
    ERR_FAIL_COND_MSG(!Titan::TrinityUI::VerifyTRUIVBuffer(verifier), "Invalid TRUIV flatbuffer");

    auto truiv = Titan::TrinityUI::GetTRUIV(buf.ptr());
    bool valid = true;

    //Verify and parse chunk
    std::function<Ref<TRUIViewChunk>(const Titan::TrinityUI::ViewChunk *)> parse_chunk;
    parse_chunk = [&](const Titan::TrinityUI::ViewChunk *chunk) -> Ref<TRUIViewChunk> {
        Ref<TRUIViewChunk> result;
        result.instantiate();
        String type = Utils::toGodotString(chunk->type());
        result->set_Type(type);
        if (auto data = chunk->data()) {
            flatbuffers::Verifier payload(data->data(), data->size());
            if (type == "UikitGauge") valid = valid && Titan::pe::UIKit::VerifyUIKitGaugeBuffer(payload);
            if (type == "UikitBody") valid = valid && Titan::pe::UIKit::VerifyUIKitBodyBuffer(payload);
            if (type == "UikitSwitch") valid = valid && Titan::pe::UIKit::VerifyUIKitSwitchBuffer(payload);
            if (type == "UikitShortcut") valid = valid && Titan::pe::UIKit::VerifyUIKitShortcutBuffer(payload);
            if (type == "UikitButton") valid = valid && Titan::pe::UIKit::VerifyUIKitButtonBuffer(payload);
            if (type == "UikitCursor") valid = valid && Titan::pe::UIKit::VerifyUIKitCursorBuffer(payload);
            if (type == "UikitGridPanel") valid = valid && Titan::pe::UIKit::VerifyUIKitGridPanelBuffer(payload);
            if (type == "UikitOptionGuide") valid = valid && Titan::pe::UIKit::VerifyUIKitOptionGuideBuffer(payload);
            if (type == "UikitScrollPanel") valid = valid && Titan::pe::UIKit::VerifyUIKitScrollPanelBuffer(payload);
            if (type == "UikitSwitchItem") valid = valid && Titan::pe::UIKit::VerifyUIKitSwitchItemBuffer(payload);
            if (type == "UikitSwitchPanel") valid = valid && Titan::pe::UIKit::VerifyUIKitSwitchPanelBuffer(payload);
            if (!valid) return Ref<TRUIViewChunk>();
            result->set_Data(result->ParseChunkData(type, data->data()));
        }

        //Parse children
        Array children;
        if (auto source = chunk->children()) {
            for (auto child : *source) {
                children.push_back(parse_chunk(child));
                if (!valid) return Ref<TRUIViewChunk>();
            }
        }
        result->set_Children(children);
        return result;
    };
    Array parsed;
    if (auto chunks = truiv->chunks()) {
        for (auto chunk : *chunks) {
            parsed.push_back(parse_chunk(chunk));
            ERR_FAIL_COND_MSG(!valid, "Invalid TRUIV component payload");
        }
    }
    set_Chunks(parsed);
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