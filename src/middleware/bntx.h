#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>
#include "utils.h"
#include <godot_cpp/variant/vector4i.hpp>

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

	static Error Read(const Ref<StreamPeerBuffer> &sp, BNTXHeader &out)
	{
		ERR_FAIL_COND_V(sp.is_null(), ERR_INVALID_PARAMETER);

		BNTXHeader result;
		result.Magic = sp->get_string(8);
		result.Version = sp->get_32();
		result.unk_1 = sp->get_16();
		result.Revision = sp->get_16();
		result.FilenameAddr = sp->get_32();
		result.unk_2 = sp->get_16();
		result.StringAddr = sp->get_16();
		result.RelocAddr = sp->get_32();
		result.FileSize = sp->get_32();

		out = result;
		return OK;
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

	static Error Read(const Ref<StreamPeerBuffer> &sp, NXHeader &out)
	{
		ERR_FAIL_COND_V(sp.is_null(), ERR_INVALID_PARAMETER);

		NXHeader result;
		result.Magic = sp->get_string(4);
		result.Count = sp->get_32();
		result.InfoPtrAddr = sp->get_64();
		result.DataBlkAddr = sp->get_64();
		result.DictAddr = sp->get_64();
		result.StrDictSize = sp->get_32();

		out = result;
		return OK;
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

	static Error Read(const Ref<StreamPeerBuffer> &sp, BRTInfo &out)
	{
		ERR_FAIL_COND_V(sp.is_null(), ERR_INVALID_PARAMETER);

		BRTInfo result;
		result.Magic = sp->get_string(4);
		result.Size = sp->get_32();
		result.OffsetToData = sp->get_64();
		result.TileMode = sp->get_8();
		result.DIM = sp->get_8();
		result.Flags = sp->get_16();
		result.Swizzle = sp->get_16();
		result.MipsCount = sp->get_16();
		result.NumMultiSample = sp->get_32();
		result.Format = sp->get_32();
		result.GPUAccessFlags = sp->get_32();
		result.Width = sp->get_32();
		result.Height = sp->get_32();
		result.Depth = sp->get_32();
		result.ArrayLength = sp->get_32();
		result.SizeRange = sp->get_32();
		result.unk38 = sp->get_32();
		result.unk3C = sp->get_32();
		result.unk40 = sp->get_32();
		result.unk44 = sp->get_32();
		result.unk48 = sp->get_32();
		result.unk4C = sp->get_32();
		result.DataSize = sp->get_32();
		result.Alignment = sp->get_32();
		result.ChannelType = sp->get_32();
		result.Type = sp->get_32();
		result.NameOffset = sp->get_64();
		result.ParentOffset = sp->get_64();
		result.MipMapArrayPtr = sp->get_64();
		result.UserDataPtr = sp->get_64();
		result.TexturePtr = sp->get_64();
		result.TextureViewPtr = sp->get_64();
		result.UserDescriptorSlot = sp->get_64();
		result.UserDataDicPtr = sp->get_64();

		out = result;
		return OK;
	}
};

struct BRTData
{
	String Magic;
	uint64_t FileSize;

	static Error Read(const Ref<StreamPeerBuffer> &sp, BRTData &out)
	{
		ERR_FAIL_COND_V(sp.is_null(), ERR_INVALID_PARAMETER);

		BRTData result;
		result.Magic = sp->get_string(8);
		result.FileSize = sp->get_64();

		out = result;
		return OK;
	}
};

class BinaryTexture : public Image {
    GDCLASS(BinaryTexture, Image)

protected:
	static void _bind_methods();
public:
	BinaryTexture(){}
	~BinaryTexture(){}

    Error LoadFromEntry(Ref<StreamPeerBuffer> sp, uint64_t info_offset, int layer = 0);
    Vector4i get_channel_sources() const;

private:
    Vector4i channel_sources = Vector4i(2, 3, 4, 5);
	Image::Format GetGodotImageFormat(int bntx_format);
	PackedByteArray Swizzle(uint32_t width, uint32_t height, BRTInfo info, PackedByteArray data, bool toSwizzle);
	uint32_t getAddrBlockLinear(uint32_t x, uint32_t y, uint32_t image_width, uint32_t bytes_per_pixel, uint32_t base_address, uint32_t block_height);
};

class BinaryTextureArchive : public Resource {
    GDCLASS(BinaryTextureArchive, Resource)
protected:
    static void _bind_methods();
public:
    Error LoadFromFile(const String &path);
    Error LoadFromBuffer(const PackedByteArray &buffer);
    TypedDictionary<String, BinaryTexture> get_textures() const { return textures.duplicate(); }
    Ref<BinaryTexture> GetTexture(const String &name) const;
private:
    TypedDictionary<String, BinaryTexture> textures;
};

class ResourceFormatLoaderBNTX : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderBNTX, ResourceFormatLoader)
protected:
	static void _bind_methods(){}
public:
	ResourceFormatLoaderBNTX(){}
	~ResourceFormatLoaderBNTX(){}

	virtual PackedStringArray _get_recognized_extensions() const override;
    String _get_resource_type(const String &path) const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

}
