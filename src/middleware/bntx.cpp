#include "bntx.h"
#include <cstring>
#include <vector>

using namespace godot;

void BinaryTexture::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_channel_sources"), &BinaryTexture::get_channel_sources);
    ClassDB::bind_method(D_METHOD("get_layer_image", "layer"), &BinaryTexture::get_layer_image);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR4I, "channel_sources", PROPERTY_HINT_NONE, "",
                              PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY),
                 "", "get_channel_sources");
}

void BinaryTextureArchive::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &BinaryTextureArchive::LoadFromFile);
    ClassDB::bind_method(D_METHOD("LoadFromBuffer", "buffer"), &BinaryTextureArchive::LoadFromBuffer);
    ClassDB::bind_method(D_METHOD("get_textures"), &BinaryTextureArchive::get_textures);
    ClassDB::bind_method(D_METHOD("GetTexture", "name"), &BinaryTextureArchive::GetTexture);
    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "textures", PROPERTY_HINT_DICTIONARY_TYPE, "String;BinaryTexture",
                              PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY),
                 "", "get_textures");
}

Vector4i BinaryTexture::get_channel_sources() const
{
    return channel_sources;
}

Ref<Image> BinaryTexture::get_layer_image(int layer) const
{
    if (layer < 0 || layer >= layer_images.size())
    {
        return Ref<Image>();
    }
    return layer_images[layer];
}

std::pair<int, int> bpps[] = {{0x0b, 0x04}, {0x07, 0x02}, {0x02, 0x01}, {0x09, 0x02}, {0x14, 0x04}, {0x15, 0x08}, {0x1a, 0x08},
                              {0x1b, 0x10}, {0x1c, 0x10}, {0x1d, 0x08}, {0x1e, 0x10}, {0x1f, 0x10}, {0x20, 0x10},
                              {0x2d, 0x10}, {0x2e, 0x10}, {0x2f, 0x10}, {0x30, 0x10}, {0x31, 0x10}, {0x32, 0x10},
                              {0x33, 0x10}, {0x34, 0x10}, {0x35, 0x10}, {0x36, 0x10}, {0x37, 0x10}, {0x38, 0x10},
                              {0x39, 0x10}, {0x3a, 0x10}};

std::pair<int, std::pair<int, int>> blk_dims[] = {
    {0x1a, {4, 4}},   {0x1b, {4, 4}},   {0x1c, {4, 4}},  {0x1d, {4, 4}},  {0x1e, {4, 4}},  {0x1f, {4, 4}},
    {0x20, {4, 4}},   {0x2d, {4, 4}},   {0x2e, {5, 4}},  {0x2f, {5, 5}},  {0x30, {6, 5}},  {0x31, {6, 6}},
    {0x32, {8, 5}},   {0x33, {8, 6}},   {0x34, {8, 8}},  {0x35, {10, 5}}, {0x36, {10, 6}}, {0x37, {10, 8}},
    {0x38, {10, 10}}, {0x39, {12, 10}}, {0x3a, {12, 12}}};

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ROUND_UP(x, y) (((x - 1) | (y - 1)) + 1)

