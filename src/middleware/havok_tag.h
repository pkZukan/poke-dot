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
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
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
#define HAVOK_TAG_TPAD   FOUR_CC('T', 'P', 'A', 'D')

namespace godot {

class HavokStrings;
class HavokTypeNameDescriptor;
class HavokTypeNameParamEntry;

namespace HavokUtils
{
	static uint32_t read_var32(Ref<StreamPeerBuffer> sp, uint32_t *bytes_read = nullptr);
	static Ref<HavokStrings> ReadStrings(Ref<StreamPeerBuffer> sp, uint32_t size);
	String ResolveTemplateParam(Ref<HavokStrings> tst, Ref<HavokTypeNameDescriptor> tna, const HavokTypeNameParamEntry &p);
}

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
	enum Kind
	{
		NONE = 0,
		POINTER = 1,
		ARRAY = 2
	};

	uint32_t typeIndex;
	Kind kind;
	uint32_t offset;
	uint32_t count;

	HavokItemEntry(){}
	HavokItemEntry(Ref<StreamPeerBuffer> sp)
	{
		uint32_t typeIndex_and_kind = sp->get_u32();
		typeIndex = typeIndex_and_kind & 0xFFFFFF;
		kind = (Kind)(typeIndex_and_kind >> 24);
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
	Ref<StreamPeerBuffer> Buffer;
};

struct HavokTypeNameParamEntry
{
	uint32_t nameIdx;
	uint32_t val;

	HavokTypeNameParamEntry(){}
	HavokTypeNameParamEntry(Ref<StreamPeerBuffer> sp)
	{
		nameIdx = HavokUtils::read_var32(sp);
		val = HavokUtils::read_var32(sp);
	}
};

struct HavokTypeNameEntry
{
	uint32_t nameIdx;
	Vector<HavokTypeNameParamEntry> params;
	int32_t bodyIndex = -1;

