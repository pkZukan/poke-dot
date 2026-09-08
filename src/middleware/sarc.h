#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/tree.hpp>
#include <godot_cpp/classes/tree_item.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include "utils.h"

namespace godot {

constexpr int SFAT_NODE_SIZE = 0x10;

struct SARCHeader {
	String magic;
	uint16_t header_size = 0;
	bool big_endian = false;
	uint32_t file_size = 0;
	uint32_t data_offset = 0;
	uint16_t version = 0;
 
	SARCHeader(Ref<StreamPeerBuffer> sp) {
		sp->seek(0);
		magic = sp->get_string(4);
 
		sp->seek(6);
		uint8_t bom0 = sp->get_u8();
		uint8_t bom1 = sp->get_u8();
		big_endian = (bom0 == 0xFE && bom1 == 0xFF);
		sp->set_big_endian(big_endian);
 
		sp->seek(4);
		header_size = sp->get_u16();
		sp->seek(8); 
		file_size = sp->get_u32();
		data_offset = sp->get_u32();
		version = sp->get_u16();
		sp->get_u16();
	}
};

struct SFATHeader {
	String magic;
	uint16_t header_size = 0;
	uint16_t node_count = 0;
	uint32_t hash_multiplier = 0;

	SFATHeader(Ref<StreamPeerBuffer> sp) {
		magic = sp->get_string(4);
		header_size = sp->get_u16();
		node_count = sp->get_u16();
		hash_multiplier = sp->get_u32();
	}
};
 
struct SFATNode {
	uint32_t name_hash = 0;
	uint8_t flags = 0;
	uint32_t name_offset = 0;
	uint32_t file_data_begin = 0;
	uint32_t file_data_end = 0;

    SFATNode(){}
	SFATNode(Ref<StreamPeerBuffer> sp) {
		name_hash = sp->get_u32();
		uint32_t flags_and_name_offset = sp->get_u32();
		flags = (uint8_t)(flags_and_name_offset >> 24);
		name_offset = flags_and_name_offset & 0xFFFFFF;
		file_data_begin = sp->get_u32();
		file_data_end = sp->get_u32();
	}
};
 
struct SFNTHeader {
	String magic;
	uint16_t header_size = 0;
 
	SFNTHeader(Ref<StreamPeerBuffer> sp) {
		magic = sp->get_string(4);
		header_size = sp->get_u16();
		sp->get_u16(); // reserved / unused
	}
};

class SeadArchive : public Resource {
    GDCLASS(SeadArchive, Resource)

protected:
	static void _bind_methods();
public:
	SeadArchive(){}
	~SeadArchive(){}

    void LoadFromFile(String file);
    PackedStringArray get_files() const;
    bool has_file(const String &p_name) const;
    int get_file_size(const String &p_name) const;
    int get_file_offset(const String &p_name) const;
    PackedByteArray get_file_data(const String &p_name) const;

private:
    String read_cstring(const PackedByteArray &data, int offset);
    Dictionary get_file_list() const;

    struct FileNode {
		String name;
		uint32_t data_begin = 0;
		uint32_t data_end = 0;
	};
 
	PackedByteArray m_data;
	uint32_t m_data_offset = 0;
	bool m_big_endian = false;
	HashMap<String, FileNode> m_files;

};

class SarcInspectorControl : public VBoxContainer {
    GDCLASS(SarcInspectorControl, VBoxContainer)

private:
    Tree *file_tree = nullptr;
    void _on_item_mouse_selected(Vector2 position, MouseButton mouse_button);

protected:
    static void _bind_methods();

public:
    void setup(Ref<SeadArchive> archive);
};

class EditorInspectorPluginSARC : public EditorInspectorPlugin {
    GDCLASS(EditorInspectorPluginSARC, EditorInspectorPlugin);

protected:
    static void _bind_methods() {}

public:
    bool _can_handle(Object *object) const override 
    {
        return Object::cast_to<SeadArchive>(object) != nullptr;
    }

    bool _parse_property(Object *object, Variant::Type type, const String &name, PropertyHint hint_type, const String &hint_string, BitField<PropertyUsageFlags> usage_flags, bool wide) override;
};

class SARCEditorPlugin : public EditorPlugin {
    GDCLASS(SARCEditorPlugin, EditorPlugin);

private:
    Ref<EditorInspectorPluginSARC> inspector_plugin;

protected:
    static void _bind_methods() {}

public:
    void _enter_tree() override 
    {
        inspector_plugin.instantiate();
        add_inspector_plugin(inspector_plugin);
    }

    void _exit_tree() override 
    {
        remove_inspector_plugin(inspector_plugin);
        inspector_plugin.unref();
    }
};

class ResourceFormatLoaderSARC : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderSARC, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderSARC(){}
	~ResourceFormatLoaderSARC(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
    virtual String _get_resource_type(const String &p_path) const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}