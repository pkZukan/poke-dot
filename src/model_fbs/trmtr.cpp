#include "trmtr.h"

using namespace godot;

void ByteExtra::_bind_methods() 
{
    GETTER_SETTER_BIND(ByteExtra, Value, Variant::INT, PROPERTY_HINT_NONE)
}

void IntExtra::_bind_methods() 
{
    GETTER_SETTER_BIND(IntExtra, res0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(IntExtra, Value, Variant::INT, PROPERTY_HINT_NONE)
}

void FloatParameter::_bind_methods() 
{
    GETTER_SETTER_BIND(FloatParameter, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(FloatParameter, Value, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void Float4Parameter::_bind_methods() 
{
    GETTER_SETTER_BIND(Float4Parameter, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Float4Parameter, Value, Variant::VECTOR4, PROPERTY_HINT_NONE)
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
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_4, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_5, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_6, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_7, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, SamplerState_8, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, RepeatU, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, RepeatV, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, RepeatW, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(SamplerEntry, BorderColor, Variant::COLOR, PROPERTY_HINT_NONE)
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
        Ref<MaterialEntry> matEnt;
        matEnt.instantiate();
        matEnt->set_Name(Utils::toGodotString(mats->Get(i)->name()));

        //Parse shaders
        auto shaders = mats->Get(i)->shaders();
        Array shdrs;
        for(int j = 0; j < shaders->size(); j++)
        {
            Ref<ShaderEntry> shader;
            shader.instantiate();
            shader->set_Name(Utils::toGodotString(shaders->Get(j)->shader_name()));

            auto vals = shaders->Get(j)->shader_values();
            Array values;
            for(int k = 0; k < vals->size(); k++)
            {
                Ref<ShaderStringParam> ssp;
                ssp.instantiate();
                ssp->set_Name(Utils::toGodotString(vals->Get(k)->string_name()));
                ssp->set_Value(Utils::toGodotString(vals->Get(k)->string_value()));
                values.push_back(ssp);
            }
            shader->set_StringParams(values);

            shdrs.push_back(shader);
        }
        matEnt->set_Shaders(shdrs);

        //Parse textures
        auto textures = mats->Get(i)->textures();
        Array texs;
        for(int j = 0; j < textures->size(); j++)
        {
            Ref<TextureEntry> texEnt;
            texEnt.instantiate();
            texEnt->set_Name(Utils::toGodotString(textures->Get(j)->texture_name()));
            texEnt->set_File(Utils::toGodotString(textures->Get(j)->texture_file()));
            texEnt->set_Slot(textures->Get(j)->texture_slot());
            texs.push_back(texEnt);
        }
        matEnt->set_Textures(texs);

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
            sampEnt->set_RepeatU(samplers->Get(j)->repeat_u());
            sampEnt->set_RepeatV(samplers->Get(j)->repeat_v());
            sampEnt->set_RepeatW(samplers->Get(j)->repeat_w());
            Color bc;
            bc.r = samplers->Get(j)->border_color()->r();
            bc.g = samplers->Get(j)->border_color()->g();
            bc.b = samplers->Get(j)->border_color()->b();
            bc.a = samplers->Get(j)->border_color()->a();
            sampEnt->set_BorderColor(bc);
            samps.push_back(sampEnt);
        }
        matEnt->set_Samplers(samps);

        //Parse floats
        auto floatParams = mats->Get(i)->float_parameter();
        Array floats;
        for(int j = 0; j < floatParams->size(); j++)
        {
            Ref<FloatParameter> fp;
            fp.instantiate();
            fp->set_Name(Utils::toGodotString(floatParams->Get(j)->float_name()));
            fp->set_Value(floatParams->Get(j)->float_value());
            floats.push_back(fp);
        }
        matEnt->set_FloatParams(floats);

        //Parse float4s
        auto float4Params = mats->Get(i)->float4_parameter();
        Array float4s;
        for(int j = 0; j < float4Params->size(); j++)
        {
            Ref<Float4Parameter> fp;
            fp.instantiate();
            fp->set_Name(Utils::toGodotString(float4Params->Get(j)->color_name()));

            Vector4 col;
            col.x = float4Params->Get(j)->color_value()->r();
            col.y = float4Params->Get(j)->color_value()->g();
            col.z = float4Params->Get(j)->color_value()->b();
            col.w = float4Params->Get(j)->color_value()->a();

            fp->set_Value(col);
            float4s.push_back(fp);
        }
        matEnt->set_Float4Params(float4s);

        //Parse light floats
        auto lightParams = mats->Get(i)->float4_light_parameter();
        Array lights;
        for(int j = 0; j < lightParams->size(); j++)
        {
            Ref<Float4Parameter> fp;
            fp.instantiate();
            fp->set_Name(Utils::toGodotString(lightParams->Get(j)->color_name()));

            Vector4 col;
            col.x = lightParams->Get(j)->color_value()->r();
            col.y = lightParams->Get(j)->color_value()->g();
            col.z = lightParams->Get(j)->color_value()->b();
            col.w = lightParams->Get(j)->color_value()->a();

            fp->set_Value(col);
            lights.push_back(fp);
        }
        matEnt->set_FloatLightParams(lights);

        //Parse ints
        auto intParams = mats->Get(i)->int_parameter();
        Array ints;
        for(int j = 0; j < intParams->size(); j++)
        {
            Ref<ShaderIntParam> ip;
            ip.instantiate();
            ip->set_Name(Utils::toGodotString(intParams->Get(j)->int_name()));
            ip->set_Value(intParams->Get(j)->int_value());
            ints.push_back(ip);
        }
        matEnt->set_IntParams(ints);

        //Set extras
        Ref<ByteExtra> be;
        be.instantiate();
        be->set_Value(mats->Get(i)->byte_extra()->value());
        matEnt->set_ByteExt(be);

        Ref<IntExtra> ie;
        ie.instantiate();
        ie->set_res0(mats->Get(i)->int_extra()->res_0());
        ie->set_Value(mats->Get(i)->int_extra()->value());
        matEnt->set_IntExt(ie);

        matEnt->set_unk_5(Utils::toGodotString(mats->Get(i)->unk_5()));
        matEnt->set_unk_8(Utils::toGodotString(mats->Get(i)->unk_8()));
        matEnt->set_unk_10(Utils::toGodotString(mats->Get(i)->unk_10()));
        matEnt->set_unk_11(Utils::toGodotString(mats->Get(i)->unk_11()));
        matEnt->set_unk_12(Utils::toGodotString(mats->Get(i)->unk_12()));
        matEnt->set_AlphaType(Utils::toGodotString(mats->Get(i)->alpha_type()));
        
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