#include "havok_tag.h"

using namespace godot;

void HavokSdkVer::_bind_methods() {}
void HavokItem::_bind_methods() {}
void HavokStrings::_bind_methods() {}
void HavokData::_bind_methods() {}
void HavokTypeNameDescriptor::_bind_methods() {}
void HavokTypeBodyDescriptor::_bind_methods() {}

void HavokTag::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &HavokTag::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_tree_item"), &HavokTag::get_tree_item);
}

// ---------------- Loading / chunk parsing ----------------

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
		case HAVOK_TAG_TAG0: node = parse_tag0(section.data, section.size); break;
		case HAVOK_TAG_SDKV: node = parse_sdkv(section.data, parent, section.size); break;
		case HAVOK_TAG_DATA: node = parse_data(section.data, parent, section.size); break;
		case HAVOK_TAG_TYPE: node = parse_type(section.data, parent, section.size); break;
		case HAVOK_TAG_INDX: node = parse_indx(section.data, parent, section.size); break;
		case HAVOK_TAG_TST1: node = parse_tst1(section.data, parent, section.size); break;
		case HAVOK_TAG_TNA1: node = parse_tna1(section.data, parent, section.size); break;
		case HAVOK_TAG_FST1: node = parse_fst1(section.data, parent, section.size); break;
		case HAVOK_TAG_TBDY: node = parse_tbdy(section.data, parent, section.size); break;
		case HAVOK_TAG_ITEM: node = parse_item(section.data, parent, section.size); break;
		case HAVOK_TAG_TPAD: node = parse_tpad(section.data, parent, section.size); break;
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
	if(parent == nullptr) return nullptr;

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
	if(parent == nullptr) return nullptr;

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
	if(parent == nullptr) return nullptr;
	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TYPE");
	return node;
}

TreeItem* HavokTag::parse_indx(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size)
{
	if(parent == nullptr) return nullptr;
	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "INDX");
	return node;
}

TreeItem* HavokTag::parse_tst1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size)
{
	if(parent == nullptr) return nullptr;

	Ref<HavokStrings> tst = HavokUtils::ReadStrings(sp, size);
	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TST1");
	node->set_metadata(0, tst);
	return node;
}

TreeItem* HavokTag::parse_tna1(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size)
{
	if(parent == nullptr) return nullptr;

	Ref<HavokTypeNameDescriptor> tna;
	tna.instantiate();

	uint32_t count = HavokUtils::read_var32(sp);
	for(uint32_t i = 0; i < count; i++)
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
	if(parent == nullptr) return nullptr;

	Ref<HavokStrings> fst = HavokUtils::ReadStrings(sp, size);
	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "FST1");
	node->set_metadata(0, fst);
	return node;
}

TreeItem* HavokTag::parse_tbdy(Ref<StreamPeerBuffer> sp, TreeItem *parent, uint32_t size)
{
	if(parent == nullptr) return nullptr;

	TreeItem *root = get_tree_item();
	ERR_FAIL_NULL_V_MSG(root, nullptr, "Tree root is null");

	TreeItem *tna_obj = Utils::FindTreeItemByName(root, "TNA1");
	ERR_FAIL_NULL_V_MSG(tna_obj, nullptr, "Couldn't find TNA1");

	Ref<HavokTypeNameDescriptor> tna = tna_obj->get_metadata(0);
	ERR_FAIL_COND_V_MSG(tna.is_null(), nullptr, "TNA1 metadata is not a HavokTypeNameDescriptor");

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
	if(parent == nullptr) return nullptr;

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
	if(parent == nullptr) return nullptr;

	while(sp->get_position() < size)
		sp->get_8();

	TreeItem *node = tree->create_item(parent);
	node->set_text(0, "TPAD");
	return node;
}

