#include "havok_tag.h"

using namespace godot;

void HavokSdkVer::_bind_methods() 
{
	
}

void HavokItem::_bind_methods() 
{
	
}

void HavokTag::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &HavokTag::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_tree_item"), &HavokTag::get_tree_item);
}

void HavokTag::LoadFromFile(String file)
{
	PackedByteArray buf = FileAccess::get_file_as_bytes(file);
	ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load HAVOK TAG file: %s", file));

	Ref<StreamPeerBuffer> sp;
	sp.instantiate();
	sp->set_data_array(buf);

	parse_section(sp, nullptr);
}

void HavokTag::parse_section(Ref<StreamPeerBuffer> sp, TreeItem *parent) 
{	
	TreeItem *node = nullptr;

	HavokSection section(sp);

	switch(section.tag)
	{
		case HAVOK_TAG_TAG0:
			node = parse_tag0(section.data, section.size);
			break;
		case HAVOK_TAG_SDKV:
			node = parse_sdkv(section.data, parent, section.size);
			break;
		case HAVOK_TAG_DATA:
			node = parse_data(section.data, parent, section.size);
			break;
		case HAVOK_TAG_TYPE:
			node = parse_type(section.data, parent, section.size);
			break;
		case HAVOK_TAG_INDX:
			node = parse_indx(section.data, parent, section.size);
			break;
		case HAVOK_TAG_TST1:
			node = parse_tst1(section.data, parent, section.size);
			break;
		case HAVOK_TAG_TNA1:
			node = parse_tna1(section.data, parent, section.size);
			break;
		case HAVOK_TAG_FST1:
			node = parse_fst1(section.data, parent, section.size);
			break;
		case HAVOK_TAG_TBDY:
			node = parse_tbdy(section.data, parent, section.size);
			break;
		case HAVOK_TAG_ITEM:
			node = parse_item(section.data, parent, section.size);
			break;
		default:
			ERR_FAIL_MSG(vformat("Unknown section tag: 0x%x", section.tag));
			break;
	}

	if(!section.isLeaf && node != nullptr)
		while(section.data->get_position() < section.data->get_size())
			parse_section(section.data, node);
}

TreeItem* HavokTag::parse_tag0(Ref<StreamPeerBuffer> sp, uint32_t size) 
{
	UtilityFunctions::print("Parsing TAG0 section");

	TreeItem *node = tree->create_item();
	node->set_text(0, "TAG0");

	return node;
}

TreeItem* HavokTag::parse_sdkv(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing SDKV section");
	
	Ref<HavokSdkVer> sdk_ver;
	sdk_ver.instantiate();
	sdk_ver->Version = sp->get_string(size);

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "SDKV");
	node->set_metadata(0, sdk_ver);

	return node;
}

TreeItem* HavokTag::parse_data(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing DATA section");
	//TODO: Implement

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "DATA");

	return node;
}

TreeItem* HavokTag::parse_type(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing TYPE section");
	//TODO: Implement

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TYPE");

	return node;
}

TreeItem* HavokTag::parse_indx(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing INDX section");
	//TODO: Implement

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "INDX");

	return node;
}

TreeItem* HavokTag::parse_tst1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing TST1 section");
	
    Ref<HavokStrings> tst = HavokTag::ReadStrings(sp, size);

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TST1");
	node->set_metadata(0, tst);

	return node;
}

TreeItem* HavokTag::parse_tna1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing TNA1 section");
	
    auto [ofs, count] = HavokTag::read_var32(sp, size);

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TNA1");

	return node;
}

TreeItem* HavokTag::parse_fst1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing FST1 section");
	
    Ref<HavokStrings> fst = HavokTag::ReadStrings(sp, size);

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "FST1");
	node->set_metadata(0, fst);

	return node;
}

TreeItem* HavokTag::parse_tbdy(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing TBODY section");
	//TODO: Implement

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TBODY");

	return node;
}

TreeItem* HavokTag::parse_item(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing ITEM section");
	
    Ref<HavokItem> item;
    item.instantiate();
    while(sp->get_position() < size)
    {
        HavokItemEntry entry(sp);
        item->Entries.push_back(entry);
    }

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "ITEM");
    node->set_metadata(0, item);

	return node;
}

Ref<HavokStrings> HavokTag::ReadStrings(Ref<StreamPeerBuffer> sp, uint32_t size) 
{
	Ref<HavokStrings> str;
	str.instantiate();

	uint64_t start_pos = sp->get_position();
    uint64_t end_pos = start_pos + size;

	while(sp->get_position() < end_pos)
	{
		uint64_t currPos = sp->get_position();
		uint8_t first = sp->get_u8();

		if(first == 0xFF)
		{
			sp->get_u8();
			continue;
		}
		sp->seek(currPos);

		String s = Utils::read_null_terminated_string(sp);
		if (!s.is_empty()) 
			str->Strings.push_back(s);
	}

	return str;
}

std::pair<int, uint32_t> HavokTag::read_var32(Ref<StreamPeerBuffer> sp, uint32_t size) 
{
	uint64_t val = 0;

	const uint32_t count = MIN<uint32_t>(8, size);
	for (uint32_t i = 0; i < count; i++) {
		val = (val << 8) | static_cast<uint8_t>(sp->get_u8());
	}

	auto extract = [](uint64_t value, int start, int end) -> uint64_t {
		const int width = end - start + 1;
		return (value >> start) & ((1ULL << width) - 1ULL);
	};

	auto reverse_extract = [&](uint64_t value, int start, int end) -> uint64_t {
		return extract(value, 63 - end, 63 - start);
	};

	const uint64_t msb = reverse_extract(val, 0, 7);
	const uint64_t mode = msb >> 3;

	if (mode <= 15)
		return {1, static_cast<uint32_t>(msb)};

	if (mode <= 23)
		return {2, static_cast<uint32_t>(
			reverse_extract(val, 2, 15)
		)};

	if (mode <= 27)
		return {3, static_cast<uint32_t>(
			reverse_extract(val, 3, 23)
		)};

	if (mode == 28)
		return {4, static_cast<uint32_t>(
			reverse_extract(val, 5, 31)
		)};

	if (mode == 29)
		return {5, static_cast<uint32_t>(
			reverse_extract(val, 5, 39)
		)};

	if (mode == 30)
		return {8, static_cast<uint32_t>(
			reverse_extract(val, 5, 63)
		)};

	return {0, 0};
}