	HavokTypeNameEntry(){}
	HavokTypeNameEntry(Ref<StreamPeerBuffer> sp)
	{
		nameIdx = HavokUtils::read_var32(sp);
		uint32_t paramCnt = HavokUtils::read_var32(sp);
		for(int i = 0; i < paramCnt; i++)
		{
			HavokTypeNameParamEntry paramEnt(sp);
			params.push_back(paramEnt);
		}
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

struct HavokTypeBodyInterfaceEntry
{
	uint32_t typeIndex;
	uint32_t offset;

	HavokTypeBodyInterfaceEntry(){}
	HavokTypeBodyInterfaceEntry(Ref<StreamPeerBuffer> sp)
	{
		typeIndex = HavokUtils::read_var32(sp);
		offset = HavokUtils::read_var32(sp);
	}
};

struct HavokFieldEntry
{
	uint32_t nameIndex;
	uint32_t flags;
	uint32_t offset;
	uint32_t typeIndex;

	HavokFieldEntry(){}
	HavokFieldEntry(Ref<StreamPeerBuffer> sp)
	{
		nameIndex = HavokUtils::read_var32(sp);
		flags = HavokUtils::read_var32(sp);
		offset = HavokUtils::read_var32(sp);
		typeIndex = HavokUtils::read_var32(sp);
	}
};

struct HavokTypeBodyEntry
{
public:
	enum Kind
	{
		VOID = 0,
		OPAQUE = 1,
		BOOL = 2,
		STRING = 3,
		INT = 4,
		FLOAT = 5,
		POINTER = 6,
		RECORD = 7,
		ARRAY = 8
	};

	uint32_t typeIndex = 0;
	uint32_t parentIndex = 0;
	uint32_t format = 0;
	Kind kind = Kind::VOID;
	uint32_t subtype = 0;
	uint32_t version = 0;
	uint32_t size = 0;
	uint32_t alignment = 0;
	uint32_t flags = 0;
	Vector<HavokFieldEntry> members;
	Vector<HavokTypeBodyInterfaceEntry> interfaces;
	uint32_t attribs = 0;

	enum OptionalFlags
	{
		HAS_FORMAT =	 (1 << 0),
		HAS_SUBTYPE =	 (1 << 1),
		HAS_VERSION =	 (1 << 2),
		HAS_SIZE_ALIGN = (1 << 3),
		HAS_FLAGS = 	 (1 << 4),
		HAS_MEMBERS = 	 (1 << 5),
		HAS_INTERFACES = (1 << 6),
		HAS_ATTRIBUTES = (1 << 7),
	};

private:
	uint32_t opts = 0;

public:
	HavokTypeBodyEntry(){}
	HavokTypeBodyEntry(Ref<StreamPeerBuffer> sp)
	{
		typeIndex = HavokUtils::read_var32(sp);
		if(typeIndex != 0)
		{
			parentIndex = HavokUtils::read_var32(sp);
			opts = HavokUtils::read_var32(sp);
			if(opts & OptionalFlags::HAS_FORMAT)
			{
				format = HavokUtils::read_var32(sp);
				kind = (Kind)(format & 0xF);
			}
			if(opts & OptionalFlags::HAS_SUBTYPE)
				subtype = HavokUtils::read_var32(sp);
			if(opts & OptionalFlags::HAS_VERSION)
				version = HavokUtils::read_var32(sp);
			if(opts & OptionalFlags::HAS_SIZE_ALIGN)
			{
				size = HavokUtils::read_var32(sp);
				alignment = HavokUtils::read_var32(sp);
			}
			if(opts & OptionalFlags::HAS_FLAGS)
				flags = HavokUtils::read_var32(sp);
			if(opts & OptionalFlags::HAS_MEMBERS)
			{
				uint32_t num = HavokUtils::read_var32(sp);
				uint16_t fieldCnt = num & 0xFFFF;
				for(int i = 0; i < fieldCnt; i++)
				{
					HavokFieldEntry ent(sp);
					members.push_back(ent);
				}
			}
			if (opts & OptionalFlags::HAS_INTERFACES)
			{
				uint32_t num = HavokUtils::read_var32(sp);
				for (uint32_t i = 0; i < num; i++)
					interfaces.push_back(HavokTypeBodyInterfaceEntry(sp));
			}
			//if(opts & OptionalFlags::HAS_ATTRIBUTES)
			//	attribs = HavokUtils::read_var32(sp);
		}
	}

	uint32_t AlignUp(uint32_t x) const
	{
		if ((opts & OptionalFlags::HAS_SIZE_ALIGN) && alignment > 0)
			x = (x + alignment - 1) & ~(alignment - 1);
		return x;
	}
};

class HavokTypeBodyDescriptor : public Resource {
	GDCLASS(HavokTypeBodyDescriptor, Resource)
protected:
	static void _bind_methods();
public:
	HavokTypeBodyDescriptor() {}
	~HavokTypeBodyDescriptor() {}
	Vector<HavokTypeBodyEntry> Entries;
};

struct HavokSimdTreeNode
{
	float lx[4], hx[4];
	float ly[4], hy[4];
	float lz[4], hz[4];
	uint32_t data[4];
	bool isLeaf;

	bool IsBound(int i) const { return lx[i] <= hx[i] && ly[i] <= hy[i] && lz[i] <= hz[i]; }
	static const int NodeCount = 4;
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

// ---- Cursor-based read layer ----

struct HavokContext
{
	Ref<HavokItem> item;
	Ref<HavokStrings> tst;
	Ref<HavokStrings> fst;
	Ref<HavokTypeNameDescriptor> tna;
	Ref<HavokTypeBodyDescriptor> tbdy;
	Ref<HavokData> data;

	bool IsValid() const { return item.is_valid() && tst.is_valid() && fst.is_valid() && tna.is_valid() && tbdy.is_valid() && data.is_valid(); }
};

struct HavokMeshSection
{
	PackedVector3Array vertices;
	PackedInt32Array faceIndices;
};

struct HavokCursor
{
	HavokContext *ctx = nullptr;
	uint32_t typeIdx = 0;
	uint32_t offset = 0;
	bool valid = false;
	HavokTypeBodyEntry::Kind kind = HavokTypeBodyEntry::Kind::VOID;
	HavokTypeBodyEntry body;

	// only meaningful when kind == ARRAY
	uint32_t arrCount = 0, arrElemType = 0, arrStride = 0, arrOffset = 0;

	bool IsNull() const { return !valid; }
	HavokCursor Field(const String &name) const;
	HavokCursor operator[](uint32_t i) const;
	uint32_t Count() const { return arrCount; }
	int64_t AsInt() const;
	double AsFloat() const;
	bool AsBool() const;
	String AsString() const;
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

	// cursor-based reads
	HavokContext BuildContext();
	HavokCursor Root(uint32_t itemIdx, HavokContext &ctx);
	Vector<HavokMeshSection> GetGeometrySections();

	static bool ResolveTypeKind(uint32_t typeIdx, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy,
		HavokTypeBodyEntry::Kind &kind, HavokTypeBodyEntry &body);

private:
	Tree* tree;

	// dump path
	void ParseItemEntry(Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data, uint32_t idx);
	void ParseItemEntry(Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data, uint32_t idx, HashSet<uint32_t> &visiting);
	void WalkMembers(uint32_t typeIdx, uint32_t base_offset, int depth, Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data, HashSet<uint32_t> &visiting, bool is_inherited = false);

	// shared lookup helpers
	int32_t FindItemByTypeName(Ref<HavokItem> item, Ref<HavokTypeNameDescriptor> tna, Ref<HavokStrings> tst, const String &typeName);

	// chunk parsing
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
	TreeItem* parse_tpad(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size);
};
}