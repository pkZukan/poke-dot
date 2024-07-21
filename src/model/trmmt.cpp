#include "trmmt.h"

using namespace godot;

void MaterialProperty::_bind_methods()
{
    GETTER_SETTER_BIND(MaterialProperty, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void MaterialSwitch::_bind_methods()
{
    GETTER_SETTER_BIND(MaterialSwitch, Name, Variant::STRING, PROPERTY_HINT_NONE)
}

void MaterialTable::_bind_methods()
{
    GETTER_SETTER_BIND(MaterialTable, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialTable, MatNames, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialTable, MaterialSwitches, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MaterialSwitch")
    GETTER_SETTER_BIND(MaterialTable, MaterialProperties, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MaterialProperty")
}

void TRModelMaterialTable::_bind_methods() 
{
    GETTER_SETTER_BIND(TRModelMaterialTable, MaterialTables, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MaterialTable")
}

void TRModelMaterialTable::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMMT file: %s", file));
    
    auto matTable = Titan::Model::GetTRMMT(buf.ptr());
    ERR_FAIL_COND_MSG(matTable == NULL, vformat("Couldn't load TRMMT flatbuffer"));

    auto tables = matTable->material();
    for(int i = 0; i < tables->size(); i++)
    {
        auto table = tables->Get(i);

        Ref<MaterialTable> mt;
        mt.instantiate();

        //Get Names
        auto matNames = table->material_name();
        Array mNames;
        for(int j = 0; j < matNames->size(); j++)
            mNames.push_back(Utils::toGodotString(matNames->Get(j)));
        mt->set_MatNames(mNames);

        //Get switches
        auto matSwitches = table->material_switches();
        Array mSwitches;
        for(int j = 0; j < matSwitches->size(); j++)
        {
            auto mswitch = matSwitches->Get(j);
            Ref<MaterialSwitch> msw;
            msw.instantiate();
            msw->set_Name(Utils::toGodotString(mswitch->material_name()));

            mSwitches.push_back(msw);
        }
        mt->set_MaterialSwitches(mSwitches);    

        //Get properies
        auto matProps = table->material_switches();
        Array mProps;
        for(int j = 0; j < matProps->size(); j++)
        {
            auto mprop = matProps->Get(j);
            Ref<MaterialProperty> mp;
            mp.instantiate();
            mp->set_Name(Utils::toGodotString(mprop->material_name()));

            mProps.push_back(mp);
        }
        mt->set_MaterialProperties(mProps);

        mt->set_Name(Utils::toGodotString(table->name()));
        
        MaterialTables.push_back(mt);
    }
}

Variant ResourceFormatLoaderTRMMT::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRModelMaterialTable> trmmt;
    trmmt.instantiate();
    trmmt->LoadFromFile(p_path);
    return trmmt;
}

PackedStringArray ResourceFormatLoaderTRMMT::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trmmt");

    return exts;
}

bool ResourceFormatLoaderTRMMT::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRModelMaterialTable");
}