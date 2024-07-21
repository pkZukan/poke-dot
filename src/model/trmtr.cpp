#include "trmtr.h"

using namespace godot;

void ByteExtra::_bind_methods() 
{
    //
}

void IntExtra::_bind_methods() 
{
    //
}

void FloatParameter::_bind_methods() 
{
    //
}

void Float4Parameter::_bind_methods() 
{
    //
}

void IntParameter::_bind_methods() 
{
    //
}

void ShaderIntParam::_bind_methods() 
{
    GETTER_SETTER_BIND(ShaderIntParam, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ShaderIntParam, Value, Variant::INT, PROPERTY_HINT_NONE)
}

void ShaderStringParam::_bind_methods() 
{
    GETTER_SETTER_BIND(ShaderStringParam, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ShaderStringParam, Value, Variant::STRING, PROPERTY_HINT_NONE)
}

void SamplerEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_1, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_4, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_5, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_6, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_7, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_8, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, RepeatU, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, RepeatV, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, RepeatW, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, BorderColor, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "RGBA")
}

void TextureEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(TextureEntry, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TextureEntry, File, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TextureEntry, Slot, Variant::INT, PROPERTY_HINT_NONE)
}

void ShaderEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(ShaderEntry, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ShaderEntry, StringParams, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "ShaderStringParam")
}

void MaterialEntry::_bind_methods() 
{
    GETTER_SETTER_BIND(MaterialEntry, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialEntry, Shaders, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "ShaderEntry")
    GETTER_SETTER_BIND(MaterialEntry, Textures, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TextureEntry")
    GETTER_SETTER_BIND(MaterialEntry, Samplers, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "SamplerEntry")
    GETTER_SETTER_BIND(MaterialEntry, FloatParams, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "FloatParameter")
    GETTER_SETTER_BIND(MaterialEntry, unk_5, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialEntry, FloatLightParams, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Float4Parameter")
    GETTER_SETTER_BIND(MaterialEntry, Float4Params, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Float4Parameter")
    GETTER_SETTER_BIND(MaterialEntry, unk_8, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialEntry, IntParams, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "IntParameter")
    GETTER_SETTER_BIND(MaterialEntry, unk_10, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialEntry, unk_11, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialEntry, unk_12, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialEntry, ByteExt, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "ByteExtra")
    GETTER_SETTER_BIND(MaterialEntry, IntExt, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "IntExtra")
    GETTER_SETTER_BIND(MaterialEntry, AlphaType, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRMaterial::_bind_methods() 
{
    GETTER_SETTER_BIND(TRMaterial, Materials, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MaterialEntry")
}

void TRMaterial::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMTR file: %s", file));
    
    auto mat = Titan::Model::GetTRMTR(buf.ptr());
    ERR_FAIL_COND_MSG(mat == NULL, vformat("Couldn't load TRMTR flatbuffer"));

    auto mats = mat->materials();
    for(int i = 0; i < mats->size(); i++)
    {
        //Parse shaders
        auto shaders = mats->Get(i)->shaders();
        Array shdrs;
        for(int j = 0; j < shaders->size(); j++)
        {
            auto vals = shaders->Get(j)->shader_values();
            Array values;
            for(int k = 0; k < shaders->size(); k++)
            {
                Ref<ShaderStringParam> ssp;
                ssp.instantiate();
                ssp->set_name(Utils::toGodotString(vals->Get(k)->string_name()));
                values.push_back(ssp);
            }
            Ref<ShaderEntry> shader;
            shader.instantiate();
            shader->set_name(Utils::toGodotString(shaders->Get(j)->shader_name()));
            shader->set_StringParams(values);
            shdrs.push_back(shader);
        }

        //Parse textures
        auto textures = mats->Get(i)->textures();
        Array texs;
        for(int j = 0; j < shaders->size(); j++)
        {
            Ref<TextureEntry> texEnt;
            texEnt.instantiate();
            texEnt->set_name(Utils::toGodotString(textures->Get(j)->texture_name()));
            texEnt->set_File(Utils::toGodotString(textures->Get(j)->texture_file()));
            texEnt->set_Slot(textures->Get(j)->texture_slot());
            texs.push_back(texEnt);
        }

        //Parse samplers
        auto samplers = mats->Get(i)->samplers();
        Array samps;
        for(int j = 0; j < samplers->size(); j++)
        {
            Ref<SamplerEntry> sampEnt;
            sampEnt.instantiate();
            sampEnt->set_SamplerState_0(samplers->Get(j)->SamplerState_0());
            sampEnt->set_SamplerState_1(samplers->Get(j)->SamplerState_1());
            sampEnt->set_SamplerState_2(samplers->Get(j)->SamplerState_2());
            sampEnt->set_SamplerState_3(samplers->Get(j)->SamplerState_3());
            sampEnt->set_SamplerState_4(samplers->Get(j)->SamplerState_4());
            sampEnt->set_SamplerState_5(samplers->Get(j)->SamplerState_5());
            sampEnt->set_SamplerState_6(samplers->Get(j)->SamplerState_6());
            sampEnt->set_SamplerState_7(samplers->Get(j)->SamplerState_7());
            sampEnt->set_SamplerState_8(samplers->Get(j)->SamplerState_8());
            sampEnt->set_RepeatU(String(Titan::Model::EnumNameUVWrapMode(samplers->Get(j)->repeat_u())));
            sampEnt->set_RepeatV(String(Titan::Model::EnumNameUVWrapMode(samplers->Get(j)->repeat_v())));
            sampEnt->set_RepeatW(String(Titan::Model::EnumNameUVWrapMode(samplers->Get(j)->repeat_w())));
            Ref<RGBA> bc;
            bc.instantiate();
            bc->set_R(samplers->Get(j)->border_color()->r());
            bc->set_G(samplers->Get(j)->border_color()->g());
            bc->set_B(samplers->Get(j)->border_color()->b());
            bc->set_A(samplers->Get(j)->border_color()->a());
            sampEnt->set_BorderColor(bc);
            samps.push_back(sampEnt);
        }

        Ref<MaterialEntry> matEnt;
        matEnt.instantiate();
        matEnt->set_name(Utils::toGodotString(mats->Get(i)->name()));
        matEnt->set_Shaders(shdrs);
        matEnt->set_Textures(texs);
        Materials.push_back(matEnt);
    }
}

Variant ResourceFormatLoaderTRMTR::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRMaterial> trmtr;
    trmtr.instantiate();
    trmtr->LoadFromFile(p_path);
    return trmtr;
}

PackedStringArray ResourceFormatLoaderTRMTR::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trmtr");

    return exts;
}

bool ResourceFormatLoaderTRMTR::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRMaterial");
}