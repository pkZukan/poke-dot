#include "trscn.h"

using namespace godot;

void TRScene::_bind_methods()
{
    GETTER_SETTER_BIND(TRScene, Name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TRScene, nested_type, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "Resource")
	GETTER_SETTER_BIND(TRScene, sub_objects, Variant::ARRAY, PROPERTY_HINT_NONE)
}

void TRSCN::_bind_methods()
{
    GETTER_SETTER_BIND(TRSCN, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSCN, Extra, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSCN, res_2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSCN, res_3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRSCN, chunks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRScene")
    GETTER_SETTER_BIND(TRSCN, res_4, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(TRSCN, unk_6, Variant::INT, PROPERTY_HINT_NONE)
}

Ref<Resource> TRScene::ParseData(String type, const void* data)
{
    if( type == "SubScene" )
    {
        Ref<TRSubScene> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else if( type == "trinity_SceneObject" )
    {
        Ref<TrinitySceneObject> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else if( type == "trinity_ObjectTemplate")
    {
        Ref<TrinityObjectTemplate> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else if( type == "trinity_ScenePoint")
    {
        Ref<TrinityScenePoint> res;
        res.instantiate();
        res->LoadFromBuffer(data);
        return res;
    }
    else return Ref<Resource>();
} 

void TRSCN::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load SCN file: %s", file));
    auto trscn = Titan::TrinityScene::GetTRSCN(buf.ptr());
    ERR_FAIL_COND_MSG(trscn == nullptr, "Couldn't parse SCN");

    set_Name(Utils::toGodotString(trscn->scene_name()));
    set_Extra(Utils::toGodotString(trscn->scene_extra()));
    set_res_2(trscn->res_2());
    set_res_3(trscn->res_3());

    auto res4 = trscn->res_4();
    if(res4)
    {
        for(int i = 0; i < res4->size(); i++)
            res_4.push_back(res4->Get(i));
    }
    set_res_4(res_4);

    set_unk_6(trscn->unk_6());


    for (int i = 0; i < trscn->chunks()->size(); i++) {
        auto chunk = trscn->chunks()->Get(i);
        Ref<TRScene> trscene;
        trscene.instantiate();

        String typeName = Utils::toGodotString(chunk->type_name());
        trscene->set_Name(typeName);
        
        if (auto data = chunk->nested_type()) 
        {
            auto parsedData = trscene->ParseData(typeName, data->data());
            trscene->set_nested_type(parsedData);
        }

        Array subObjects;
		for (int j = 0; j < chunk->sub_objects()->size(); j++) {
            Ref<Resource> res;
            res.instantiate();

			//TODO

            subObjects.push_back(res);
		}
        trscene->set_sub_objects(subObjects);

        chunks.push_back(trscene);
    }
    set_chunks(chunks);
}

Variant ResourceFormatLoaderTRSCN::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRSCN> trscn;
    trscn.instantiate();
    trscn->LoadFromFile(p_path);
    return trscn;
}

PackedStringArray ResourceFormatLoaderTRSCN::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trscn");

    return exts;
}

bool ResourceFormatLoaderTRSCN::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRSCN");
}