PackedByteArray BinaryTexture::Swizzle(uint32_t width, uint32_t height, BRTInfo info, PackedByteArray data,
                                       bool toSwizzle)
{
    uint32_t bpp = 4;
    for (int i = 0; i < (int)(sizeof(bpps) / sizeof(bpps[0])); i++)
    {
        if (bpps[i].first == (info.Format >> 8))
        {
            bpp = bpps[i].second;
            break;
        }
    }

    uint32_t blkWidth = 1, blkHeight = 1;
    for (int i = 0; i < (int)(sizeof(blk_dims) / sizeof(blk_dims[0])); i++)
    {
        if (blk_dims[i].first == (info.Format >> 8))
        {
            blkWidth = blk_dims[i].second.first;
            blkHeight = blk_dims[i].second.second;
            break;
        }
    }

    width = DIV_ROUND_UP(width, blkWidth);
    height = DIV_ROUND_UP(height, blkHeight);

    uint32_t block_height = 1u << info.SizeRange;
    {
        uint32_t height_in_gobs = DIV_ROUND_UP(height, 8);
        while (block_height > 1 && (block_height >> 1) >= height_in_gobs)
        {
            block_height >>= 1;
        }
    }

    uint32_t pitch, surfSize;
    if (info.TileMode == 1)
    {
        pitch = ROUND_UP(width * bpp, 32);
        surfSize = ROUND_UP(pitch * height, info.Alignment);
    }
    else
    {
        pitch = ROUND_UP(width * bpp, 64);
        surfSize = ROUND_UP(pitch * ROUND_UP(height, block_height * 8), info.Alignment);
    }

    uint32_t linearSize = width * height * bpp;

    PackedByteArray result;
    result.resize(toSwizzle ? surfSize : linearSize);
    uint8_t* destination = result.ptrw();
    const uint8_t* source = data.ptr();

    uint32_t dataSize = (uint32_t)data.size();

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            uint32_t linear = (y * width + x) * bpp;
            uint32_t swizzled;

            if (info.TileMode == 1)
            {
                swizzled = y * pitch + x * bpp;
            }
            else
            {
                swizzled = getAddrBlockLinear(x, y, width, bpp, 0, block_height);
            }

            if (swizzled + bpp > surfSize)
            {
                continue;
            }
            if (linear + bpp > linearSize)
            {
                continue;
            }

            if (toSwizzle)
            {
                if (linear + bpp > dataSize)
                {
                    continue;
                }
                std::memcpy(destination + swizzled, source + linear, bpp);
            }
            else
            {
                ERR_FAIL_COND_V(swizzled + bpp > dataSize, PackedByteArray());
                std::memcpy(destination + linear, source + swizzled, bpp);
            }
        }
    }

    return result;
}

uint32_t BinaryTexture::getAddrBlockLinear(uint32_t x, uint32_t y, uint32_t image_width, uint32_t bytes_per_pixel,
                                           uint32_t base_address, uint32_t block_height)
{
    uint32_t image_width_in_gobs = DIV_ROUND_UP(image_width * bytes_per_pixel, 64);

    uint32_t GOB_address = (base_address + (y / (8 * block_height)) * 512 * block_height * image_width_in_gobs +
                            (x * bytes_per_pixel / 64) * 512 * block_height + (y % (8 * block_height) / 8) * 512);

    x *= bytes_per_pixel;

    uint32_t Address =
        (GOB_address + ((x % 64) / 32) * 256 + ((y % 8) / 2) * 64 + ((x % 32) / 16) * 32 + (y % 2) * 16 + (x % 16));
    return Address;
}

