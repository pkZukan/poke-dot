#include "trins.h"

using namespace godot;

void TRINS::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &TRINS::LoadFromFile);
	GETTER_SETTER_BIND(TRINS, transforms, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Transform3D")
}

void TRINS::LoadFromFile(String file)
{
    transforms.clear();

    PackedByteArray bytes = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(bytes.is_empty(), vformat("Couldn't load TRINS file: %s", file));

    auto instances = Titan::TrinityScene::GetTRINS(bytes.ptr());
    ERR_FAIL_COND_MSG(instances->type_name()->str() != "SrtInstanceBuffer" || instances->version() != 1,
        vformat("Unsupported TRINS instance buffer: %s", file));

    auto data = instances->buffer()->data();
    ERR_FAIL_COND_MSG(uint64_t(instances->count()) * 64 != data->size(),
        vformat("TRINS matrix count does not match buffer size: %s", file));

    TypedArray<Transform3D> loaded_transforms;
    for (uint32_t i = 0; i < instances->count(); ++i) {
        float m[16];
        for (int j = 0; j < 16; ++j)
            m[j] = flatbuffers::ReadScalar<float>(data->data() + uint64_t(i) * 64 + j * 4);
        Transform3D transform(Basis(Vector3(m[0], m[1], m[2]),
            Vector3(m[4], m[5], m[6]), Vector3(m[8], m[9], m[10])),
            Vector3(m[12], m[13], m[14]));
        ERR_FAIL_COND_MSG(!transform.is_finite(),
            vformat("Non-finite TRINS transform: %s", file));
        loaded_transforms.append(transform);
    }
    set_transforms(loaded_transforms);
}

Variant ResourceFormatLoaderTRINS::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
	Ref<TRINS> trins;
	trins.instantiate();
	trins->LoadFromFile(p_path);
	return trins;
}

PackedStringArray ResourceFormatLoaderTRINS::_get_recognized_extensions() const
{
	PackedStringArray exts;
	exts.push_back("trins");
	return exts;
}

bool ResourceFormatLoaderTRINS::_handles_type(const StringName &p_type) const
{
	return p_type == StringName("TRINS");
}
