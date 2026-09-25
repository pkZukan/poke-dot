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
	ClassDB::bind_method(D_METHOD("GetObjectCount"), &HavokTag::GetObjectCount);
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
		case HAVOK_TAG_TPAD:
			node = parse_tpad(section.data, parent, section.size);
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
	TreeItem *node = tree->create_item();
	node->set_text(0, "TAG0");

	return node;
}

TreeItem* HavokTag::parse_sdkv(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;
	
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
	
	Ref<HavokData> data;
	data.instantiate();
	data->Buffer = sp;

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "DATA");
	node->set_metadata(0, data);

	return node;
}

TreeItem* HavokTag::parse_type(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TYPE");

	return node;
}

TreeItem* HavokTag::parse_indx(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "INDX");

	return node;
}

TreeItem* HavokTag::parse_tst1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;
	
    Ref<HavokStrings> tst = HavokUtils::ReadStrings(sp, size);

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TST1");
	node->set_metadata(0, tst);

	return node;
}

TreeItem* HavokTag::parse_tna1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	Ref<HavokTypeNameDescriptor> tna;
	tna.instantiate();

	//Get entries cnt
	uint32_t count = HavokUtils::read_var32(sp);
	for(int i = 0; i < count; i++)
	{
		HavokTypeNameEntry name_ent(sp);
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
	
    Ref<HavokStrings> fst = HavokUtils::ReadStrings(sp, size);

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "FST1");
	node->set_metadata(0, fst);

	return node;
}

TreeItem* HavokTag::parse_tbdy(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;

	TreeItem *root = get_tree_item();
    ERR_FAIL_NULL_V_MSG(root, nullptr, "Tree root is null");

	TreeItem *tna_obj = Utils::FindTreeItemByName(root, "TNA1");
    ERR_FAIL_NULL_V_MSG(tna_obj, nullptr, "Couldn't find TNA1");

	Ref<HavokTypeNameDescriptor> tna = tna_obj->get_metadata(0);
    ERR_FAIL_COND_V_MSG(tna.is_null(), nullptr, "TNA1 metadata is not a HavokItem");
	
	Ref<HavokTypeBodyDescriptor> tbod;
	tbod.instantiate();
	
	int i = 0;
	while (sp->get_position() < size) 
	{
		HavokTypeBodyEntry bod_ent(sp);
		if (bod_ent.typeIndex != 0 && bod_ent.typeIndex - 1 < (uint32_t)tna->Entries.size())
			tna->Entries.ptrw()[bod_ent.typeIndex - 1].bodyIndex = i;
		tbod->Entries.push_back(bod_ent);
		i++;
	}

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TBDY");
	node->set_metadata(0, tbod);

	return node;
}

