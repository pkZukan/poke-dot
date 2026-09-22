#include "havok_tag.h"

using namespace godot;

void HavokSdkVer::_bind_methods() 
{
	
}

void HavokItem::_bind_methods() 
{
	
}

void HavokStrings::_bind_methods()
{

}

void HavokData::_bind_methods()
{

}

void HavokTypeNameDescriptor::_bind_methods()
{

}

void HavokTypeBodyDescriptor::_bind_methods()
{

}

void HavokTag::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &HavokTag::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_tree_item"), &HavokTag::get_tree_item);
	ClassDB::bind_method(D_METHOD("GetObject", "idx"), &HavokTag::GetObject);
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
	
	Ref<HavokData> data;
	data.instantiate();
	data->Buffer = sp->get_data_array();

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "DATA");
	node->set_metadata(0, data);

	return node;
}

TreeItem* HavokTag::parse_type(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing TYPE section");

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TYPE");

	return node;
}

TreeItem* HavokTag::parse_indx(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	UtilityFunctions::print("Parsing INDX section");

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

	Ref<HavokTypeNameDescriptor> tna;
	tna.instantiate();

	//Get entries cnt
	uint32_t count = HavokUtils::read_var32(sp);
	UtilityFunctions::print(vformat("Count: %d", count));
	for(int i = 0; i < count; i++)
	{
		HavokTypeNameEntry name_ent(sp);
		UtilityFunctions::print(vformat("Nameidx: %d", name_ent.nameIdx));
		for(int j = 0; j < name_ent.params.size(); j++)
			UtilityFunctions::print(vformat("Param: %d", name_ent.params[j].nameIdx));
		tna->Entries.push_back(name_ent);
	}

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TNA1");
	node->set_metadata(0, tna);

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
	
	Ref<HavokTypeBodyDescriptor> tbod;
	tbod.instantiate();

	//TODO: impl

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TBODY");
	node->set_metadata(0, tbod);

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

void HavokTag::GetObject(uint32_t idx)
{
    TreeItem *root = get_tree_item();
    ERR_FAIL_NULL_MSG(root, "Tree root is null");

    TreeItem *item_obj = Utils::FindTreeItemByName(root, "ITEM");
    ERR_FAIL_NULL_MSG(item_obj, "Couldn't find ITEM");

	Ref<HavokItem> item = item_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(item.is_null(), "ITEM metadata is not a HavokItem");

	TreeItem *tst_obj = Utils::FindTreeItemByName(root, "TST1");
    ERR_FAIL_NULL_MSG(tst_obj, "Couldn't find ITEM");

	Ref<HavokStrings> tst = tst_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(tst.is_null(), "TST1 metadata is not a HavokItem");

	TreeItem *tna_obj = Utils::FindTreeItemByName(root, "TNA1");
    ERR_FAIL_NULL_MSG(tna_obj, "Couldn't find TNA1");

	Ref<HavokTypeNameDescriptor> tna = tna_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(tna.is_null(), "TNA1 metadata is not a HavokItem");

    const auto &item_ent = item->Entries[idx];
	uint32_t typeIdx = item_ent.typeIndex;
	auto tna_ent = tna->Entries[typeIdx];
	String name = tst->Strings[tna_ent.nameIdx];
	for(int i = 0; i < tna_ent.params.size(); i++)
	{
		auto p_ent = tna_ent.params[i];
		String paramName = tst->Strings[p_ent.nameIdx];
		//TODO
	}
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

uint32_t HavokUtils::read_var32(Ref<StreamPeerBuffer> sp, uint32_t *bytes_read) 
{
    uint64_t val = 0;

    const int64_t start_pos = sp->get_position();
    const uint32_t count = MIN<uint32_t>(8, sp->get_size() - start_pos);
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

    uint32_t local_bytes_read = 0;
    uint32_t result = 0;

    if (mode <= 15) 
	{
        local_bytes_read = 1;
        result = static_cast<uint32_t>(msb);
    } else if (mode <= 23) 
	{
        local_bytes_read = 2;
        result = static_cast<uint32_t>(reverse_extract(val, 2, 15));
    } else if (mode <= 27) 
	{
        local_bytes_read = 3;
        result = static_cast<uint32_t>(reverse_extract(val, 3, 23));
    } else if (mode == 28) 
	{
        local_bytes_read = 4;
        result = static_cast<uint32_t>(reverse_extract(val, 5, 31));
    } else if (mode == 29) 
	{
        local_bytes_read = 5;
        result = static_cast<uint32_t>(reverse_extract(val, 5, 39));
    } else if (mode == 30) 
	{
        local_bytes_read = 8;
        result = static_cast<uint32_t>(reverse_extract(val, 5, 63));
    } else 
	{
        local_bytes_read = 0;
        result = 0;
    }

    sp->seek(start_pos + local_bytes_read); // only "consume" what the varint actually used

    if (bytes_read)
        *bytes_read = local_bytes_read;

    return result;
}