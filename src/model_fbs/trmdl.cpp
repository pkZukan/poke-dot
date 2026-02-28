#include "trmdl.h"

using namespace godot;

void LodIndex::_bind_methods() 
{
    GETTER_SETTER_BIND(LodIndex, unk0, Variant::INT, PROPERTY_HINT_NONE)
}

void LOD::_bind_methods() 
{
    GETTER_SETTER_BIND(LOD, Index, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "LodIndex")
    GETTER_SETTER_BIND(LOD, Type, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRModel::_bind_methods() 
{
    GETTER_SETTER_BIND(TRModel, Meshes, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRModel, Skeleton, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRModel, Materials, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRModel, LODs, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "LOD")
}

void TRModel::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMDL file: %s", file));

    auto model = Titan::Model::GetTRMDL(buf.ptr());
    ERR_FAIL_COND_MSG(model == NULL, vformat("Couldn't load TRMDL flatbuffer"));

    //Get Meshes
    auto meshes = model->meshes();
    for(int i = 0; i < meshes->size(); i++)
        Meshes.push_back(Utils::toGodotString(meshes->Get(i)->filename()));

    //Get Mats
    auto mats = model->materials();
    for(int i = 0; i < mats->size(); i++)
        Materials.push_back(Utils::toGodotString(mats->Get(i)));

    //Get LODs
    auto lods = model->lods();
    for(int i = 0; i < lods->size(); i++)
    {
        auto l = lods->Get(i);
        Ref<LOD> lod;
        lod.instantiate();
        lod->set_Type(Utils::toGodotString(l->lod_type()));
        auto lind = l->index();
        Array linds;
        for(int j = 0; j < lind->size(); j++)
        {
            Ref<LodIndex> li;
            li.instantiate();
            li->set_unk0(lind->Get(j)->unk0());
            linds.push_back(li);
        }
            
        lod->set_Index(linds);
    }

    Skeleton = Utils::toGodotString(model->skeleton()->filename());
}

Variant ResourceFormatLoaderTRMDL::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRModel> trmdl;
    trmdl.instantiate();
    trmdl->LoadFromFile(p_path);
    return trmdl;
}

PackedStringArray ResourceFormatLoaderTRMDL::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trmdl");

    return exts;
}

bool ResourceFormatLoaderTRMDL::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRModel");
}