TreeItem* HavokTag::parse_item(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size) 
{
	if(parent == nullptr)
		return nullptr;
	
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

TreeItem* HavokTag::parse_tpad(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size)
{
	if(parent == nullptr)
		return nullptr;
	
    //Run through padding
	while(sp->get_position() < size)
		sp->get_8();

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TPAD");

	return node;
}

String HavokUtils::ResolveTemplateParam(Ref<HavokStrings> tst, Ref<HavokTypeNameDescriptor> tna, const HavokTypeNameParamEntry &p)
{
    String pname = tst->Strings[p.nameIdx];
    if (pname.begins_with("t")) 
	{
        if (p.val == 0 || p.val - 1 >= (uint32_t)tna->Entries.size())
            return "?";
        return tst->Strings[tna->Entries[p.val - 1].nameIdx];
    }
    return String::num_uint64(p.val);
}

void HavokTag::WalkMembers(uint32_t typeIdx, uint32_t base_offset, int depth,
	Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst,
	Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data,
	HashSet<uint32_t> &visiting)
{
	if (typeIdx == 0 || typeIdx - 1 >= (uint32_t)tna->Entries.size())
		return;
	int32_t bodyIdx = tna->Entries[typeIdx - 1].bodyIndex;
	if (bodyIdx < 0)
		return;

	HavokTypeBodyEntry &owner = tbdy->Entries.ptrw()[bodyIdx];
	if (owner.parentIndex != 0)
		WalkMembers(owner.parentIndex, base_offset, depth, item, tst, fst, tna, tbdy, data, visiting); // inherited fields first

	for (int j = 0; j < owner.members.size(); j++)
	{
		HavokFieldEntry memb = owner.members[j];

		HavokTypeBodyEntry::Kind kind = HavokTypeBodyEntry::Kind::VOID;
		HavokTypeBodyEntry field_body;
		bool has_body = false;
		if (memb.typeIndex != 0 && memb.typeIndex - 1 < (uint32_t)tna->Entries.size())
		{
			int32_t fbidx = tna->Entries[memb.typeIndex - 1].bodyIndex;
			if (fbidx >= 0)
			{
				field_body = tbdy->Entries[fbidx];
				kind = (HavokTypeBodyEntry::Kind)(field_body.format & 0x0f);
				has_body = true;
			}
		}

		uint32_t field_off = has_body ? field_body.AlignUp(base_offset + memb.offset) : (base_offset + memb.offset);

		data->Buffer->seek(field_off);
		String indent; for (int d = 0; d < depth; d++) indent += "  ";
		String val;

		switch (kind)
		{
			case HavokTypeBodyEntry::Kind::VOID:
				val = "void";
				break;
			case HavokTypeBodyEntry::Kind::OPAQUE:
				val = "opaque";
				break;
			case HavokTypeBodyEntry::Kind::BOOL:
				val = data->Buffer->get_u8() == 1 ? "true" : "false";
				break;
			case HavokTypeBodyEntry::Kind::STRING:
			{
				uint32_t str_idx = data->Buffer->get_u32();
				if (str_idx == 0 || str_idx >= (uint32_t)item->Entries.size())
					val = "\"\"";
				else
				{
					data->Buffer->seek(item->Entries[str_idx].offset);
					val = "\"" + Utils::read_null_terminated_string(data->Buffer) + "\"";
				}
				break;
			}
			case HavokTypeBodyEntry::Kind::INT:
			{
				uint32_t bytes = MAX<uint32_t>(1, (field_body.format >> 10) / 8);
				bool big_endian = (field_body.format & 0x100) != 0;
				bool is_signed  = (field_body.format & 0x200) != 0;
				uint64_t raw = 0;
				for (uint32_t b = 0; b < bytes; b++)
				{
					uint8_t byte = data->Buffer->get_u8();
					if (big_endian) raw = (raw << 8) | byte;
					else raw |= (uint64_t)byte << (8 * b);
				}
				if (is_signed && bytes < 8)
				{
					uint64_t sign_bit = 1ULL << (bytes * 8 - 1);
					if (raw & sign_bit) raw |= ~((1ULL << (bytes * 8)) - 1);
				}
				val = vformat("%d", (int64_t)raw);
				break;
			}
			case HavokTypeBodyEntry::Kind::FLOAT:
				val = vformat("%f", field_body.size == 8 ? data->Buffer->get_double() : data->Buffer->get_float());
				break;
			case HavokTypeBodyEntry::Kind::POINTER:
			{
				uint32_t ptr = data->Buffer->get_u32();
				if (ptr == 0 || ptr >= (uint32_t)item->Entries.size())
					val = "null";
				else if (visiting.has(ptr))
					val = vformat("-> item[%d] (cycle)", ptr);
				else
				{
					val = vformat("-> item[%d]", ptr);
					UtilityFunctions::print(indent + vformat("+0x%X %s %s", memb.offset, fst->Strings[memb.nameIndex], val));
					visiting.insert(ptr);
					ParseItemEntry(item, tst, fst, tna, tbdy, data, ptr, visiting);
					visiting.erase(ptr);
					continue; // already printed above
				}
				break;
			}
			case HavokTypeBodyEntry::Kind::ARRAY:
			{
				bool inline_array = has_body && (field_body.format & 0x20) != 0;
				if (inline_array)
				{
					int32_t elem_bidx = (field_body.subtype != 0 && field_body.subtype - 1 < (uint32_t)tna->Entries.size())
						? tna->Entries[field_body.subtype - 1].bodyIndex : -1;
					uint32_t elem_stride = (elem_bidx >= 0) ? tbdy->Entries[elem_bidx].size : 0;
					uint32_t elem_count = (elem_stride > 0) ? field_body.size / elem_stride : 0;
					val = vformat("[inline array, count=%d]", elem_count);
					UtilityFunctions::print(indent + vformat("+0x%X %s %s", memb.offset, fst->Strings[memb.nameIndex], val));
					for (uint32_t e = 0; e < elem_count; e++)
						WalkMembers(field_body.subtype, field_off + e * elem_stride, depth + 1, item, tst, fst, tna, tbdy, data, visiting);
					continue;
				}
				uint32_t arr_idx = data->Buffer->get_u32();
				if (arr_idx == 0 || arr_idx >= (uint32_t)item->Entries.size())
					val = "[]";
				else
					val = vformat("-> item[%d] count=%d", arr_idx, item->Entries[arr_idx].count);
				break;
			}
			case HavokTypeBodyEntry::Kind::RECORD:
				val = "{struct}";
				UtilityFunctions::print(indent + vformat("+0x%X %s %s", memb.offset, fst->Strings[memb.nameIndex], val));
				WalkMembers(memb.typeIndex, field_off, depth + 1, item, tst, fst, tna, tbdy, data, visiting); // same offset, no dereference
				continue;
			default:
				val = "?";
		}
		UtilityFunctions::print(indent + vformat("+0x%X %s %s", memb.offset, fst->Strings[memb.nameIndex], val));
	}
}

void HavokTag::ParseItemEntry(Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data, uint32_t idx)
{
	HashSet<uint32_t> visiting;
	visiting.insert(idx);
	ParseItemEntry(item, tst, fst, tna, tbdy, data, idx, visiting);
}

void HavokTag::ParseItemEntry(Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data, uint32_t idx, HashSet<uint32_t> &visiting)
{
	if (idx >= (uint32_t)item->Entries.size())
		return;

	const auto &item_ent = item->Entries[idx];
	uint32_t typeIdx = item_ent.typeIndex;
	if (typeIdx == 0 || typeIdx - 1 >= (uint32_t)tna->Entries.size())
		return;

	HavokTypeNameEntry tna_ent = tna->Entries[typeIdx - 1];
	String name = tst->Strings[tna_ent.nameIdx];
	PackedStringArray params;
	for (int i = 0; i < tna_ent.params.size(); i++)
		params.append(HavokUtils::ResolveTemplateParam(tst, tna, tna_ent.params[i]));

	uint32_t stride = (tna_ent.bodyIndex >= 0) ? tbdy->Entries[tna_ent.bodyIndex].size : 0;
	uint32_t elem_count = MAX<uint32_t>(1, item_ent.count);
	for (uint32_t e = 0; e < elem_count && stride > 0; e++)
	{
		if (elem_count > 1)
			UtilityFunctions::print(vformat("%s<%s> [%d]", name, String(", ").join(params), e));
		else
			UtilityFunctions::print(vformat("%s<%s>", name, String(", ").join(params)));
		WalkMembers(typeIdx, item_ent.offset + e * stride, 1, item, tst, fst, tna, tbdy, data, visiting);
	}
}

void HavokTag::GetObject(uint32_t idx)
{
    TreeItem *root = get_tree_item();
    ERR_FAIL_NULL_MSG(root, "Tree root is null");

    TreeItem *item_obj = Utils::FindTreeItemByName(root, "ITEM");
    ERR_FAIL_NULL_MSG(item_obj, "Couldn't find ITEM");

	Ref<HavokItem> item = item_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(item.is_null(), "ITEM metadata is not a HavokItem");

	if (idx >= item->Entries.size())
		return;

	TreeItem *tst_obj = Utils::FindTreeItemByName(root, "TST1");
    ERR_FAIL_NULL_MSG(tst_obj, "Couldn't find TST1");

	Ref<HavokStrings> tst = tst_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(tst.is_null(), "TST1 metadata is not a HavokItem");

	TreeItem *fst_obj = Utils::FindTreeItemByName(root, "FST1");
    ERR_FAIL_NULL_MSG(fst_obj, "Couldn't find FST1");

	Ref<HavokStrings> fst = fst_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(fst.is_null(), "FST1 metadata is not a HavokItem");

	TreeItem *tna_obj = Utils::FindTreeItemByName(root, "TNA1");
    ERR_FAIL_NULL_MSG(tna_obj, "Couldn't find TNA1");

	Ref<HavokTypeNameDescriptor> tna = tna_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(tna.is_null(), "TNA1 metadata is not a HavokItem");

	TreeItem *tbdy_obj = Utils::FindTreeItemByName(root, "TBDY");
    ERR_FAIL_NULL_MSG(tbdy_obj, "Couldn't find TBDY");

	Ref<HavokTypeBodyDescriptor> tbdy = tbdy_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(tbdy.is_null(), "TBDY metadata is not a HavokItem");

	TreeItem *data_obj = Utils::FindTreeItemByName(root, "DATA");
    ERR_FAIL_NULL_MSG(data_obj, "Couldn't find DATA");

	Ref<HavokData> data = data_obj->get_metadata(0);
    ERR_FAIL_COND_MSG(data.is_null(), "DATA metadata is not a HavokItem");

	ParseItemEntry(item, tst, fst, tna, tbdy, data, idx);
}

uint32_t HavokTag::GetObjectCount()
{
	TreeItem *root = get_tree_item();
    ERR_FAIL_NULL_V_MSG(root, 0, "Tree root is null");

    TreeItem *item_obj = Utils::FindTreeItemByName(root, "ITEM");
    ERR_FAIL_NULL_V_MSG(item_obj, 0, "Couldn't find ITEM");

	Ref<HavokItem> item = item_obj->get_metadata(0);
    ERR_FAIL_COND_V_MSG(item.is_null(), 0, "ITEM metadata is not a HavokItem");

	return item->Entries.size();
}

Ref<HavokStrings> HavokUtils::ReadStrings(Ref<StreamPeerBuffer> sp, uint32_t size) 
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