Image::Format BinaryTexture::GetGodotImageFormat(int bntx_format)
{
    Image::Format fmt = Image::FORMAT_MAX;
    switch (bntx_format)
    {
        case 0x0b01:
            fmt = Image::FORMAT_RGBA8;
            break;
        case 0x0b06:
            fmt = Image::FORMAT_RGBA8;
            break;
        case 0x0701:
            fmt = Image::FORMAT_RGB565;
            break;
        case 0x0201:
            fmt = Image::FORMAT_L8;
            break;
        case 0x0901:
            fmt = Image::FORMAT_RG8;
            break;
        case 0x1405:
            fmt = Image::FORMAT_RF;
            break;
        case 0x1505:
            fmt = Image::FORMAT_RGBAH;
            break;
        case 0x1a01:
            fmt = Image::FORMAT_DXT1;
            break;
        case 0x1a06:
            fmt = Image::FORMAT_DXT1;
            break;
        case 0x1b01:
            fmt = Image::FORMAT_DXT3;
            break;
        case 0x1b06:
            fmt = Image::FORMAT_DXT3;
            break;
        case 0x1c01:
            fmt = Image::FORMAT_DXT5;
            break;
        case 0x1c06:
            fmt = Image::FORMAT_DXT5;
            break;
        case 0x1d01:
            fmt = Image::FORMAT_RGTC_R;
            break;
        case 0x1d02:
            fmt = Image::FORMAT_RGTC_R;
            break;
        case 0x1e01:
            fmt = Image::FORMAT_RGTC_RG;
            break;
        case 0x1e02:
            fmt = Image::FORMAT_RGTC_RG;
            break;
        case 0x1f0a:
            fmt = Image::FORMAT_BPTC_RGBFU;
            break;
        case 0x1f05:
            fmt = Image::FORMAT_BPTC_RGBF;
            break;
        case 0x2001:
            fmt = Image::FORMAT_BPTC_RGBA;
            break;
        case 0x2006:
            fmt = Image::FORMAT_BPTC_RGBA;
            break;
        case 0x2d01:
            fmt = Image::FORMAT_ASTC_4x4;
            break;
        case 0x2d06:
            fmt = Image::FORMAT_ASTC_4x4;
            break;
        // Godot's Image only has the 4x4 and 8x8 ASTC block sizes.
        case 0x3401:
            fmt = Image::FORMAT_ASTC_8x8;
            break;
        case 0x3406:
            fmt = Image::FORMAT_ASTC_8x8;
            break;
    }

    return fmt;
}

static bool has_bytes(const Ref<StreamPeerBuffer>& sp, uint64_t offset, uint64_t size)
{
    uint64_t end = sp->get_size();
    return offset <= end && size <= end - offset;
}

// A BC1 block with color0 <= color1 is in 3-color mode, where index 3 is
// transparent black.
static bool bc1_has_punch_through(const PackedByteArray& data, int width, int height)
{
    const uint8_t* blocks = data.ptr();
    const int blocks_wide = DIV_ROUND_UP(width, 4);
    for (int64_t i = 0; i + 8 <= data.size(); i += 8)
    {
        const uint16_t color0 = blocks[i] | (blocks[i + 1] << 8);
        const uint16_t color1 = blocks[i + 2] | (blocks[i + 3] << 8);
        if (color0 > color1)
        {
            continue;
        }
        const uint32_t indices =
            blocks[i + 4] | (blocks[i + 5] << 8) | (blocks[i + 6] << 16) | (uint32_t(blocks[i + 7]) << 24);
        const int block = int(i / 8);
        for (int texel = 0; texel < 16; texel++)
        {
            if ((block % blocks_wide) * 4 + texel % 4 >= width ||
                (block / blocks_wide) * 4 + texel / 4 >= height)
            {
                continue;
            }
            if (((indices >> (texel * 2)) & 3) == 3)
            {
                return true;
            }
        }
    }
    return false;
}

