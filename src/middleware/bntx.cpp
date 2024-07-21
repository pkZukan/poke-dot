#include "bntx.h"

using namespace godot;

void BinaryTexture::_bind_methods() 
{
    GETTER_SETTER_BIND(BinaryTexture, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BinaryTexture, Width, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BinaryTexture, Height, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BinaryTexture, MipsCount, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BinaryTexture, ImageData, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "Image")
}

std::pair<int, int> bpps[] = {
    {0x0b, 0x04}, {0x07, 0x02}, {0x02, 0x01}, {0x09, 0x02}, {0x1a, 0x08},
    {0x1b, 0x10}, {0x1c, 0x10}, {0x1d, 0x08}, {0x1e, 0x10}, {0x1f, 0x10},
    {0x20, 0x10}, {0x2d, 0x10}, {0x2e, 0x10}, {0x2f, 0x10}, {0x30, 0x10},
    {0x31, 0x10}, {0x32, 0x10}, {0x33, 0x10}, {0x34, 0x10}, {0x35, 0x10},
    {0x36, 0x10}, {0x37, 0x10}, {0x38, 0x10}, {0x39, 0x10}, {0x3a, 0x10}
};

std::pair<int, std::pair<int, int>> blk_dims[] = {
    {0x1a, {4, 4}}, {0x1b, {4, 4}}, {0x1c, {4, 4}},
    {0x1d, {4, 4}}, {0x1e, {4, 4}}, {0x1f, {4, 4}},
    {0x20, {4, 4}}, {0x2d, {4, 4}}, {0x2e, {5, 4}},
    {0x2f, {5, 5}}, {0x30, {6, 5}},
    {0x31, {6, 6}}, {0x32, {8, 5}},
    {0x33, {8, 6}}, {0x34, {8, 8}},
    {0x35, {10, 5}}, {0x36, {10, 6}},
    {0x37, {10, 8}}, {0x38, {10, 10}},
    {0x39, {12, 10}}, {0x3a, {12, 12}}
};

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define ROUND_UP(x, y) (((x - 1) | (y - 1)) + 1)
PackedByteArray BinaryTexture::Swizzle(uint32_t width, uint32_t height, BRTInfo info, PackedByteArray data, bool toSwizzle) {
    uint32_t block_height = 1 << info.SizeRange;
    uint32_t bpp;
    uint32_t blkWidth = 1;
    uint32_t blkHeight = 1;
    for (int i = 0; i < sizeof(bpps) / sizeof(bpps[0]); i++) {
        if (bpps[i].first == (info.Format >> 8)) {
            bpp = bpps[i].second;
            break;
        }
    }
    for (int i = 0; i < sizeof(blk_dims) / sizeof(blk_dims[0]); i++) {
        if (blk_dims[i].first == (info.Format >> 8)) {
            auto dims = blk_dims[i].second;
            blkWidth = dims.first;
            blkHeight = dims.second;
            break;
        }
    }
    
    width = DIV_ROUND_UP(width, blkWidth);
    height = DIV_ROUND_UP(height, blkHeight);

    uint32_t pitch, surfSize;
    if (info.TileMode == 1) {
        pitch = ROUND_UP(width * bpp, 32);
        surfSize = ROUND_UP(pitch * height, info.Alignment);
    } else {
        pitch = ROUND_UP(width * bpp, 64);
        surfSize = ROUND_UP(pitch * ROUND_UP(height, block_height * 8), info.Alignment);
    }

    PackedByteArray result;
    result.resize(surfSize);

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint32_t pos = -1;
            uint32_t pos_ = (y * width + x) * bpp;

            if (info.TileMode == 1)
                pos = y * pitch + x * bpp;
            else
                pos = getAddrBlockLinear(x, y, width, bpp, 0, block_height);

            if (pos + bpp <= surfSize) {
                if (toSwizzle) {
                    for(int i = 0; i < bpp; i++) result.set(pos + i, data[pos_ + i]);
                } else {
                    for(int i = 0; i < bpp; i++) result.set(pos_ + i, data[pos + i]);
                }
            }
        }
    }

    return result;
}

