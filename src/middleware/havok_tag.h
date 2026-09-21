#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/classes/tree.hpp>
#include <godot_cpp/classes/tree_item.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/templates/vector.hpp>
#include "utils.h"

#include <vector>
#include <string>
#include <utility>
#include <cmath>
#include <cstring>
#include <algorithm>

#define FOUR_CC(a, b, c, d) (((a) << 24) | ((b) << 16) | ((c) << 8) | ((d) << 0))

#define HAVOK_TAG_TAG0   FOUR_CC('T', 'A', 'G', '0')
#define HAVOK_TAG_SDKV   FOUR_CC('S', 'D', 'K', 'V')

#define HAVOK_TAG_DATA   FOUR_CC('D', 'A', 'T', 'A')
#define HAVOK_TAG_TYPE   FOUR_CC('T', 'Y', 'P', 'E')
#define HAVOK_TAG_INDX   FOUR_CC('I', 'N', 'D', 'X')

#define HAVOK_TAG_TST1   FOUR_CC('T', 'S', 'T', '1')
#define HAVOK_TAG_TNA1   FOUR_CC('T', 'N', 'A', '1')
#define HAVOK_TAG_FST1   FOUR_CC('F', 'S', 'T', '1')
#define HAVOK_TAG_TBDY   FOUR_CC('T', 'B', 'D', 'Y')

#define HAVOK_TAG_ITEM   FOUR_CC('I', 'T', 'E', 'M')

namespace godot {

class HavokSdkVer : public Resource {
	GDCLASS(HavokSdkVer, Resource)

protected:
	static void _bind_methods();

public:
	HavokSdkVer() {}
	~HavokSdkVer() {}

	String Version;
};

class HavokStrings : public Resource {
	GDCLASS(HavokStrings, Resource)

protected:
	static void _bind_methods();

public:
	HavokStrings() {}
	~HavokStrings() {}

	PackedStringArray Strings;
};

enum HavokSectionType : uint32_t
{	
	BRANCH = 0,
	LEAF = 1,
};

struct HavokItemEntry
{
	uint32_t typeIndex;
	uint32_t kind;
	uint32_t offset;
    uint32_t count;

    HavokItemEntry(){}
	HavokItemEntry(Ref<StreamPeerBuffer> sp)
	{
		uint32_t typeIndex_and_kind = sp->get_u32();
		
		typeIndex = typeIndex_and_kind & 0xFFFFFF;
		kind = typeIndex_and_kind >> 24;
		
		offset = sp->get_u32();
		count = sp->get_u32();
	}
};

class HavokItem : public Resource 
{
	GDCLASS(HavokItem, Resource)

protected:
	static void _bind_methods();

public:
	HavokItem() {}
    ~HavokItem() {}

	Vector<HavokItemEntry> Entries;
};

class HavokData : public Resource
{
	GDCLASS(HavokData, Resource)
protected:
	static void _bind_methods();

public:
	HavokData() {}
    ~HavokData() {}

	PackedByteArray Buffer;
};

struct HavokTypeNameEntry
{
	HavokTypeNameEntry(Ref<StreamPeerBuffer> sp)
	{
		//TODO
	}
};

class HavokTypeNameDescriptor : public Resource 
{
	GDCLASS(HavokTypeNameDescriptor, Resource)

protected:
	static void _bind_methods();

public:
	HavokTypeNameDescriptor() {}
    ~HavokTypeNameDescriptor() {}

	Vector<HavokTypeNameEntry> Entries;
};

struct HavokTypeBodyEntry
{
	HavokTypeBodyEntry(Ref<StreamPeerBuffer> sp)
	{
		//
	}
};

class HavokTypeBodyDescriptor : public Resource {
	GDCLASS(HavokTypeBodyDescriptor, Resource)

protected:
	static void _bind_methods();

public:
	HavokTypeBodyDescriptor() {}
    ~HavokTypeBodyDescriptor() {}

	Vector<HavokItemEntry> Entries;
};

struct HavokSection
{
	uint32_t size;
	uint32_t flags;
	uint32_t tag;
	bool isLeaf;
	Ref<StreamPeerBuffer> data;

	HavokSection(Ref<StreamPeerBuffer> sp)
	{
		uint32_t size_and_flags = __builtin_bswap32(sp->get_u32());
		size = (size_and_flags & 0x3FFFFFFF) - 8;
		flags = size_and_flags >> 30;
		tag = __builtin_bswap32(sp->get_u32());

		isLeaf = (flags & LEAF);

		Array res = sp->get_data(size);
		PackedByteArray body = res[1];

		data.instantiate();
		data->set_data_array(body);
	}
};

class HavokTag : public Resource {
	GDCLASS(HavokTag, Resource)

protected:
	static void _bind_methods();

public:
	HavokTag() 
	{
		tree = memnew(Tree);
		tree->set_columns(1);
	}
	~HavokTag() {
		tree->clear();
		memdelete(tree);
	}

	void LoadFromFile(String file);

	TreeItem* get_tree_item() { return tree->get_root(); }
	void GetObject(uint32_t idx);

private:
	Tree* tree;

	void parse_section(Ref<StreamPeerBuffer> sp, TreeItem *parent);
	TreeItem* parse_tag0(Ref<StreamPeerBuffer> sp, uint32_t size);
	TreeItem* parse_sdkv(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_data(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_type(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_indx(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_tst1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_tna1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_fst1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_tbdy(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
	TreeItem* parse_item(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);

	Ref<HavokStrings> ReadStrings(Ref<StreamPeerBuffer> sp, uint32_t size);
    static std::pair<int, uint32_t> read_var32(Ref<StreamPeerBuffer> sp, uint32_t size);
};
}