#include "trmtr.h"

using namespace godot;

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
    //
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
    //GETTER_SETTER_BIND(MaterialEntry, FloatParams, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MaterialEntry")
    GETTER_SETTER_BIND(MaterialEntry, Unk5, Variant::STRING, PROPERTY_HINT_NONE)
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