uint32_t BinaryTexture::getAddrBlockLinear(uint32_t x, uint32_t y, uint32_t image_width, uint32_t bytes_per_pixel, uint32_t base_address, uint32_t block_height) 
{
    /*
        From Tega X1 TRM 
                        */
    uint image_width_in_gobs = DIV_ROUND_UP(image_width * bytes_per_pixel, 64);


    uint GOB_address = (base_address
                        + (y / (8 * block_height)) * 512 * block_height * image_width_in_gobs
                        + (x * bytes_per_pixel / 64) * 512 * block_height
                        + (y % (8 * block_height) / 8) * 512);

    x *= bytes_per_pixel;

    uint Address = (GOB_address + ((x % 64) / 32) * 256 + ((y % 8) / 2) * 64
                    + ((x % 32) / 16) * 32 + (y % 2) * 16 + (x % 16));
    return Address;
}

Image::Format BinaryTexture::GetGodotImageFormat(int bntx_format)
{
    Image::Format fmt = Image::FORMAT_RGBA8;
    switch(bntx_format)
    {
        case 0x0b01: fmt = Image::FORMAT_RGBA8; break;
        case 0x0b06: fmt = Image::FORMAT_RGBA8; break;
        case 0x0701: fmt = Image::FORMAT_RGB565; break;
        case 0x0201: fmt = Image::FORMAT_L8; break;
        case 0x0901: fmt = Image::FORMAT_RGH; break;
        case 0x1a01: fmt = Image::FORMAT_DXT1; break;
        case 0x1a06: fmt = Image::FORMAT_DXT1; break;
        case 0x1b01: fmt = Image::FORMAT_DXT3; break;
        case 0x1b06: fmt = Image::FORMAT_DXT3; break;
        case 0x1c01: fmt = Image::FORMAT_DXT5; break;
        case 0x1c06: fmt = Image::FORMAT_DXT5; break;
        case 0x1d01: fmt = Image::FORMAT_RGTC_R; break;
        case 0x1d02: fmt = Image::FORMAT_RGTC_R; break;
        case 0x1e01: fmt = Image::FORMAT_RGTC_RG; break;
        case 0x1e02: fmt = Image::FORMAT_RGTC_RG; break;
        case 0x1f01: fmt = Image::FORMAT_BPTC_RGBFU; break;
        case 0x1f02: fmt = Image::FORMAT_BPTC_RGBF; break;
        case 0x2001: fmt = Image::FORMAT_BPTC_RGBA; break;
        case 0x2006: fmt = Image::FORMAT_BPTC_RGBA; break;
        case 0x2d01: fmt = Image::FORMAT_ASTC_4x4; break;
        case 0x2d06: fmt = Image::FORMAT_ASTC_4x4_HDR; break;
        /*case 0x2e01: fmt = Image::FORMAT_ASTC_5x4; break;
        case 0x2e06: fmt = Image::FORMAT_ASTC_5x4_HDR; break;
        case 0x2f01: fmt = Image::FORMAT_ASTC_5x5; break;
        case 0x2f06: fmt = Image::FORMAT_ASTC_5x5_HDR; break;
        case 0x3001: fmt = Image::FORMAT_ASTC_6x5; break;
        case 0x3006: fmt = Image::FORMAT_ASTC_6x5_HDR; break;
        case 0x3101: fmt = Image::FORMAT_ASTC_6x6; break;
        case 0x3106: fmt = Image::FORMAT_ASTC_6x6_HDR; break;
        case 0x3201: fmt = Image::FORMAT_ASTC_8x5; break;
        case 0x3206: fmt = Image::FORMAT_ASTC_8x5_HDR; break;
        case 0x3301: fmt = Image::FORMAT_ASTC_8x6; break;
        case 0x3306: fmt = Image::FORMAT_ASTC_8x6_HDR; break;*/
        case 0x3401: fmt = Image::FORMAT_ASTC_8x8; break;
        case 0x3406: fmt = Image::FORMAT_ASTC_8x8_HDR; break;
        /*case 0x3501: fmt = Image::FORMAT_ASTC_10x5; break;
        case 0x3506: fmt = Image::FORMAT_ASTC_10x5_HDR; break;
        case 0x3601: fmt = Image::FORMAT_ASTC_10x6; break;
        case 0x3606: fmt = Image::FORMAT_ASTC_10x6_HDR; break;
        case 0x3701: fmt = Image::FORMAT_ASTC_10x8; break;
        case 0x3706: fmt = Image::FORMAT_ASTC_10x8_HDR; break;
        case 0x3801: fmt = Image::FORMAT_ASTC_10x10; break;
        case 0x3806: fmt = Image::FORMAT_ASTC_10x10_HDR; break;
        case 0x3901: fmt = Image::FORMAT_ASTC_12x10; break;
        case 0x3906: fmt = Image::FORMAT_ASTC_12x10_HDR; break;
        case 0x3a01: fmt = Image::FORMAT_ASTC_12x12; break;
        case 0x3a06: fmt = Image::FORMAT_ASTC_12x12_HDR; break;*/
    }

    return fmt;
}