// ---------------- Shared utils ----------------

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
	for (uint32_t i = 0; i < count; i++)
		val = (val << 8) | static_cast<uint8_t>(sp->get_u8());

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

	if (mode <= 15) { local_bytes_read = 1; result = static_cast<uint32_t>(msb); }
	else if (mode <= 23) { local_bytes_read = 2; result = static_cast<uint32_t>(reverse_extract(val, 2, 15)); }
	else if (mode <= 27) { local_bytes_read = 3; result = static_cast<uint32_t>(reverse_extract(val, 3, 23)); }
	else if (mode == 28) { local_bytes_read = 4; result = static_cast<uint32_t>(reverse_extract(val, 5, 31)); }
	else if (mode == 29) { local_bytes_read = 5; result = static_cast<uint32_t>(reverse_extract(val, 5, 39)); }
	else if (mode == 30) { local_bytes_read = 8; result = static_cast<uint32_t>(reverse_extract(val, 5, 63)); }
	else { local_bytes_read = 0; result = 0; }

	sp->seek(start_pos + local_bytes_read);
	if (bytes_read) *bytes_read = local_bytes_read;
	return result;
}

// ---------------- Debug dump path (WalkMembers / ParseItemEntry / GetObject) ----------------

void HavokTag::WalkMembers(uint32_t typeIdx, uint32_t base_offset, int depth,
	Ref<HavokItem> item, Ref<HavokStrings> tst, Ref<HavokStrings> fst,
	Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy, Ref<HavokData> data,
	HashSet<uint32_t> &visiting, bool is_inherited)
{
	if (typeIdx == 0 || typeIdx - 1 >= (uint32_t)tna->Entries.size()) return;
	int32_t bodyIdx = tna->Entries[typeIdx - 1].bodyIndex;
	if (bodyIdx < 0) return;

	HavokTypeBodyEntry &owner = tbdy->Entries.ptrw()[bodyIdx];
	if (owner.parentIndex != 0)
		WalkMembers(owner.parentIndex, base_offset, depth, item, tst, fst, tna, tbdy, data, visiting, true);

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
			case HavokTypeBodyEntry::Kind::VOID: val = "void"; break;
			case HavokTypeBodyEntry::Kind::OPAQUE: val = "opaque"; break;
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
					continue;
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
				WalkMembers(memb.typeIndex, field_off, depth + 1, item, tst, fst, tna, tbdy, data, visiting);
				continue;
			default:
				val = "?";
		}
		String tag = is_inherited ? " [inherited]" : "";
		UtilityFunctions::print(indent + vformat("+0x%X %s %s%s", memb.offset, fst->Strings[memb.nameIndex], val, tag));
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
	if (idx >= (uint32_t)item->Entries.size()) return;

	const auto &item_ent = item->Entries[idx];
	uint32_t typeIdx = item_ent.typeIndex;
	if (typeIdx == 0 || typeIdx - 1 >= (uint32_t)tna->Entries.size()) return;

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

// ---------------- Cursor-based read layer ----------------

bool HavokTag::ResolveTypeKind(uint32_t typeIdx, Ref<HavokTypeNameDescriptor> tna, Ref<HavokTypeBodyDescriptor> tbdy,
	HavokTypeBodyEntry::Kind &kind, HavokTypeBodyEntry &body)
{
	if (typeIdx == 0 || typeIdx - 1 >= (uint32_t)tna->Entries.size())
		return false;
	int32_t bodyIdx = tna->Entries[typeIdx - 1].bodyIndex;
	if (bodyIdx < 0)
		return false;
	body = tbdy->Entries[bodyIdx];
	kind = (HavokTypeBodyEntry::Kind)(body.format & 0x0f);
	return true;
}

int32_t HavokTag::FindItemByTypeName(Ref<HavokItem> item, Ref<HavokTypeNameDescriptor> tna, Ref<HavokStrings> tst, const String &typeName)
{
	for (int i = 0; i < item->Entries.size(); i++)
	{
		uint32_t ti = item->Entries[i].typeIndex;
		if (ti == 0 || ti - 1 >= (uint32_t)tna->Entries.size())
			continue;
		if (tst->Strings[tna->Entries[ti - 1].nameIdx] == typeName)
			return i;
	}
	return -1;
}