Error BinaryTexture::LoadFromEntry(Ref<StreamPeerBuffer> sp, uint64_t info_offset, bool upload_array)
{
    ERR_FAIL_COND_V(!has_bytes(sp, info_offset, 160), ERR_FILE_CORRUPT);
    sp->seek(info_offset);

    BRTInfo info;
    BRTInfo::Read(sp, info);
    ERR_FAIL_COND_V(info.Magic != "BRTI", ERR_FILE_CORRUPT);
    ERR_FAIL_COND_V(!has_bytes(sp, info.NameOffset, 2), ERR_FILE_CORRUPT);
    sp->seek(info.NameOffset);
    uint16_t name_length = sp->get_16();
    ERR_FAIL_COND_V(!has_bytes(sp, info.NameOffset + 2, name_length), ERR_FILE_CORRUPT);
    set_name(sp->get_string(name_length));

    Image::Format format = GetGodotImageFormat(info.Format);
    ERR_FAIL_COND_V_MSG(format == Image::FORMAT_MAX, ERR_UNAVAILABLE,
                        vformat("Unsupported BNTX format 0x%04x for texture '%s'.", info.Format, get_name()));
    ERR_FAIL_COND_V(info.Width <= 0 || info.Height <= 0 || info.Width > 32768 || info.Height > 32768 ||
                        info.SizeRange < 0 || info.SizeRange > 5 || info.Alignment <= 0 ||
                        (info.Alignment & (info.Alignment - 1)) != 0 || info.MipsCount == 0 || info.DataSize <= 0 ||
                        info.ArrayLength <= 0 || info.DataSize % info.ArrayLength != 0,
                    ERR_FILE_CORRUPT);
    ERR_FAIL_COND_V(!has_bytes(sp, info.MipMapArrayPtr, uint64_t(info.MipsCount) * 8), ERR_FILE_CORRUPT);
    sp->seek(info.MipMapArrayPtr);
    int full_mip_count = 1;
    for (int extent = MAX(info.Width, info.Height); extent > 1; extent >>= 1)
    {
        ++full_mip_count;
    }
    ERR_FAIL_COND_V(info.MipsCount > full_mip_count, ERR_FILE_CORRUPT);
    std::vector<uint64_t> mip_offsets(info.MipsCount);
    for (uint64_t& offset : mip_offsets)
    {
        offset = sp->get_64();
    }
    const uint64_t first_mip_start = mip_offsets[0];
    const uint64_t layer_size = uint64_t(info.DataSize) / info.ArrayLength;
    ERR_FAIL_COND_V(!has_bytes(sp, first_mip_start, info.DataSize), ERR_FILE_CORRUPT);
    for (int mip = 0; mip < info.MipsCount; ++mip)
    {
        const uint64_t end = mip + 1 < info.MipsCount ? mip_offsets[mip + 1] : first_mip_start + layer_size;
        ERR_FAIL_COND_V(mip_offsets[mip] < first_mip_start || end <= mip_offsets[mip] ||
                            end - first_mip_start > layer_size,
                        ERR_FILE_CORRUPT);
    }
    // Godot Images require a complete chain. Retain the old base-level fallback
    // for partial chains; the game's multi-level textures have full chains.
    const bool use_mipmaps = info.MipsCount > 1 && info.MipsCount == full_mip_count;
    const int mip_count = use_mipmaps ? info.MipsCount : 1;
    const uint32_t sources = uint32_t(info.ChannelType);
    channel_sources = Vector4i(sources & 0xff, (sources >> 8) & 0xff, (sources >> 16) & 0xff, (sources >> 24) & 0xff);

    TypedArray<Ref<Image>> layers;
    bool punch_through = false;
    for (int layer = 0; layer < info.ArrayLength; ++layer)
    {
        PackedByteArray pixels;
        for (int mip = 0; mip < mip_count; ++mip)
        {
            const uint64_t start = mip_offsets[mip] + layer_size * layer;
            const uint64_t end =
                (mip + 1 < info.MipsCount ? mip_offsets[mip + 1] : first_mip_start + layer_size) + layer_size * layer;
            sp->seek(start);
            Array bytes = sp->get_data(end - start);
            ERR_FAIL_COND_V(bytes.size() < 2 || int(bytes[0]) != OK, ERR_FILE_CORRUPT);
            const int width = MAX(1, info.Width >> mip);
            const int height = MAX(1, info.Height >> mip);
            PackedByteArray level = Swizzle(width, height, info, bytes[1], false);
            ERR_FAIL_COND_V(level.is_empty(), ERR_FILE_CORRUPT);
            if (format == Image::FORMAT_DXT1 && !punch_through)
            {
                punch_through = bc1_has_punch_through(level, width, height);
            }
            pixels.append_array(level);
        }
        Ref<Image> image;
        image.instantiate();
        image->set_data(info.Width, info.Height, use_mipmaps, format, pixels);
        ERR_FAIL_COND_V(image->is_empty(), ERR_FILE_CORRUPT);
        layers.push_back(image);
    }
    // NVN samples BC1 with its 1-bit alpha, but Godot uploads DXT1 as opaque
    // BC1_RGB, so cutout textures need an uncompressed copy to keep alpha.
    if (punch_through)
    {
        for (int layer = 0; layer < layers.size(); ++layer)
        {
            Ref<Image> image = layers[layer];
            image->decompress();
        }
    }

    layer_images = layers;
    if (upload_array)
    {
        Error error = create_from_images(layers);
        ERR_FAIL_COND_V(error != OK, error);
    }
    return OK;
}

