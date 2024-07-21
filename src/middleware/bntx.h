#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "utils.h"

namespace godot {

struct BNTXHeader
{
	String Magic;
	uint32_t Version;
	uint16_t unk_1;
	uint16_t Revision;
	uint32_t FilenameAddr;
	uint16_t unk_2;
	uint16_t StringAddr;
	uint32_t RelocAddr;
	uint32_t FileSize;

	BNTXHeader(Ref<StreamPeerBuffer> sp)
	{
		Magic = sp->get_string(8);
		Version = sp->get_32();
		unk_1 = sp->get_16();
		Revision = sp->get_16();
		FilenameAddr = sp->get_32();
		unk_2 = sp->get_16();
		StringAddr = sp->get_16();
		RelocAddr = sp->get_32();
		FileSize = sp->get_32();
	}
};

struct NXHeader
{
	String Magic;
	uint32_t Count;
	uint64_t InfoPtrAddr;
	uint64_t DataBlkAddr;
	uint64_t DictAddr;
	uint32_t StrDictSize;

	NXHeader(Ref<StreamPeerBuffer> sp)
	{
		Magic = sp->get_string(4);
		Count = sp->get_32();
		InfoPtrAddr = sp->get_64();
		DataBlkAddr = sp->get_64();
		DictAddr = sp->get_64();
		StrDictSize = sp->get_32();
	}
};

struct BRTInfo
{
	String Magic;
	uint32_t Size;
	uint64_t OffsetToData;
	uint8_t TileMode;
	uint8_t DIM;
	uint16_t Flags;
	uint16_t Swizzle;
	uint16_t MipsCount;
	uint32_t NumMultiSample;
	uint32_t Format;
	uint32_t GPUAccessFlags;
	int32_t Width;
	int32_t Height;
	int32_t Depth;
	int32_t ArrayLength;
	int32_t SizeRange;
	uint32_t unk38;
	uint32_t unk3C;
	uint32_t unk40;
	uint32_t unk44;
	uint32_t unk48;
	uint32_t unk4C;
	int32_t DataSize;
	int32_t Alignment;
	int32_t ChannelType;
	int32_t Type;
	uint64_t NameOffset;
	uint64_t ParentOffset;
	uint64_t MipMapArrayPtr;
	uint64_t UserDataPtr;
    uint64_t TexturePtr;
    uint64_t TextureViewPtr;
    uint64_t UserDescriptorSlot;
    uint64_t UserDataDicPtr;

	BRTInfo(Ref<StreamPeerBuffer> sp)
	{
		Magic = sp->get_string(4);
		Size = sp->get_32();
		OffsetToData = sp->get_64();
		TileMode = sp->get_8();
		DIM = sp->get_8();
		Flags = sp->get_16();
		Swizzle = sp->get_16();
		MipsCount = sp->get_16();
		NumMultiSample = sp->get_32();
		Format = sp->get_32();
		GPUAccessFlags = sp->get_32();
		Width = sp->get_32();
		Height = sp->get_32();
		Depth = sp->get_32();
		ArrayLength = sp->get_32();
		SizeRange = sp->get_32();
		unk38 = sp->get_32();
		unk3C = sp->get_32();
		unk40 = sp->get_32();
		unk44 = sp->get_32();
		unk48 = sp->get_32();
		unk4C = sp->get_32();
		DataSize = sp->get_32();
		Alignment = sp->get_32();
		ChannelType = sp->get_32();
		Type = sp->get_32();
		NameOffset = sp->get_64();
		ParentOffset = sp->get_64();
		MipMapArrayPtr = sp->get_64();
		UserDataPtr = sp->get_64();
		TexturePtr = sp->get_64();
		TextureViewPtr = sp->get_64();
		UserDescriptorSlot = sp->get_64();
		UserDataDicPtr = sp->get_64();
	}
};

struct BRTData
{
	String Magic;
	uint64_t FileSize;

	BRTData(Ref<StreamPeerBuffer> sp)
	{
		Magic = sp->get_string(8);
		FileSize = sp->get_64();
	}
};

class BinaryTexture : public Resource {
    GDCLASS(BinaryTexture, Resource)

	typedef struct
	{
		uint16_t nx_fmt;
		Image::Format godot_fmt;
		uint8_t bpp;
		uint32_t blk_width;
		uint32_t blk_height;
	} FormatTableEntry;
protected:
	static void _bind_methods();
public:
	BinaryTexture(){}
	~BinaryTexture(){}

    void LoadFromFile(String file);

	GETTER_SETTER_DEFINE(String, Name)
	GETTER_SETTER_DEFINE(int, Width)
	GETTER_SETTER_DEFINE(int, Height)
	GETTER_SETTER_DEFINE(int, Depth)
	GETTER_SETTER_DEFINE(int, MipsCount)
	GETTER_SETTER_DEFINE(Ref<Image>, ImageData)

private:
	Image::Format GetGodotImageFormat(int bntx_format);
	PackedByteArray Swizzle(uint32_t width, uint32_t height, BRTInfo info, PackedByteArray data, bool toSwizzle);
	uint32_t getAddrBlockLinear(uint32_t x, uint32_t y, uint32_t image_width, uint32_t bytes_per_pixel, uint32_t base_address, uint32_t block_height);

	String Name;
	int Width, Height;
	int Depth;
	int MipsCount;
	Ref<Image> ImageData;
};

class ResourceFormatLoaderBNTX : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderBNTX, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderBNTX(){}
	~ResourceFormatLoaderBNTX(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}