HavokContext HavokTag::BuildContext()
{
	HavokContext ctx;
	TreeItem *root = get_tree_item();
	if (!root) return ctx;

	auto fetch = [&](const String &name) -> Ref<Resource> {
		TreeItem *n = Utils::FindTreeItemByName(root, name);
		return n ? Ref<Resource>(n->get_metadata(0)) : Ref<Resource>();
	};
	ctx.item = fetch("ITEM");
	ctx.tst  = fetch("TST1");
	ctx.fst  = fetch("FST1");
	ctx.tna  = fetch("TNA1");
	ctx.tbdy = fetch("TBDY");
	ctx.data = fetch("DATA");
	return ctx;
}

HavokCursor HavokTag::Root(uint32_t itemIdx, HavokContext &ctx)
{
	HavokCursor c;
	if (ctx.item.is_null() || itemIdx >= (uint32_t)ctx.item->Entries.size())
		return c;

	const auto &ent = ctx.item->Entries[itemIdx];
	c.ctx = &ctx;
	c.typeIdx = ent.typeIndex;
	c.offset = ent.offset;
	c.valid = ResolveTypeKind(ent.typeIndex, ctx.tna, ctx.tbdy, c.kind, c.body);

	if (c.valid && c.kind == HavokTypeBodyEntry::Kind::ARRAY)
	{
		c.arrCount = ent.count;
		c.arrElemType = ent.typeIndex;
		c.arrStride = c.body.size;
		c.arrOffset = ent.offset;
	}
	return c;
}

HavokCursor HavokCursor::Field(const String &name) const
{
	HavokCursor out;
	if (!valid || kind != HavokTypeBodyEntry::Kind::RECORD) return out;

	uint32_t curType = typeIdx;
	while (curType != 0 && curType - 1 < (uint32_t)ctx->tna->Entries.size())
	{
		int32_t bodyIdx = ctx->tna->Entries[curType - 1].bodyIndex;
		if (bodyIdx < 0) break;
		HavokTypeBodyEntry &owner = ctx->tbdy->Entries.ptrw()[bodyIdx];

		for (int i = 0; i < owner.members.size(); i++)
		{
			if (ctx->fst->Strings[owner.members[i].nameIndex] != name)
				continue;

			HavokFieldEntry memb = owner.members[i];
			HavokTypeBodyEntry::Kind fkind = HavokTypeBodyEntry::Kind::VOID;
			HavokTypeBodyEntry fbody;
			bool has_body = HavokTag::ResolveTypeKind(memb.typeIndex, ctx->tna, ctx->tbdy, fkind, fbody);
			uint32_t field_off = has_body ? fbody.AlignUp(offset + memb.offset) : (offset + memb.offset);

			HavokCursor result;
			result.ctx = ctx;
			result.typeIdx = memb.typeIndex;
			result.kind = fkind;
			result.body = fbody;

			ctx->data->Buffer->seek(field_off);

			if (fkind == HavokTypeBodyEntry::Kind::RECORD)
			{
				result.offset = field_off;
				result.valid = true;
			}
			else if (fkind == HavokTypeBodyEntry::Kind::POINTER)
			{
				uint32_t ptr = ctx->data->Buffer->get_u32();
				if (ptr == 0 || ptr >= (uint32_t)ctx->item->Entries.size())
					return HavokCursor();
				const auto &target = ctx->item->Entries[ptr];
				result.typeIdx = target.typeIndex;
				result.offset = target.offset;
				result.valid = HavokTag::ResolveTypeKind(target.typeIndex, ctx->tna, ctx->tbdy, result.kind, result.body);
			}
			else if (fkind == HavokTypeBodyEntry::Kind::ARRAY)
			{
				bool inline_arr = has_body && (fbody.format & 0x20) != 0;
				if (inline_arr)
				{
					HavokTypeBodyEntry::Kind ekind; HavokTypeBodyEntry ebody;
					uint32_t estride = HavokTag::ResolveTypeKind(fbody.subtype, ctx->tna, ctx->tbdy, ekind, ebody) ? ebody.size : 0;
					result.arrCount = (estride > 0) ? fbody.size / estride : 0;
					result.arrElemType = fbody.subtype;
					result.arrStride = estride;
					result.arrOffset = field_off;
					result.valid = true;
				}
				else
				{
					uint32_t arrIdx = ctx->data->Buffer->get_u32();
					if (arrIdx == 0 || arrIdx >= (uint32_t)ctx->item->Entries.size())
						return HavokCursor();
					const auto &target = ctx->item->Entries[arrIdx];
					HavokTypeBodyEntry::Kind ekind; HavokTypeBodyEntry ebody;
					uint32_t estride = HavokTag::ResolveTypeKind(target.typeIndex, ctx->tna, ctx->tbdy, ekind, ebody) ? ebody.size : 0;
					result.arrCount = target.count;
					result.arrElemType = target.typeIndex;
					result.arrStride = estride;
					result.arrOffset = target.offset;
					result.valid = true;
				}
			}
			else
			{
				result.offset = field_off;
				result.valid = true;
			}
			return result;
		}
		curType = owner.parentIndex;
	}
	return out;
}

