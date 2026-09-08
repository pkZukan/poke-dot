#include "sarc.h"

using namespace godot;

void SarcInspectorControl::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("_on_item_mouse_selected", "position", "mouse_button"), &SarcInspectorControl::_on_item_mouse_selected);
    ClassDB::bind_method(D_METHOD("_on_item_activated"), &SarcInspectorControl::_on_item_activated);
}

void SeadArchive::_bind_methods() 
{
    ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &SeadArchive::LoadFromFile);
    ClassDB::bind_method(D_METHOD("get_files"), &SeadArchive::get_files);
    ClassDB::bind_method(D_METHOD("has_file", "name"), &SeadArchive::has_file);
    ClassDB::bind_method(D_METHOD("get_file_size", "name"), &SeadArchive::get_file_size);
    ClassDB::bind_method(D_METHOD("get_file_offset", "name"), &SeadArchive::get_file_offset);
    ClassDB::bind_method(D_METHOD("get_file_data", "name"), &SeadArchive::get_file_data);
    ClassDB::bind_method(D_METHOD("get_file_list"), &SeadArchive::get_file_list);
    
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "file_list", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_file_list");
}

void SarcInspectorControl::_on_item_mouse_selected(Vector2 position, MouseButton mouse_button) 
{
    if (mouse_button != MOUSE_BUTTON_LEFT) 
        return;

    TreeItem *item = file_tree->get_selected();

    if (!item)
        return;

    // Ignore directories/root.
    if (item->get_child_count() > 0)
        return;

    String file_name = item->get_metadata(0);

    UtilityFunctions::print("Clicked SARC file: ", file_name);
}

void SarcInspectorControl::_on_item_activated() 
{
    TreeItem *item = file_tree->get_selected();
    if (!item)
        return;

    if (item->get_child_count() > 0)
        return;

    if (m_archive.is_null())
        return;

    String file_name = item->get_metadata(0);
    uint32_t offset = m_archive->get_file_offset(file_name);
    uint32_t size = m_archive->get_file_size(file_name);
    PackedByteArray data = m_archive->get_file_data(file_name);

    UtilityFunctions::print("Double-Clicked SARC file: ", file_name, " | Off=", offset, " | size=", size);
}

void SarcInspectorControl::setup(Ref<SeadArchive> archive) {
    m_archive = archive;

    if (file_tree) {
        file_tree->queue_free();
    }

    // Create Tree Widget
    file_tree = memnew(Tree);
    file_tree->set_columns(2);
    file_tree->set_column_titles_visible(true);
    file_tree->set_column_title(0, "FileName");
    file_tree->set_column_title(1, "Size");
    file_tree->set_custom_minimum_size(Vector2(0, 250));
    file_tree->connect(
        "item_mouse_selected",
        Callable(this, "_on_item_mouse_selected")
    );
    file_tree->connect(
        "item_activated",
        Callable(this, "_on_item_activated")
    );
    add_child(file_tree);

    TreeItem *root = file_tree->create_item();
    root->set_text(0, "archive://");

    // Parse paths into TreeItems
    PackedStringArray files = archive->get_files();
    HashMap<String, TreeItem*> dir_nodes;

    for (int i = 0; i < files.size(); i++) {
        String path = files[i];
        PackedStringArray parts = path.split("/");

        TreeItem *current_parent = root;
        String current_path = "";

        for (int j = 0; j < parts.size(); j++) {
            String part = parts[j];
            current_path += (j == 0 ? "" : "/") + part;

            if (j == parts.size() - 1) {
                // Leaf Node (File)
                TreeItem *file_item = file_tree->create_item(current_parent);
                file_item->set_text(0, part);
                file_item->set_text(1, vformat("%d bytes", archive->get_file_size(path)));

                //Set metadata
                file_item->set_metadata(0, path);
            } else {
                // Directory Node
                if (!dir_nodes.has(current_path)) {
                    TreeItem *dir_item = file_tree->create_item(current_parent);
                    dir_item->set_text(0, part);
                    dir_item->set_selectable(0, false);
                    dir_nodes[current_path] = dir_item;
                }
                current_parent = dir_nodes[current_path];
            }
        }
    }
}

String SeadArchive::read_cstring(const PackedByteArray &data, int offset) {
	int end = offset;
	const int len = data.size();
	while (end < len && data[end] != 0) {
		end++;
	}
	return data.slice(offset, end).get_string_from_utf8();
}

Dictionary SeadArchive::get_file_list() const {
    Dictionary dict;
    for (const KeyValue<String, FileNode> &E : m_files) {
        Dictionary file_info;
        file_info["offset"] = E.value.data_begin;
        file_info["size"] = E.value.data_end - E.value.data_begin;
        dict[E.key] = file_info;
    }
    return dict;
}