Ref<BinaryTexture> BinaryTextureArchive::GetTexture(const String& name) const
{
    return textures.get(name, Ref<BinaryTexture>());
}

Error BinaryTextureArchive::LoadFromFile(const String& path)
{
    textures.clear();
    Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
    ERR_FAIL_COND_V_MSG(file.is_null(), ERR_FILE_CANT_OPEN, "Could not open " + path);
    return LoadFromBuffer(file->get_buffer(file->get_length()));
}

Error BinaryTextureArchive::LoadFromBuffer(const PackedByteArray& buffer)
{
    return load_buffer(buffer, true);
}

Error BinaryTextureArchive::LoadImagesFromBuffer(const PackedByteArray& buffer)
{
    return load_buffer(buffer, false);
}

Error BinaryTextureArchive::load_buffer(const PackedByteArray& buffer, bool upload_arrays)
{
    textures.clear();
    ERR_FAIL_COND_V(buffer.size() < 68, ERR_FILE_CORRUPT);
    Ref<StreamPeerBuffer> sp;
    sp.instantiate();
    sp->set_data_array(buffer);
    BNTXHeader header;
    BNTXHeader::Read(sp, header);
    ERR_FAIL_COND_V(header.Magic != "BNTX" || header.unk_1 != 0xfeff || header.FileSize != uint64_t(buffer.size()),
                    ERR_FILE_CORRUPT);
    NXHeader nx;
    NXHeader::Read(sp, nx);
    ERR_FAIL_COND_V(nx.Magic != "NX  " || !has_bytes(sp, nx.InfoPtrAddr, uint64_t(nx.Count) * 8) ||
                        !has_bytes(sp, nx.DataBlkAddr, 16),
                    ERR_FILE_CORRUPT);
    sp->seek(nx.DataBlkAddr);
    BRTData data;
    BRTData::Read(sp, data);
    ERR_FAIL_COND_V(data.Magic != "BRTD", ERR_FILE_CORRUPT);
    TypedDictionary<String, BinaryTexture> loaded;
    for (uint32_t i = 0; i < nx.Count; ++i)
    {
        sp->seek(nx.InfoPtrAddr + uint64_t(i) * 8);
        uint64_t offset = sp->get_64();
        Ref<BinaryTexture> texture;
        texture.instantiate();
        Error error = texture->LoadFromEntry(sp, offset, upload_arrays);
        if (error != OK)
        {
            return error;
        }
        const String name = texture->get_name();
        ERR_FAIL_COND_V_MSG(loaded.has(name), ERR_FILE_CORRUPT, vformat("Duplicate BNTX texture name '%s'.", name));
        loaded[name] = texture;
    }
    textures = loaded;
    return OK;
}

Variant ResourceFormatLoaderBNTX::_load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                                        int32_t p_cache_mode) const
{
    Ref<BinaryTextureArchive> bntx;
    bntx.instantiate();
    Error error = bntx->LoadFromFile(p_path);
    if (error != OK)
    {
        return error;
    }
    return bntx;
}

PackedStringArray ResourceFormatLoaderBNTX::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("bntx");
    return exts;
}

bool ResourceFormatLoaderBNTX::_handles_type(const StringName& p_type) const
{
    return p_type == StringName("BinaryTextureArchive");
}

String ResourceFormatLoaderBNTX::_get_resource_type(const String& path) const
{
    return path.get_extension().to_lower() == "bntx" ? "BinaryTextureArchive" : "";
}
