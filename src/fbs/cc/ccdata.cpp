#include "ccdata.h"

using namespace godot;

void TrinityCCDataSomeTable2::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityCCDataSomeTable2, unk0, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityCCDataEntry::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityCCDataEntry, name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, is_root, Variant::BOOL, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, model_file, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, material_file, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, anim_file, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, unk3, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, anim_file_list, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, unk4, Variant::BOOL, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, shared_file_list, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry, unk5, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
}

void TrinityCCDataEntry4::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityCCDataEntry4, type, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry4, enable, Variant::INT, PROPERTY_HINT_NONE)
}

void TrinityCCDataEntry3::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityCCDataEntry3, name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry3, type, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry3, unk0, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityCCDataEntry4")
}

void TrinityCCDataEntry2::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityCCDataEntry2, name, Variant::STRING, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCCDataEntry2, entries, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityCCDataEntry3")
}

void TrinityCharacterCreationData::_bind_methods()
{
	GETTER_SETTER_BIND(TrinityCharacterCreationData, entries, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityCCDataEntry")
	GETTER_SETTER_BIND(TrinityCharacterCreationData, unk1, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TrinityCCDataSomeTable2")
	GETTER_SETTER_BIND(TrinityCharacterCreationData, unk2, Variant::INT, PROPERTY_HINT_NONE)
	GETTER_SETTER_BIND(TrinityCharacterCreationData, unk3, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TrinityCCDataEntry2")
}


PackedStringArray TrinityCharacterCreationData::read_string_vector(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *values)
{
	PackedStringArray result;
	if (!values) return result;
	for (int i = 0; i < values->size(); ++i)
		result.push_back(Utils::toGodotString(values->Get(i)));

	return result;
}

Ref<TrinityCCDataEntry4> TrinityCharacterCreationData::read_entry4(const ::Entry4 *source)
{
	if (!source) return Ref<TrinityCCDataEntry4>();

	Ref<TrinityCCDataEntry4> result;
	result.instantiate();
	result->set_type(Utils::toGodotString(source->type()));
	result->set_enable(source->enable());

	return result;
}

Ref<TrinityCCDataEntry3> TrinityCharacterCreationData::read_entry3(const ::Entry3 *source)
{
	if (!source) return Ref<TrinityCCDataEntry3>();

	Ref<TrinityCCDataEntry3> result;
	result.instantiate();
	result->set_name(Utils::toGodotString(source->name()));
	result->set_type(Utils::toGodotString(source->type()));
	Array values;
	if (const auto *items = source->unk0()) 
    {
		for (int i = 0; i < items->size(); ++i) 
        {
			Ref<TrinityCCDataEntry4> item = read_entry4(items->Get(i));
			if (item.is_valid()) 
                values.push_back(item);
		}
	}
	result->set_unk0(values);
	return result;
}

Ref<TrinityCCDataEntry2> TrinityCharacterCreationData::read_entry2(const ::Entry2 *source)
{
	if (!source) return Ref<TrinityCCDataEntry2>();

	Ref<TrinityCCDataEntry2> result;
	result.instantiate();
	result->set_name(Utils::toGodotString(source->name()));
	Array values;
	if (const auto *items = source->entries()) 
    {
		for (int i = 0; i < items->size(); ++i) 
        {
			Ref<TrinityCCDataEntry3> item = read_entry3(items->Get(i));
			if (item.is_valid()) 
                values.push_back(item);
		}
	}
	result->set_entries(values);

	return result;
}

Ref<TrinityCCDataEntry> TrinityCharacterCreationData::read_entry(const ::Entry *source)
{
	if (!source) return Ref<TrinityCCDataEntry>();

	Ref<TrinityCCDataEntry> result;
	result.instantiate();
	result->set_name(Utils::toGodotString(source->name()));
	result->set_is_root(source->is_root());
	result->set_model_file(Utils::toGodotString(source->model_file()));
	result->set_material_file(Utils::toGodotString(source->material_file()));
	result->set_anim_file(Utils::toGodotString(source->anim_file()));
	result->set_unk3(Utils::toGodotString(source->unk3()));
	result->set_anim_file_list(read_string_vector(source->anim_file_list()));
	result->set_unk4(source->unk4());
	result->set_shared_file_list(read_string_vector(source->shared_file_list()));
	result->set_unk5(read_string_vector(source->unk5()));

	return result;
}

void TrinityCharacterCreationData::LoadFromFile(String file)
{
	PackedByteArray buffer = FileAccess::get_file_as_bytes(file);
	ERR_FAIL_COND_MSG(buffer.is_empty(), vformat("Couldn't load CCData file: %s", file));
	const ::CharacterCreationData *data = ::GetCharacterCreationData(buffer.ptr());
	ERR_FAIL_COND_MSG(data == nullptr, "Couldn't parse CCData file");

	Array entry_values;
	if (const auto *items = data->entries()) 
    {
		for (int i = 0; i < items->size(); ++i) 
        {
			Ref<TrinityCCDataEntry> item = read_entry(items->Get(i));
			if (item.is_valid()) 
                entry_values.push_back(item);
		}
	}
	set_entries(entry_values);

	Ref<TrinityCCDataSomeTable2> table;
	if (const auto *source = data->unk1()) 
    {
		table.instantiate();
		table->set_unk0(Utils::toGodotString(source->unk0()));
	}
	set_unk1(table);
	set_unk2(data->unk2());

	Array grouped_values;
	if (const auto *items = data->unk3()) 
    {
		for (int i = 0; i < items->size(); ++i) 
        {
			Ref<TrinityCCDataEntry2> item = read_entry2(items->Get(i));
			if (item.is_valid()) 
                grouped_values.push_back(item);
		}
	}
	set_unk3(grouped_values);
}

Variant ResourceFormatLoaderCCDATA::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
	Ref<TrinityCharacterCreationData> data;
	data.instantiate();
	data->LoadFromFile(p_path);
	return data;
}

PackedStringArray ResourceFormatLoaderCCDATA::_get_recognized_extensions() const
{
	PackedStringArray extensions;
	extensions.push_back("ccdata");
	return extensions;
}

bool ResourceFormatLoaderCCDATA::_handles_type(const StringName &p_type) const
{
	return p_type == String("CharacterCreationData");
}