void BinaryTexture::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMBF file: %s", file));

    Ref<StreamPeerBuffer> sp;
    sp.instantiate();
    sp->set_data_array(buf);

    sp->seek(0);
    //BNTX header
    BNTXHeader bntx_hdr(sp);
    ERR_FAIL_COND(bntx_hdr.Magic != "BNTX");

    //NX header
    NXHeader nx_hdr(sp);
    ERR_FAIL_COND(nx_hdr.Magic != "NX  ");

    for(int i = 0; i < nx_hdr.Count; i++)
    {
        sp->seek(nx_hdr.InfoPtrAddr + (i * 8));
        uint64_t info_pos = sp->get_64();
        sp->seek(info_pos);

        //BRTInfo
        BRTInfo brti_hdr(sp);
        ERR_FAIL_COND(brti_hdr.Magic != "BRTI");
        Width = brti_hdr.Width;
        Height = brti_hdr.Height;
        Depth = brti_hdr.Depth;
        MipsCount = brti_hdr.MipsCount;

        //Get image name
        sp->seek(brti_hdr.NameOffset);
        uint16_t nameLen = sp->get_16();
        Name = sp->get_string(nameLen);

        //Get Mipmap pointers
        sp->seek(brti_hdr.MipMapArrayPtr);
        std::vector<uint64_t> Mips;
        for(int j = 0; j < MipsCount; j++)
        {
            Mips.push_back(sp->get_64());
        }

        //BRTD
        sp->seek(nx_hdr.DataBlkAddr);
        BRTData brtd_hdr(sp);
        ERR_FAIL_COND(brtd_hdr.Magic != "BRTD");

        sp->seek(Mips[0]);
        Array data = sp->get_data(brti_hdr.DataSize); //Mips[1] - Mips[0]
        PackedByteArray buffer = PackedByteArray(data[1]);

        Image::Format fmt = GetGodotImageFormat(brti_hdr.Format);

        PackedByteArray unswizzle = Swizzle(Width, Height, brti_hdr, buffer, 0);

        Ref<Image> img;
        img.instantiate();
        img->set_data(Width, Height, false, fmt, unswizzle);

        set_ImageData(img);
    }
}

Variant ResourceFormatLoaderBNTX::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<BinaryTexture> bntx;
    bntx.instantiate();
    bntx->LoadFromFile(p_path);
    return bntx;
}

PackedStringArray ResourceFormatLoaderBNTX::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("bntx");

    return exts;
}

bool ResourceFormatLoaderBNTX::_handles_type(const StringName &p_type) const 
{
    return p_type == String("ImageTexture");
}