HavokCursor HavokCursor::operator[](uint32_t i) const
{
	HavokCursor out;
	if (!valid || kind != HavokTypeBodyEntry::Kind::ARRAY || i >= arrCount) return out;

	out.ctx = ctx;
	out.typeIdx = arrElemType;
	out.offset = arrOffset + i * arrStride;
	out.valid = HavokTag::ResolveTypeKind(arrElemType, ctx->tna, ctx->tbdy, out.kind, out.body);
	return out;
}

int64_t HavokCursor::AsInt() const
{
	if (!valid) return 0;
	ctx->data->Buffer->seek(offset);
	uint32_t bytes = MAX<uint32_t>(1, (body.format >> 10) / 8);
	bool big_endian = (body.format & 0x100) != 0;
	bool is_signed  = (body.format & 0x200) != 0;
	uint64_t raw = 0;
	for (uint32_t b = 0; b < bytes; b++)
	{
		uint8_t byte = ctx->data->Buffer->get_u8();
		if (big_endian) raw = (raw << 8) | byte;
		else raw |= (uint64_t)byte << (8 * b);
	}
	if (is_signed && bytes < 8)
	{
		uint64_t sign_bit = 1ULL << (bytes * 8 - 1);
		if (raw & sign_bit) raw |= ~((1ULL << (bytes * 8)) - 1);
	}
	return (int64_t)raw;
}

double HavokCursor::AsFloat() const
{
	if (!valid) return 0.0;
	ctx->data->Buffer->seek(offset);
	return body.size == 8 ? ctx->data->Buffer->get_double() : ctx->data->Buffer->get_float();
}

bool HavokCursor::AsBool() const
{
	if (!valid) return false;
	ctx->data->Buffer->seek(offset);
	return ctx->data->Buffer->get_u8() == 1;
}

String HavokCursor::AsString() const
{
	if (!valid) return "";
	ctx->data->Buffer->seek(offset);
	uint32_t str_idx = ctx->data->Buffer->get_u32();
	if (str_idx == 0 || str_idx >= (uint32_t)ctx->item->Entries.size())
		return "";
	ctx->data->Buffer->seek(ctx->item->Entries[str_idx].offset);
	return Utils::read_null_terminated_string(ctx->data->Buffer);
}

// ---------------- Mesh extraction ----------------