PackedStringArray SeadArchive::get_files() const {
    PackedStringArray list;
    for (const KeyValue<String, FileNode> &E : m_files) {
        list.push_back(E.key);
    }
    return list;
}

bool SeadArchive::has_file(const String &p_name) const {
    return m_files.has(p_name);
}

int SeadArchive::get_file_size(const String &p_name) const {
    ERR_FAIL_COND_V(!m_files.has(p_name), 0);
    const FileNode &node = m_files[p_name];
    return node.data_end - node.data_begin;
}

int SeadArchive::get_file_offset(const String &p_name) const {
    ERR_FAIL_COND_V(!m_files.has(p_name), 0);
    return m_files[p_name].data_begin;
}

PackedByteArray SeadArchive::get_file_data(const String &p_name) const {
    ERR_FAIL_COND_V(!m_files.has(p_name), PackedByteArray());
    const FileNode &node = m_files[p_name];
    int start = m_data_offset + node.data_begin;
    int end = m_data_offset + node.data_end;
    return m_data.slice(start, end);
}

void SeadArchive::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load SARC file: %s", file));

    Ref<StreamPeerBuffer> sp;
    sp.instantiate();
    sp->set_data_array(buf);

    sp->seek(0);

    // SARC header
    SARCHeader hdr(sp);
    ERR_FAIL_COND(hdr.magic != "SARC");

    // SFAT header
	sp->seek(hdr.header_size);
	SFATHeader sfat(sp);
	ERR_FAIL_COND_MSG(sfat.magic != "SFAT", "Unknown SFAT magic");
	ERR_FAIL_COND_MSG(sfat.header_size != 0xc, "Unexpected SFAT header size");
	ERR_FAIL_COND_MSG((sfat.node_count >> 0xe) != 0, "Too many entries");
 
	const int node_offset = hdr.header_size + sfat.header_size;
	sp->seek(node_offset);
 
	Vector<SFATNode> nodes;
	nodes.resize(sfat.node_count);
	for (int i = 0; i < sfat.node_count; i++) {
		nodes.set(i, SFATNode(sp));
	}
 
	// SFNT header
	const int sfnt_header_offset = node_offset + SFAT_NODE_SIZE * sfat.node_count;
	sp->seek(sfnt_header_offset);
	SFNTHeader sfnt(sp);
	ERR_FAIL_COND_MSG(sfnt.magic != "SFNT", "Unknown SFNT magic");
	ERR_FAIL_COND_MSG(sfnt.header_size != 8, "Unexpected SFNT header size");
 
	const int name_table_offset = sfnt_header_offset + sfnt.header_size;
	ERR_FAIL_COND_MSG((int)hdr.data_offset < name_table_offset,
			"File data should not be stored before the name table");
 
	// Only commit state once everything above has validated successfully.
	m_data = buf;
	m_data_offset = hdr.data_offset;
	m_big_endian = hdr.big_endian;
	m_files.clear();
 
	for (int i = 0; i < nodes.size(); i++) {
		const SFATNode &node = nodes[i];
		ERR_FAIL_COND_MSG(node.flags == 0 && node.name_offset == 0,
				"Unnamed files are not supported");
 
		const int abs_name_offset = name_table_offset + 4 * node.name_offset;
		ERR_FAIL_COND_MSG(abs_name_offset > (int)hdr.data_offset,
				vformat("Invalid name offset for hash 0x%08x", node.name_hash));
 
		FileNode fn;
		fn.name = read_cstring(m_data, abs_name_offset);
		fn.data_begin = node.file_data_begin;
		fn.data_end = node.file_data_end;
		m_files[fn.name] = fn;
	}

}

bool EditorInspectorPluginSARC::_parse_property(Object *object, Variant::Type type, const String &name, PropertyHint hint_type, const String &hint_string, BitField<PropertyUsageFlags> usage_flags, bool wide)
{
    if (name == "file_list") {
        Ref<SeadArchive> archive = Object::cast_to<SeadArchive>(object);

        if (archive.is_valid()) {
            SarcInspectorControl *control = memnew(SarcInspectorControl);
            control->setup(archive);

            add_property_editor(
                "file_list",
                control,
                true,
                "File List"
            );

            return true;
        }
    }

    return false;
}

Variant ResourceFormatLoaderSARC::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<SeadArchive> sarc;
    sarc.instantiate();
    sarc->LoadFromFile(p_path);
    return sarc;
}

String ResourceFormatLoaderSARC::_get_resource_type(const String &p_path) const {
    return "SeadArchive";
}

PackedStringArray ResourceFormatLoaderSARC::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("arc");
    return exts;
}

bool ResourceFormatLoaderSARC::_handles_type(const StringName &p_type) const
{
    return p_type == String("SeadArchive");
}