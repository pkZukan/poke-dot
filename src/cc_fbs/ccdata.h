#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include "generated/ccdata_generated.h"
#include <utils.h>

namespace godot {

class TrinityCCDataSomeTable2 : public Resource {
	GDCLASS(TrinityCCDataSomeTable2, Resource)
protected:
	static void _bind_methods();
public:
	GETTER_SETTER_DEFINE(String, unk0)
private:
	String unk0;
};

class TrinityCCDataEntry : public Resource {
	GDCLASS(TrinityCCDataEntry, Resource)
protected:
	static void _bind_methods();
public:
	GETTER_SETTER_DEFINE(String, name)
	GETTER_SETTER_DEFINE(bool, is_root)
	GETTER_SETTER_DEFINE(String, model_file)
	GETTER_SETTER_DEFINE(String, material_file)
	GETTER_SETTER_DEFINE(String, anim_file)
	GETTER_SETTER_DEFINE(String, unk3)
	GETTER_SETTER_DEFINE(PackedStringArray, anim_file_list)
	GETTER_SETTER_DEFINE(bool, unk4)
	GETTER_SETTER_DEFINE(PackedStringArray, shared_file_list)
	GETTER_SETTER_DEFINE(PackedStringArray, unk5)
private:
	String name;
	bool is_root = false;
	String model_file;
	String material_file;
	String anim_file;
	String unk3;
	PackedStringArray anim_file_list;
	bool unk4 = false;
	PackedStringArray shared_file_list;
	PackedStringArray unk5;
};

class TrinityCCDataEntry4 : public Resource {
	GDCLASS(TrinityCCDataEntry4, Resource)
protected:
	static void _bind_methods();
public:
	GETTER_SETTER_DEFINE(String, type)
	GETTER_SETTER_DEFINE(uint32_t, enable)
private:
	String type;
	uint32_t enable = 0;
};

class TrinityCCDataEntry3 : public Resource {
	GDCLASS(TrinityCCDataEntry3, Resource)
protected:
	static void _bind_methods();
public:
	GETTER_SETTER_DEFINE(String, name)
	GETTER_SETTER_DEFINE(String, type)
	GETTER_SETTER_DEFINE(Array, unk0)
private:
	String name;
	String type;
	Array unk0;
};

class TrinityCCDataEntry2 : public Resource {
	GDCLASS(TrinityCCDataEntry2, Resource)
protected:
	static void _bind_methods();
public:
	GETTER_SETTER_DEFINE(String, name)
	GETTER_SETTER_DEFINE(Array, entries)
private:
	String name;
	Array entries;
};

class TrinityCharacterCreationData : public Resource {
	GDCLASS(TrinityCharacterCreationData, Resource)
protected:
	static void _bind_methods();
public:
	void LoadFromFile(String file);
	GETTER_SETTER_DEFINE(Array, entries)
	GETTER_SETTER_DEFINE(Ref<TrinityCCDataSomeTable2>, unk1)
	GETTER_SETTER_DEFINE(uint32_t, unk2)
	GETTER_SETTER_DEFINE(Array, unk3)
private:
	Array entries;
	Ref<TrinityCCDataSomeTable2> unk1;
	uint32_t unk2 = 0;
	Array unk3;

    PackedStringArray read_string_vector(const flatbuffers::Vector<flatbuffers::Offset<flatbuffers::String>> *values);
    Ref<TrinityCCDataEntry4> read_entry4(const ::Entry4 *source);
    Ref<TrinityCCDataEntry3> read_entry3(const ::Entry3 *source);
    Ref<TrinityCCDataEntry2> read_entry2(const ::Entry2 *source);
    Ref<TrinityCCDataEntry> read_entry(const ::Entry *source);
};

class ResourceFormatLoaderCCDATA : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderCCDATA, ResourceFormatLoader)
protected:
	static void _bind_methods() {}
public:
    ResourceFormatLoaderCCDATA(){}
    ~ResourceFormatLoaderCCDATA(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

} // namespace godot