Vector<HavokMeshSection> HavokTag::GetGeometrySections()
{
	Vector<HavokMeshSection> sections;

	HavokContext ctx = BuildContext();
	ERR_FAIL_COND_V_MSG(!ctx.IsValid(), sections, "Failed to build Havok context (missing sections)");

	int32_t meshIdx = FindItemByTypeName(ctx.item, ctx.tna, ctx.tst, "hknpMeshShape");
	ERR_FAIL_COND_V_MSG(meshIdx < 0, sections, "hknpMeshShape not found in ITEM");

	HavokCursor mesh = Root((uint32_t)meshIdx, ctx);
	ERR_FAIL_COND_V_MSG(mesh.IsNull(), sections, "Failed to resolve hknpMeshShape item");

	HavokCursor vcu = mesh.Field("vertexConversionUtil");
	HavokCursor bitScale16Inv = vcu.Field("bitScale16Inv");
	float scale_x = 0.0f, scale_y = 0.0f, scale_z = 0.0f;
	if (!bitScale16Inv.IsNull())
	{
		ctx.data->Buffer->seek(bitScale16Inv.offset);
		scale_x = ctx.data->Buffer->get_float();
		scale_y = ctx.data->Buffer->get_float();
		scale_z = ctx.data->Buffer->get_float();
	}

	HavokCursor geoSections = mesh.Field("geometrySections");
	if (geoSections.IsNull())
		return sections;

	for (uint32_t s = 0; s < geoSections.Count(); s++)
	{
		HavokCursor sec = geoSections[s];

		HavokCursor prims = sec.Field("primitives");
		HavokCursor vertexBuffer = sec.Field("vertexBuffer");
		HavokCursor sectionOffset = sec.Field("sectionOffset");

		if (prims.IsNull() || vertexBuffer.IsNull() || sectionOffset.IsNull())
			continue;

		int32_t offset_x = (int32_t)sectionOffset[0].AsInt();
		int32_t offset_y = (int32_t)sectionOffset[1].AsInt();
		int32_t offset_z = (int32_t)sectionOffset[2].AsInt();

		bool isQuantized = (uint32_t)offset_x != 0x7FFFFFFFu;

		HavokMeshSection out;

		if (isQuantized)
		{
			uint32_t vertexCount = vertexBuffer.arrCount / 6;
			out.vertices.resize(vertexCount);

			for (uint32_t i = 0; i < vertexCount; i++)
			{
				ctx.data->Buffer->seek(vertexBuffer.arrOffset + i * 6);
				uint16_t rx = ctx.data->Buffer->get_u16();
				uint16_t ry = ctx.data->Buffer->get_u16();
				uint16_t rz = ctx.data->Buffer->get_u16();

				float px = (float)((int32_t)rx + offset_x) * scale_x;
				float py = (float)((int32_t)ry + offset_y) * scale_y;
				float pz = (float)((int32_t)rz + offset_z) * scale_z;

				out.vertices[i] = Vector3(px, py, pz);
			}
		}
		else
		{
			uint32_t vertexCount = vertexBuffer.arrCount / 12;
			out.vertices.resize(vertexCount);

			for (uint32_t i = 0; i < vertexCount; i++)
			{
				ctx.data->Buffer->seek(vertexBuffer.arrOffset + i * 12);
				float x = ctx.data->Buffer->get_float();
				float y = ctx.data->Buffer->get_float();
				float z = ctx.data->Buffer->get_float();
				out.vertices[i] = Vector3(x, y, z);
			}
		}

		uint32_t vertexCount = out.vertices.size();
		for (uint32_t p = 0; p < prims.Count(); p++)
		{
			HavokCursor prim = prims[p];
			uint32_t a = (uint32_t)prim.Field("aId").AsInt();
			uint32_t b = (uint32_t)prim.Field("bId").AsInt();
			uint32_t c = (uint32_t)prim.Field("cId").AsInt();

			if (a >= vertexCount || b >= vertexCount || c >= vertexCount)
				continue;

			out.faceIndices.push_back((int32_t)a);
			out.faceIndices.push_back((int32_t)b);
			out.faceIndices.push_back((int32_t)c);
		}

		sections.push_back(out);
	}

	return sections;
}