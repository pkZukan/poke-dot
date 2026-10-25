#include "wwise.h"
#include "wwise_codebooks.h"
#include <godot_cpp/templates/local_vector.hpp>

using namespace godot;

// WEM -> Ogg Vorbis conversion is a port of ww2ogg (https://github.com/hcs64/ww2ogg),
// limited to the modern layout (Wwise 2012+): vorb data inside a 0x42 fmt chunk,
// 2-byte packet headers and external aoTuV 6.03 codebooks.
// PTADPCM decoding matches the Wwise SDK's PtADPCM_Decode.cpp.

void WwisePackage::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &WwisePackage::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_wem_ids"), &WwisePackage::get_wem_ids);
	ClassDB::bind_method(D_METHOD("has_wem", "wem_id"), &WwisePackage::has_wem);
	ClassDB::bind_method(D_METHOD("get_wem_data", "wem_id", "max_size"), &WwisePackage::get_wem_data, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_stream", "wem_id"), &WwisePackage::get_stream);
	ClassDB::bind_method(D_METHOD("get_bank_ids"), &WwisePackage::get_bank_ids);
	ClassDB::bind_method(D_METHOD("get_bank_data", "bank_id"), &WwisePackage::get_bank_data);
}

void AudioStreamWwise::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_wem_id"), &AudioStreamWwise::get_wem_id);
	ClassDB::bind_method(D_METHOD("get_decoded_stream"), &AudioStreamWwise::get_decoded_stream);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "wem_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_wem_id");
}

void WwiseSoundBank::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "path"), &WwiseSoundBank::LoadFromFile);
	ClassDB::bind_method(D_METHOD("LoadFromBuffer", "buffer"), &WwiseSoundBank::LoadFromBuffer);
	ClassDB::bind_method(D_METHOD("add_package", "package"), &WwiseSoundBank::add_package);
	ClassDB::bind_method(D_METHOD("get_bank_version"), &WwiseSoundBank::get_bank_version);
	ClassDB::bind_method(D_METHOD("get_bank_id"), &WwiseSoundBank::get_bank_id);
	ClassDB::bind_method(D_METHOD("get_streams"), &WwiseSoundBank::get_streams);
	ClassDB::bind_method(D_METHOD("get_stream", "wem_id"), &WwiseSoundBank::get_stream);
	ClassDB::bind_method(D_METHOD("get_wem_ids"), &WwiseSoundBank::get_wem_ids);
	ClassDB::bind_method(D_METHOD("get_wem_data", "wem_id"), &WwiseSoundBank::get_wem_data);
	ClassDB::bind_method(D_METHOD("get_wem_codec", "wem_id"), &WwiseSoundBank::get_wem_codec);
	ClassDB::bind_method(D_METHOD("is_streamed", "wem_id"), &WwiseSoundBank::is_streamed);
	ClassDB::bind_static_method("WwiseSoundBank", D_METHOD("load_wem", "wem"), &WwiseSoundBank::load_wem);
	ClassDB::bind_static_method("WwiseSoundBank", D_METHOD("wem_to_ogg", "wem"), &WwiseSoundBank::wem_to_ogg);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "bank_version", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_bank_version");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bank_id", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_bank_id");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "streams", PROPERTY_HINT_DICTIONARY_TYPE, "int;AudioStream",
		PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY), "", "get_streams");

	BIND_ENUM_CONSTANT(CODEC_PCM);
	BIND_ENUM_CONSTANT(CODEC_PCM_AUTHORING);
	BIND_ENUM_CONSTANT(CODEC_VORBIS);
	BIND_ENUM_CONSTANT(CODEC_OPUS_NX);
	BIND_ENUM_CONSTANT(CODEC_OPUS);
	BIND_ENUM_CONSTANT(CODEC_OPUS_WEM);
	BIND_ENUM_CONSTANT(CODEC_PTADPCM);
}

void AudioStreamPreviewControl::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_on_play_pressed"), &AudioStreamPreviewControl::_on_play_pressed);
	ClassDB::bind_method(D_METHOD("_on_finished"), &AudioStreamPreviewControl::_on_finished);
}

namespace
{

// Headers of every retail WEM fit in the first 0x62e bytes; this leaves plenty of room.
constexpr uint32_t WEM_HEADER_READ_SIZE = 0x10000;

uint16_t read_u16(const uint8_t *p)
{
	return uint16_t(p[0] | (p[1] << 8));
}

uint32_t read_u32(const uint8_t *p)
{
	return uint32_t(p[0] | (p[1] << 8) | (p[2] << 16) | (uint32_t(p[3]) << 24));
}

void write_u32(uint8_t *p, uint32_t v)
{
	p[0] = uint8_t(v);
	p[1] = uint8_t(v >> 8);
	p[2] = uint8_t(v >> 16);
	p[3] = uint8_t(v >> 24);
}

uint32_t ilog(uint32_t v)
{
	uint32_t bits = 0;
	while (v)
	{
		bits++;
		v >>= 1;
	}
	return bits;
}

uint32_t book_maptype1_quantvals(uint32_t entries, uint32_t dimensions)
{
	const int bits = ilog(entries);
	int vals = entries >> ((bits - 1) * (dimensions - 1) / dimensions);
	while (true)
	{
		uint64_t acc = 1;
		uint64_t acc1 = 1;
		for (uint32_t i = 0; i < dimensions; i++)
		{
			acc *= vals;
			acc1 *= vals + 1;
		}
		if (acc <= entries && acc1 > entries)
			return vals;
		vals += acc > entries ? -1 : 1;
	}
}

// LSB-first bit reader, as Vorbis packs its bitstream.
struct BitReader
{
	const uint8_t *data;
	uint64_t size_bits;
	uint64_t pos = 0;
	bool overflow = false;

	BitReader(const uint8_t *p_data, uint64_t p_size) :
			data(p_data), size_bits(p_size * 8) {}

	uint32_t read(uint32_t bits)
	{
		uint32_t v = 0;
		for (uint32_t i = 0; i < bits; i++)
		{
			if (pos >= size_bits)
			{
				overflow = true;
				return v;
			}
			if ((data[pos >> 3] >> (pos & 7)) & 1)
				v |= 1u << i;
			pos++;
		}
		return v;
	}
};

struct BitWriter
{
	LocalVector<uint8_t> bytes;
	uint64_t accum = 0;
	uint32_t count = 0;

	void write(uint32_t value, uint32_t bits)
	{
		if (bits < 32)
			value &= (1u << bits) - 1;
		accum |= uint64_t(value) << count;
		count += bits;
		while (count >= 8)
		{
			bytes.push_back(uint8_t(accum));
			accum >>= 8;
			count -= 8;
		}
	}

	// Writes the next `bits` of the input through unchanged and returns them.
	uint32_t copy(BitReader &in, uint32_t bits)
	{
		const uint32_t v = in.read(bits);
		write(v, bits);
		return v;
	}

	void flush()
	{
		if (count)
			bytes.push_back(uint8_t(accum));
		accum = 0;
		count = 0;
	}

	void clear()
	{
		bytes.clear();
		accum = 0;
		count = 0;
	}
};

void write_vorbis_header(BitWriter &out, uint8_t type)
{
	out.write(type, 8);
	for (const char c : { 'v', 'o', 'r', 'b', 'i', 's' })
		out.write(uint8_t(c), 8);
}

struct OggWriter
{
	LocalVector<uint8_t> out;
	uint32_t sequence = 0;

	// Writes one packet starting on a fresh page, spilling onto continued pages past 255 segments.
	void write_packet(const LocalVector<uint8_t> &packet, int64_t granule, bool last)
	{
		uint32_t offset = 0;
		bool continued = false;
		do
		{
			const uint32_t remaining = packet.size() - offset;
			const bool completes = remaining / 255 + 1 <= 255;
			const uint32_t segments = completes ? remaining / 255 + 1 : 255;
			const uint32_t payload = completes ? remaining : 255 * 255;
			const uint64_t page_granule = completes ? uint64_t(granule) : ~uint64_t(0);

			const uint32_t start = out.size();
			out.resize(start + 27 + segments + payload);
			uint8_t *page = out.ptr() + start;
			memcpy(page, "OggS", 4);
			page[4] = 0;
			page[5] = (continued ? 1 : 0) | (sequence == 0 ? 2 : 0) | (last && completes ? 4 : 0);
			write_u32(page + 6, uint32_t(page_granule));
			write_u32(page + 10, uint32_t(page_granule >> 32));
			write_u32(page + 14, 1); // serial
			write_u32(page + 18, sequence++);
			write_u32(page + 22, 0); // crc placeholder
			page[26] = uint8_t(segments);
			for (uint32_t i = 0; i < segments; i++)
				page[27 + i] = uint8_t(MIN(255u, payload - i * 255));
			if (payload)
				memcpy(page + 27 + segments, packet.ptr() + offset, payload);
			write_u32(page + 22, crc(page, 27 + segments + payload));

			offset += payload;
			continued = true;
		} while (offset < packet.size());
	}

	static uint32_t crc(const uint8_t *data, uint32_t size)
	{
		static const LocalVector<uint32_t> table = []()
		{
			LocalVector<uint32_t> t;
			t.resize(256);
			for (uint32_t i = 0; i < 256; i++)
			{
				uint32_t r = i << 24;
				for (int j = 0; j < 8; j++)
					r = (r & 0x80000000) ? (r << 1) ^ 0x04c11db7 : (r << 1);
				t[i] = r;
			}
			return t;
		}();

		uint32_t value = 0;
		for (uint32_t i = 0; i < size; i++)
			value = (value << 8) ^ table[((value >> 24) & 0xff) ^ data[i]];
		return value;
	}
};

struct WemInfo
{
	uint16_t format = 0;
	uint16_t channels = 0;
	uint32_t sample_rate = 0;
	uint32_t avg_bytes_per_second = 0;
	uint16_t block_align = 0;
	uint16_t bits_per_sample = 0;
	uint32_t fmt_offset = 0;
	uint32_t fmt_size = 0;
	uint32_t data_offset = 0;
	uint32_t data_size = 0;          // bytes available, clamped to the buffer
	uint32_t declared_data_size = 0; // bytes the full file holds
	bool truncated = false;          // prefetch copy or header-only read
	bool has_loop = false;
	uint32_t loop_start = 0;
	uint32_t loop_end = 0;
};

Error parse_wem(const uint8_t *d, uint32_t size, WemInfo &info)
{
	ERR_FAIL_COND_V_MSG(size < 12, ERR_FILE_CORRUPT, "WEM too small");
	ERR_FAIL_COND_V_MSG(memcmp(d, "RIFX", 4) == 0, ERR_UNAVAILABLE, "Big-endian WEM (RIFX) is not supported");
	ERR_FAIL_COND_V_MSG(memcmp(d, "RIFF", 4) != 0 || memcmp(d + 8, "WAVE", 4) != 0, ERR_FILE_CORRUPT, "Missing RIFF/WAVE header");

	bool has_fmt = false;
	bool has_data = false;
	// Wwise chunks are not padded to even sizes
	for (uint64_t offset = 12; offset + 8 <= size && !has_data; offset += 8 + uint64_t(read_u32(d + offset + 4)))
	{
		const uint8_t *chunk = d + offset;
		const uint32_t chunk_size = read_u32(chunk + 4);
		if (memcmp(chunk, "fmt ", 4) == 0)
		{
			info.fmt_offset = uint32_t(offset + 8);
			info.fmt_size = chunk_size;
			has_fmt = true;
		}
		else if (memcmp(chunk, "data", 4) == 0)
		{
			info.data_offset = uint32_t(offset + 8);
			info.declared_data_size = chunk_size;
			has_data = true;
		}
		else if (memcmp(chunk, "smpl", 4) == 0 && chunk_size >= 0x34 && offset + 8 + 0x34 <= size)
		{
			// single forward loop
			const uint8_t *smpl = chunk + 8;
			if (read_u32(smpl + 0x1c) == 1 && read_u32(smpl + 0x28) == 0)
			{
				info.has_loop = true;
				info.loop_start = read_u32(smpl + 0x2c);
				info.loop_end = read_u32(smpl + 0x30) + 1;
			}
		}
	}
	ERR_FAIL_COND_V_MSG(!has_fmt || !has_data, ERR_FILE_CORRUPT, "WEM is missing fmt or data chunk");
	ERR_FAIL_COND_V_MSG(info.fmt_size < 0x10 || uint64_t(info.fmt_offset) + info.fmt_size > size, ERR_FILE_CORRUPT, "Bad WEM fmt chunk");

	const uint8_t *fmt = d + info.fmt_offset;
	info.format = read_u16(fmt + 0x00);
	info.channels = read_u16(fmt + 0x02);
	info.sample_rate = read_u32(fmt + 0x04);
	info.avg_bytes_per_second = read_u32(fmt + 0x08);
	info.block_align = read_u16(fmt + 0x0c);
	info.bits_per_sample = read_u16(fmt + 0x0e);
	ERR_FAIL_COND_V_MSG(info.channels == 0 || info.sample_rate == 0, ERR_FILE_CORRUPT, "Bad WEM fmt chunk");

	info.truncated = uint64_t(info.data_offset) + info.declared_data_size > size;
	info.data_size = info.truncated ? MAX(size, info.data_offset) - info.data_offset : info.declared_data_size;
	return OK;
}

// Total length in seconds from the header alone, so it also works on prefetch copies and header reads.
double wem_length(const uint8_t *d, const WemInfo &info)
{
	uint64_t samples = 0;
	switch (info.format)
	{
		case WwiseSoundBank::CODEC_VORBIS:
			if (info.fmt_size >= 0x1c)
				samples = read_u32(d + info.fmt_offset + 0x18);
			break;
		case WwiseSoundBank::CODEC_PCM:
		case WwiseSoundBank::CODEC_PCM_AUTHORING:
			if (info.bits_per_sample >= 8)
				samples = info.declared_data_size / (info.bits_per_sample / 8 * info.channels);
			break;
		case WwiseSoundBank::CODEC_PTADPCM:
			if (info.block_align >= 6 * info.channels)
				samples = uint64_t(info.declared_data_size / info.block_align) * (2 + (info.block_align / info.channels - 5) * 2);
			break;
		default:
			break;
	}
	return double(samples) / info.sample_rate;
}

// Expands one codebook from ww2ogg's packed library format into a standard Vorbis codebook.
Error rebuild_codebook(BitReader &in, BitWriter &out)
{
	const uint32_t dimensions = in.read(4);
	const uint32_t entries = in.read(14);
	out.write(0x564342, 24); // "BCV"
	out.write(dimensions, 16);
	out.write(entries, 24);

	const bool ordered = out.copy(in, 1);
	if (ordered)
	{
		out.copy(in, 5); // initial length
		uint32_t current_entry = 0;
		while (current_entry < entries && !in.overflow)
			current_entry += out.copy(in, ilog(entries - current_entry));
		ERR_FAIL_COND_V(current_entry > entries, ERR_FILE_CORRUPT);
	}
	else
	{
		const uint32_t codeword_length_bits = in.read(3);
		const bool sparse = out.copy(in, 1);
		ERR_FAIL_COND_V_MSG(codeword_length_bits == 0 || codeword_length_bits > 5, ERR_FILE_CORRUPT, "Nonsense codeword length");
		for (uint32_t i = 0; i < entries && !in.overflow; i++)
		{
			const bool present = !sparse || out.copy(in, 1);
			if (present)
				out.write(in.read(codeword_length_bits), 5);
		}
	}

	const uint32_t lookup_type = in.read(1);
	out.write(lookup_type, 4);
	if (lookup_type == 1)
	{
		out.copy(in, 32); // min
		out.copy(in, 32); // max
		const uint32_t value_bits = out.copy(in, 4) + 1;
		out.copy(in, 1); // sequence flag
		const uint32_t quantvals = book_maptype1_quantvals(entries, dimensions);
		for (uint32_t i = 0; i < quantvals && !in.overflow; i++)
			out.copy(in, value_bits);
	}

	ERR_FAIL_COND_V_MSG(in.overflow, ERR_FILE_CORRUPT, "Codebook truncated");
	return OK;
}

Error rebuild_external_codebook(uint32_t id, BitWriter &out)
{
	// packed library: codebook data, then a u32 offset table whose start is the final u32
	const uint8_t *library = WWISE_CODEBOOKS_AOTUV603;
	const uint32_t library_size = sizeof(WWISE_CODEBOOKS_AOTUV603);
	const uint32_t table_offset = read_u32(library + library_size - 4);
	const uint32_t count = (library_size - table_offset) / 4;
	ERR_FAIL_COND_V_MSG(id + 1 >= count, ERR_FILE_CORRUPT, vformat("Invalid Vorbis codebook id %d", id));

	const uint32_t start = read_u32(library + table_offset + id * 4);
	const uint32_t end = read_u32(library + table_offset + (id + 1) * 4);
	BitReader in(library + start, end - start);
	return rebuild_codebook(in, out);
}

// Rebuilds the setup packet Wwise strips down to codebook ids and bare floor/residue/mapping/mode data.
Error rebuild_setup(BitReader &in, BitWriter &out, uint32_t channels, LocalVector<bool> &r_mode_blockflag)
{
	write_vorbis_header(out, 5);

	const uint32_t codebook_count = out.copy(in, 8) + 1;
	for (uint32_t i = 0; i < codebook_count; i++)
	{
		const Error err = rebuild_external_codebook(in.read(10), out);
		if (err != OK)
			return err;
	}

	// time domain transforms (placeholder)
	out.write(0, 6);
	out.write(0, 16);

	const uint32_t floor_count = out.copy(in, 6) + 1;
	for (uint32_t i = 0; i < floor_count; i++)
	{
		out.write(1, 16); // always floor type 1
		const uint32_t partitions = out.copy(in, 5);
		LocalVector<uint32_t> partition_class;
		partition_class.resize(partitions);
		uint32_t maximum_class = 0;
		for (uint32_t j = 0; j < partitions; j++)
		{
			partition_class[j] = out.copy(in, 4);
			maximum_class = MAX(maximum_class, partition_class[j]);
		}

		LocalVector<uint32_t> class_dimensions;
		class_dimensions.resize(maximum_class + 1);
		for (uint32_t j = 0; j <= maximum_class; j++)
		{
			class_dimensions[j] = out.copy(in, 3) + 1;
			const uint32_t subclasses = out.copy(in, 2);
			if (subclasses)
				ERR_FAIL_COND_V_MSG(out.copy(in, 8) >= codebook_count, ERR_FILE_CORRUPT, "Invalid floor1 masterbook");
			for (uint32_t k = 0; k < (1u << subclasses); k++)
			{
				const int32_t subclass_book = int32_t(out.copy(in, 8)) - 1;
				ERR_FAIL_COND_V_MSG(subclass_book >= int32_t(codebook_count), ERR_FILE_CORRUPT, "Invalid floor1 subclass book");
			}
		}

		out.copy(in, 2); // multiplier
		const uint32_t range_bits = out.copy(in, 4);
		for (uint32_t j = 0; j < partitions; j++)
		{
			for (uint32_t k = 0; k < class_dimensions[partition_class[j]]; k++)
				out.copy(in, range_bits);
		}
	}

	const uint32_t residue_count = out.copy(in, 6) + 1;
	for (uint32_t i = 0; i < residue_count; i++)
	{
		const uint32_t residue_type = in.read(2);
		out.write(residue_type, 16);
		ERR_FAIL_COND_V_MSG(residue_type > 2, ERR_FILE_CORRUPT, "Invalid residue type");

		out.copy(in, 24); // begin
		out.copy(in, 24); // end
		out.copy(in, 24); // partition size
		const uint32_t classifications = out.copy(in, 6) + 1;
		ERR_FAIL_COND_V_MSG(out.copy(in, 8) >= codebook_count, ERR_FILE_CORRUPT, "Invalid residue classbook");

		LocalVector<uint32_t> cascade;
		cascade.resize(classifications);
		for (uint32_t j = 0; j < classifications; j++)
		{
			const uint32_t low_bits = out.copy(in, 3);
			const uint32_t high_bits = out.copy(in, 1) ? out.copy(in, 5) : 0;
			cascade[j] = high_bits * 8 + low_bits;
		}
		for (uint32_t j = 0; j < classifications; j++)
		{
			for (uint32_t k = 0; k < 8; k++)
			{
				if (cascade[j] & (1u << k))
					ERR_FAIL_COND_V_MSG(out.copy(in, 8) >= codebook_count, ERR_FILE_CORRUPT, "Invalid residue book");
			}
		}
	}

	const uint32_t mapping_count = out.copy(in, 6) + 1;
	for (uint32_t i = 0; i < mapping_count; i++)
	{
		out.write(0, 16); // always mapping type 0
		const uint32_t submaps = out.copy(in, 1) ? out.copy(in, 4) + 1 : 1;
		if (out.copy(in, 1))
		{
			// square polar coupling
			const uint32_t coupling_steps = out.copy(in, 8) + 1;
			const uint32_t channel_bits = ilog(channels - 1);
			for (uint32_t j = 0; j < coupling_steps; j++)
			{
				const uint32_t magnitude = out.copy(in, channel_bits);
				const uint32_t angle = out.copy(in, channel_bits);
				ERR_FAIL_COND_V_MSG(angle == magnitude || magnitude >= channels || angle >= channels, ERR_FILE_CORRUPT, "Invalid coupling");
			}
		}
		ERR_FAIL_COND_V_MSG(out.copy(in, 2) != 0, ERR_FILE_CORRUPT, "Mapping reserved field nonzero");
		if (submaps > 1)
		{
			for (uint32_t j = 0; j < channels; j++)
				ERR_FAIL_COND_V_MSG(out.copy(in, 4) >= submaps, ERR_FILE_CORRUPT, "Invalid mapping mux");
		}
		for (uint32_t j = 0; j < submaps; j++)
		{
			out.copy(in, 8); // time config
			ERR_FAIL_COND_V_MSG(out.copy(in, 8) >= floor_count, ERR_FILE_CORRUPT, "Invalid floor mapping");
			ERR_FAIL_COND_V_MSG(out.copy(in, 8) >= residue_count, ERR_FILE_CORRUPT, "Invalid residue mapping");
		}
	}

	const uint32_t mode_count = out.copy(in, 6) + 1;
	r_mode_blockflag.resize(mode_count);
	for (uint32_t i = 0; i < mode_count; i++)
	{
		r_mode_blockflag[i] = out.copy(in, 1);
		out.write(0, 16); // window type
		out.write(0, 16); // transform type
		ERR_FAIL_COND_V_MSG(out.copy(in, 8) >= mapping_count, ERR_FILE_CORRUPT, "Invalid mode mapping");
	}
	out.write(1, 1); // framing
	out.flush();

	ERR_FAIL_COND_V_MSG(in.overflow, ERR_FILE_CORRUPT, "Vorbis setup packet truncated");
	return OK;
}

Error convert_vorbis(const uint8_t *wem, const WemInfo &info, PackedByteArray &r_ogg)
{
	ERR_FAIL_COND_V_MSG(info.fmt_size != 0x42, ERR_UNAVAILABLE,
		vformat("Unsupported Wwise Vorbis layout (fmt size 0x%x); only Wwise 2012+ WEMs are supported", info.fmt_size));
	ERR_FAIL_COND_V_MSG(read_u16(wem + info.fmt_offset + 0x10) != 0x30, ERR_FILE_CORRUPT, "Bad extra fmt length");

	const uint8_t *vorb = wem + info.fmt_offset + 0x18;
	const uint32_t sample_count = read_u32(vorb + 0x00);
	const uint32_t setup_offset = read_u32(vorb + 0x10);
	const uint32_t audio_offset = read_u32(vorb + 0x14);
	const uint32_t blocksize_0_pow = vorb[0x28];
	const uint32_t blocksize_1_pow = vorb[0x29];
	ERR_FAIL_COND_V_MSG(blocksize_0_pow < 6 || blocksize_1_pow > 13 || blocksize_0_pow > blocksize_1_pow, ERR_FILE_CORRUPT, "Bad Vorbis blocksizes");
	// Wwise strips the packet type and window flags from audio packets, except when both blocksizes match
	const bool mod_packets = blocksize_0_pow != blocksize_1_pow;

	const uint8_t *data = wem + info.data_offset;
	const uint32_t data_size = info.data_size;
	OggWriter ogg;
	ogg.out.reserve(data_size + data_size / 16 + 4096);
	BitWriter packet;

	// identification packet
	write_vorbis_header(packet, 1);
	packet.write(0, 32); // version
	packet.write(info.channels, 8);
	packet.write(info.sample_rate, 32);
	packet.write(0, 32); // bitrate max
	packet.write(info.avg_bytes_per_second * 8, 32);
	packet.write(0, 32); // bitrate min
	packet.write(blocksize_0_pow, 4);
	packet.write(blocksize_1_pow, 4);
	packet.write(1, 1); // framing
	packet.flush();
	ogg.write_packet(packet.bytes, 0, false);

	// comment packet
	static const char vendor[] = "converted from Audiokinetic Wwise by gflib";
	packet.clear();
	write_vorbis_header(packet, 3);
	packet.write(sizeof(vendor) - 1, 32);
	for (uint32_t i = 0; i < sizeof(vendor) - 1; i++)
		packet.write(uint8_t(vendor[i]), 8);
	packet.write(0, 32); // no user comments
	packet.write(1, 1);  // framing
	packet.flush();
	ogg.write_packet(packet.bytes, 0, false);

	// setup packet
	ERR_FAIL_COND_V_MSG(uint64_t(setup_offset) + 2 > data_size, ERR_FILE_CORRUPT, "Setup packet out of range");
	const uint32_t setup_size = read_u16(data + setup_offset);
	ERR_FAIL_COND_V_MSG(uint64_t(setup_offset) + 2 + setup_size > data_size, ERR_FILE_CORRUPT, "Setup packet truncated");
	ERR_FAIL_COND_V_MSG(setup_offset + 2 + setup_size != audio_offset, ERR_FILE_CORRUPT, "First audio packet doesn't follow setup packet");

	LocalVector<bool> mode_blockflag;
	BitReader setup(data + setup_offset + 2, setup_size);
	packet.clear();
	const Error err = rebuild_setup(setup, packet, info.channels, mode_blockflag);
	if (err != OK)
		return err;
	ERR_FAIL_COND_V_MSG((setup.pos + 7) / 8 != setup_size, ERR_FILE_CORRUPT, "Didn't read exactly the setup packet");
	ogg.write_packet(packet.bytes, 0, false);

	// audio packets: u16 size + payload; prefetch copies end mid-stream, so stop at the last complete one
	struct AudioPacket
	{
		uint32_t offset;
		uint32_t size;
	};
	LocalVector<AudioPacket> packets;
	for (uint32_t offset = audio_offset; offset + 2 <= data_size;)
	{
		const uint32_t size = read_u16(data + offset);
		if (size == 0 || uint64_t(offset) + 2 + size > data_size)
			break;
		packets.push_back({ offset + 2, size });
		offset += 2 + size;
	}
	ERR_FAIL_COND_V_MSG(packets.is_empty(), ERR_FILE_CORRUPT, "No Vorbis audio packets");

	const uint32_t mode_bits = ilog(mode_blockflag.size() - 1);
	const uint32_t mode_mask = (1u << mode_bits) - 1;
	auto packet_mode = [&](uint32_t index)
	{
		const uint8_t first = data[packets[index].offset];
		return mod_packets ? (first & mode_mask) : ((first >> 1) & mode_mask);
	};

	int64_t granule = 0;
	uint32_t prev_blocksize = 0;
	bool prev_blockflag = false;
	for (uint32_t i = 0; i < packets.size(); i++)
	{
		const uint8_t *src = data + packets[i].offset;
		const uint32_t size = packets[i].size;
		const uint32_t mode = packet_mode(i);
		ERR_FAIL_COND_V_MSG(mode >= mode_blockflag.size(), ERR_FILE_CORRUPT, "Invalid Vorbis mode number");
		const bool blockflag = mode_blockflag[mode];

		packet.clear();
		if (mod_packets)
		{
			packet.write(0, 1); // audio packet type
			packet.write(mode, mode_bits);
			if (blockflag)
			{
				const bool next_blockflag = i + 1 < packets.size() && mode_blockflag[MIN(packet_mode(i + 1), mode_blockflag.size() - 1)];
				packet.write(prev_blockflag, 1);
				packet.write(next_blockflag, 1);
			}
			packet.write(src[0] >> mode_bits, 8 - mode_bits);

			// the rest is no longer byte aligned; shift it through 32 bits at a time
			uint32_t b = 1;
			for (; b + 4 <= size; b += 4)
				packet.write(read_u32(src + b), 32);
			for (; b < size; b++)
				packet.write(src[b], 8);
			packet.flush();
		}
		else
		{
			packet.bytes.resize(size);
			memcpy(packet.bytes.ptr(), src, size);
		}
		prev_blockflag = blockflag;

		// granule = samples completed after this packet: (prev + cur) / 4, nothing for the first packet
		const uint32_t blocksize = 1u << (blockflag ? blocksize_1_pow : blocksize_0_pow);
		if (i > 0)
			granule += prev_blocksize / 4 + blocksize / 4;
		prev_blocksize = blocksize;

		// the final page's granule trims the encoder padding off the end
		const bool last = i + 1 == packets.size();
		const int64_t page_granule = (last && !info.truncated) ? MIN(granule, int64_t(sample_count)) : granule;
		ogg.write_packet(packet.bytes, page_granule, last);
	}

	r_ogg.resize(ogg.out.size());
	memcpy(r_ogg.ptrw(), ogg.out.ptr(), ogg.out.size());
	return OK;
}

Ref<AudioStreamWAV> make_wav(const WemInfo &info, const PackedByteArray &pcm16)
{
	Ref<AudioStreamWAV> wav;
	wav.instantiate();
	wav->set_format(AudioStreamWAV::FORMAT_16_BITS);
	wav->set_mix_rate(info.sample_rate);
	wav->set_stereo(info.channels == 2);
	wav->set_data(pcm16);
	if (info.has_loop && info.loop_end > info.loop_start)
	{
		wav->set_loop_mode(AudioStreamWAV::LOOP_FORWARD);
		wav->set_loop_begin(info.loop_start);
		wav->set_loop_end(info.loop_end);
	}
	return wav;
}

Ref<AudioStream> convert_pcm(const uint8_t *wem, const WemInfo &info)
{
	ERR_FAIL_COND_V_MSG(info.bits_per_sample != 16, Ref<AudioStream>(), vformat("Unsupported PCM bit depth %d", info.bits_per_sample));
	ERR_FAIL_COND_V_MSG(info.channels > 2, Ref<AudioStream>(), vformat("Unsupported PCM channel count %d", info.channels));

	const uint32_t frame_bytes = 2 * info.channels;
	PackedByteArray pcm;
	pcm.resize(info.data_size / frame_bytes * frame_bytes);
	memcpy(pcm.ptrw(), wem + info.data_offset, pcm.size());
	return make_wav(info, pcm);
}

// Platinum "PtADPCM": [level][nibble] -> {difference, next level}
const int16_t PTADPCM_TABLE[12][16][2] = {
	{ { -14, 2 }, { -10, 2 }, { -7, 1 }, { -5, 1 }, { -3, 0 }, { -2, 0 }, { -1, 0 }, { 0, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 5, 1 }, { 7, 1 }, { 10, 2 }, { 14, 2 } },
	{ { -28, 3 }, { -20, 3 }, { -14, 2 }, { -10, 2 }, { -7, 1 }, { -5, 1 }, { -3, 1 }, { -1, 0 }, { 1, 0 }, { 3, 1 }, { 5, 1 }, { 7, 1 }, { 10, 2 }, { 14, 2 }, { 20, 3 }, { 28, 3 } },
	{ { -56, 4 }, { -40, 4 }, { -28, 3 }, { -20, 3 }, { -14, 2 }, { -10, 2 }, { -6, 2 }, { -2, 1 }, { 2, 1 }, { 6, 2 }, { 10, 2 }, { 14, 2 }, { 20, 3 }, { 28, 3 }, { 40, 4 }, { 56, 4 } },
	{ { -112, 5 }, { -80, 5 }, { -56, 4 }, { -40, 4 }, { -28, 3 }, { -20, 3 }, { -12, 3 }, { -4, 2 }, { 4, 2 }, { 12, 3 }, { 20, 3 }, { 28, 3 }, { 40, 4 }, { 56, 4 }, { 80, 5 }, { 112, 5 } },
	{ { -224, 6 }, { -160, 6 }, { -112, 5 }, { -80, 5 }, { -56, 4 }, { -40, 4 }, { -24, 4 }, { -8, 3 }, { 8, 3 }, { 24, 4 }, { 40, 4 }, { 56, 4 }, { 80, 5 }, { 112, 5 }, { 160, 6 }, { 224, 6 } },
	{ { -448, 7 }, { -320, 7 }, { -224, 6 }, { -160, 6 }, { -112, 5 }, { -80, 5 }, { -48, 5 }, { -16, 4 }, { 16, 4 }, { 48, 5 }, { 80, 5 }, { 112, 5 }, { 160, 6 }, { 224, 6 }, { 320, 7 }, { 448, 7 } },
	{ { -896, 8 }, { -640, 8 }, { -448, 7 }, { -320, 7 }, { -224, 6 }, { -160, 6 }, { -96, 6 }, { -32, 5 }, { 32, 5 }, { 96, 6 }, { 160, 6 }, { 224, 6 }, { 320, 7 }, { 448, 7 }, { 640, 8 }, { 896, 8 } },
	{ { -1792, 9 }, { -1280, 9 }, { -896, 8 }, { -640, 8 }, { -448, 7 }, { -320, 7 }, { -192, 7 }, { -64, 6 }, { 64, 6 }, { 192, 7 }, { 320, 7 }, { 448, 7 }, { 640, 8 }, { 896, 8 }, { 1280, 9 }, { 1792, 9 } },
	{ { -3584, 10 }, { -2560, 10 }, { -1792, 9 }, { -1280, 9 }, { -896, 8 }, { -640, 8 }, { -384, 8 }, { -128, 7 }, { 128, 7 }, { 384, 8 }, { 640, 8 }, { 896, 8 }, { 1280, 9 }, { 1792, 9 }, { 2560, 10 }, { 3584, 10 } },
	{ { -7168, 11 }, { -5120, 11 }, { -3584, 10 }, { -2560, 10 }, { -1792, 9 }, { -1280, 9 }, { -768, 9 }, { -256, 8 }, { 256, 8 }, { 768, 9 }, { 1280, 9 }, { 1792, 9 }, { 2560, 10 }, { 3584, 10 }, { 5120, 11 }, { 7168, 11 } },
	{ { -14336, 11 }, { -10240, 11 }, { -7168, 11 }, { -5120, 11 }, { -3584, 10 }, { -2560, 10 }, { -1536, 10 }, { -512, 9 }, { 512, 9 }, { 1536, 10 }, { 2560, 10 }, { 3584, 10 }, { 5120, 11 }, { 7168, 11 }, { 10240, 11 }, { 14336, 11 } },
	{ { -28672, 11 }, { -20480, 11 }, { -14336, 11 }, { -10240, 11 }, { -7168, 11 }, { -5120, 11 }, { -3072, 11 }, { -1024, 10 }, { 1024, 10 }, { 3072, 11 }, { 5120, 11 }, { 7168, 11 }, { 10240, 11 }, { 14336, 11 }, { 20480, 11 }, { 28672, 11 } },
};

Ref<AudioStream> convert_ptadpcm(const uint8_t *wem, const WemInfo &info)
{
	ERR_FAIL_COND_V_MSG(info.channels > 2, Ref<AudioStream>(), vformat("Unsupported PTADPCM channel count %d", info.channels));
	const uint32_t frame_size = info.block_align / info.channels;
	ERR_FAIL_COND_V_MSG(info.bits_per_sample != 4 || frame_size < 6 || frame_size * info.channels != info.block_align,
		Ref<AudioStream>(), "Bad PTADPCM block layout");

	// each block holds one frame per channel: s16 sample1, s16 sample2, u8 level, then nibbles (low first)
	const uint32_t channels = info.channels;
	const uint32_t samples_per_frame = 2 + (frame_size - 5) * 2;
	const uint32_t blocks = info.data_size / info.block_align;
	PackedByteArray pcm;
	pcm.resize(blocks * samples_per_frame * channels * 2);
	int16_t *out = reinterpret_cast<int16_t *>(pcm.ptrw());
	const uint8_t *data = wem + info.data_offset;

	for (uint32_t block = 0; block < blocks; block++)
	{
		for (uint32_t ch = 0; ch < channels; ch++)
		{
			const uint8_t *frame = data + block * info.block_align + ch * frame_size;
			int16_t *dst = out + block * samples_per_frame * channels + ch;
			int32_t prev2 = int16_t(read_u16(frame + 0));
			int32_t prev1 = int16_t(read_u16(frame + 2));
			uint32_t level = MIN(frame[4], uint8_t(11));
			dst[0] = int16_t(prev2);
			dst[channels] = int16_t(prev1);
			for (uint32_t i = 0; i < samples_per_frame - 2; i++)
			{
				const uint32_t nibble = (frame[5 + i / 2] >> ((i & 1) * 4)) & 0xF;
				const int32_t sample = CLAMP(prev1 * 2 - prev2 + PTADPCM_TABLE[level][nibble][0], -32768, 32767);
				level = PTADPCM_TABLE[level][nibble][1];
				dst[(i + 2) * channels] = int16_t(sample);
				prev2 = prev1;
				prev1 = sample;
			}
		}
	}
	return make_wav(info, pcm);
}

bool is_supported_codec(uint16_t format)
{
	switch (format)
	{
		case WwiseSoundBank::CODEC_VORBIS:
		case WwiseSoundBank::CODEC_PCM:
		case WwiseSoundBank::CODEC_PCM_AUTHORING:
		case WwiseSoundBank::CODEC_PTADPCM:
			return true;
		default:
			return false;
	}
}

Ref<AudioStream> decode_wem(const uint8_t *wem, uint32_t size)
{
	WemInfo info;
	if (parse_wem(wem, size, info) != OK)
		return Ref<AudioStream>();

	switch (info.format)
	{
		case WwiseSoundBank::CODEC_VORBIS:
		{
			PackedByteArray ogg;
			if (convert_vorbis(wem, info, ogg) != OK)
				return Ref<AudioStream>();
			Ref<AudioStreamOggVorbis> stream = AudioStreamOggVorbis::load_from_buffer(ogg);
			if (stream.is_valid() && info.has_loop)
			{
				stream->set_loop(true);
				stream->set_loop_offset(double(info.loop_start) / info.sample_rate);
			}
			return stream;
		}
		case WwiseSoundBank::CODEC_PCM:
		case WwiseSoundBank::CODEC_PCM_AUTHORING:
			return convert_pcm(wem, info);
		case WwiseSoundBank::CODEC_PTADPCM:
			return convert_ptadpcm(wem, info);
		default:
			ERR_FAIL_V_MSG(Ref<AudioStream>(), vformat("Unsupported WEM codec 0x%04x", info.format));
	}
}

// HIRC object types (bank v145) that reference media
constexpr uint8_t HIRC_SOUND = 2;
constexpr uint8_t HIRC_MUSIC_TRACK = 11;

// Plug-in ids are (codec or plug-in id << 16) | (company << 4) | type, see AkTypes.h
constexpr uint32_t AK_PLUGIN_TYPE_MASK = 0xF;
constexpr uint32_t AK_PLUGIN_TYPE_CODEC = 1;
constexpr uint32_t AK_PLUGIN_TYPE_SOURCE = 2;

// AKCODECID_PCM, AKCODECID_ADPCM (PTADPCM on Switch), AKCODECID_VORBIS, AKCODECID_PCMEX
bool is_supported_codec_plugin(uint32_t plugin_id)
{
	if ((plugin_id & AK_PLUGIN_TYPE_MASK) != AK_PLUGIN_TYPE_CODEC)
		return false;
	switch (plugin_id >> 16)
	{
		case 1:
		case 2:
		case 4:
		case 7:
			return true;
		default:
			return false;
	}
}

struct HircSource
{
	uint32_t id = 0;
	uint32_t plugin_id = 0;
	uint8_t stream_type = 0; // 0 = in bank, 1 = prefetch + streamed, 2 = streamed
};

// One AkBankSourceData, as read by CAkBankMgr::LoadSource.
bool read_bank_source(const uint8_t *d, uint32_t &offset, uint64_t end, HircSource &r_source)
{
	if (offset + uint64_t(14) > end)
		return false;
	r_source.plugin_id = read_u32(d + offset);
	r_source.stream_type = d[offset + 4];
	r_source.id = read_u32(d + offset + 5);
	offset += 14; // + in-memory size and source bits

	if ((r_source.plugin_id & AK_PLUGIN_TYPE_MASK) == AK_PLUGIN_TYPE_SOURCE)
	{
		// source plug-ins carry a parameter blob
		if (offset + uint64_t(4) > end)
			return false;
		const uint64_t blob_end = offset + uint64_t(4) + read_u32(d + offset);
		if (blob_end > end)
			return false;
		offset = uint32_t(blob_end);
	}
	return true;
}

void parse_hirc_sources(const uint8_t *d, uint32_t offset, uint32_t size, LocalVector<HircSource> &r_sources)
{
	if (size < 4)
		return;
	const uint64_t end = uint64_t(offset) + size;
	const uint32_t count = read_u32(d + offset);
	offset += 4;
	for (uint32_t i = 0; i < count && offset + uint64_t(5) <= end; i++)
	{
		const uint8_t type = d[offset];
		const uint32_t body = offset + 5;
		const uint64_t object_end = uint64_t(body) + read_u32(d + offset + 1);
		ERR_FAIL_COND_MSG(object_end > end, "HIRC object truncated");

		HircSource source;
		if (type == HIRC_SOUND)
		{
			uint32_t p = body + 4; // id
			if (read_bank_source(d, p, object_end, source))
				r_sources.push_back(source);
		}
		else if (type == HIRC_MUSIC_TRACK && body + uint64_t(9) <= object_end)
		{
			uint32_t p = body + 5; // id, flags
			const uint32_t source_count = read_u32(d + p);
			p += 4;
			for (uint32_t k = 0; k < source_count && read_bank_source(d, p, object_end, source); k++)
				r_sources.push_back(source);
		}
		offset = uint32_t(object_end);
	}
}

}

// WwisePackage

Error WwisePackage::LoadFromFile(const String &path)
{
	file_path = String();
	wems.clear();
	banks.clear();
	wem_ids.clear();
	bank_ids.clear();

	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(file.is_null(), ERR_FILE_CANT_OPEN, "Could not open " + path);
	const uint64_t file_size = file->get_length();
	ERR_FAIL_COND_V_MSG(file_size < 0x1c || file->get_buffer(4).get_string_from_ascii() != "AKPK", ERR_FILE_CORRUPT,
		"Not a Wwise package (missing AKPK)");

	// header: version, then the byte sizes of the language map and each lookup table
	const uint32_t header_size = file->get_32();
	ERR_FAIL_COND_V_MSG(header_size < 0x14 || 8 + uint64_t(header_size) > file_size, ERR_FILE_CORRUPT, "Bad AKPK header size");
	const PackedByteArray header = file->get_buffer(header_size);
	const uint8_t *h = header.ptr();
	const uint32_t version = read_u32(h + 0x00);
	const uint32_t language_map_size = read_u32(h + 0x04);
	const uint32_t banks_size = read_u32(h + 0x08);
	const uint32_t wems_size = read_u32(h + 0x0c);
	ERR_FAIL_COND_V_MSG(version != 1, ERR_UNAVAILABLE, vformat("Unsupported AKPK version %d", version));

	// newer packages add an externals table size; detect it by whether the tables then fill the header exactly
	const uint64_t tables_size = uint64_t(language_map_size) + banks_size + wems_size;
	const uint32_t tables_offset = (0x14 + tables_size + read_u32(h + 0x10) == header_size) ? 0x14 : 0x10;
	ERR_FAIL_COND_V_MSG(tables_offset + tables_size > header_size, ERR_FILE_CORRUPT, "AKPK lookup tables exceed header");

	// lookup table: count, then {u32 id, u32 block size, u32 size, u32 start block, u32 language}
	auto parse_table = [&](uint32_t offset, uint32_t size, HashMap<uint32_t, FileEntry> &r_entries, PackedInt64Array &r_ids)
	{
		if (size < 4)
			return OK;
		const uint32_t count = read_u32(h + offset);
		ERR_FAIL_COND_V_MSG(4 + uint64_t(count) * 20 > size, ERR_FILE_CORRUPT, "AKPK lookup table truncated");
		r_ids.resize(count);
		for (uint32_t i = 0; i < count; i++)
		{
			const uint8_t *e = h + offset + 4 + i * 20;
			const uint32_t id = read_u32(e);
			FileEntry entry;
			entry.size = read_u32(e + 8);
			entry.offset = uint64_t(read_u32(e + 12)) * read_u32(e + 4);
			ERR_FAIL_COND_V_MSG(entry.offset + entry.size > file_size, ERR_FILE_CORRUPT, vformat("AKPK entry %d is out of the file", id));
			r_entries[id] = entry;
			r_ids.set(i, id);
		}
		return OK;
	};

	const uint32_t banks_offset = tables_offset + language_map_size;
	Error err = parse_table(banks_offset, banks_size, banks, bank_ids);
	if (err == OK)
		err = parse_table(banks_offset + banks_size, wems_size, wems, wem_ids);
	if (err != OK)
		return err;

	file_path = path;
	return OK;
}

PackedInt64Array WwisePackage::get_wem_ids() const
{
	return wem_ids;
}

bool WwisePackage::has_wem(int64_t wem_id) const
{
	return wems.has(uint32_t(wem_id));
}

PackedByteArray WwisePackage::get_wem_data(int64_t wem_id, int64_t max_size) const
{
	const FileEntry *entry = wems.getptr(uint32_t(wem_id));
	ERR_FAIL_NULL_V_MSG(entry, PackedByteArray(), vformat("WEM %d not found in package", wem_id));
	return read_entry(*entry, uint32_t(CLAMP(max_size, int64_t(0), int64_t(UINT32_MAX))));
}

Ref<AudioStream> WwisePackage::get_stream(int64_t wem_id) const
{
	const PackedByteArray wem = get_wem_data(wem_id);
	return wem.is_empty() ? Ref<AudioStream>() : decode_wem(wem.ptr(), wem.size());
}

PackedInt64Array WwisePackage::get_bank_ids() const
{
	return bank_ids;
}

PackedByteArray WwisePackage::get_bank_data(int64_t bank_id) const
{
	const FileEntry *entry = banks.getptr(uint32_t(bank_id));
	ERR_FAIL_NULL_V_MSG(entry, PackedByteArray(), vformat("Bank %d not found in package", bank_id));
	return read_entry(*entry, 0);
}

PackedByteArray WwisePackage::read_entry(const FileEntry &entry, uint32_t max_size) const
{
	Ref<FileAccess> file = FileAccess::open(file_path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(file.is_null(), PackedByteArray(), "Could not open " + file_path);
	file->seek(entry.offset);
	return file->get_buffer(max_size ? MIN(entry.size, max_size) : entry.size);
}

// AudioStreamWwise

void AudioStreamWwise::setup(const Ref<WwisePackage> &p_package, uint32_t p_wem_id)
{
	package = p_package;
	wem_id = p_wem_id;
	length = -1.0;
}

Ref<AudioStream> AudioStreamWwise::get_decoded_stream() const
{
	ERR_FAIL_COND_V(package.is_null(), Ref<AudioStream>());
	return package->get_stream(wem_id);
}

Ref<AudioStreamPlayback> AudioStreamWwise::_instantiate_playback() const
{
	const Ref<AudioStream> stream = get_decoded_stream();
	ERR_FAIL_COND_V_MSG(stream.is_null(), Ref<AudioStreamPlayback>(), vformat("Failed to decode streamed WEM %d", wem_id));
	return stream->instantiate_playback();
}

String AudioStreamWwise::_get_stream_name() const
{
	return vformat("WEM %d", wem_id);
}

double AudioStreamWwise::_get_length() const
{
	if (length < 0.0 && package.is_valid())
	{
		const PackedByteArray header = package->get_wem_data(wem_id, WEM_HEADER_READ_SIZE);
		WemInfo info;
		length = parse_wem(header.ptr(), header.size(), info) == OK ? wem_length(header.ptr(), info) : 0.0;
	}
	return MAX(length, 0.0);
}

// WwiseSoundBank

Error WwiseSoundBank::LoadFromFile(const String &path)
{
	Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(file.is_null(), ERR_FILE_CANT_OPEN, "Could not open " + path);

	packages.clear();
	const String dir = path.get_base_dir();
	for (const String &name : DirAccess::get_files_at(dir))
	{
		if (name.get_extension().to_lower() != "pck")
			continue;
		Ref<WwisePackage> package;
		package.instantiate();
		if (package->LoadFromFile(dir.path_join(name)) == OK)
			packages.push_back(package);
	}

	return LoadFromBuffer(file->get_buffer(file->get_length()));
}

Error WwiseSoundBank::LoadFromBuffer(const PackedByteArray &buffer)
{
	media.clear();
	media_entries.clear();
	sources.clear();
	wem_ids.clear();
	streams.clear();
	bank_version = 0;
	bank_id = 0;

	const uint8_t *d = buffer.ptr();
	const uint32_t size = buffer.size();
	ERR_FAIL_COND_V_MSG(size < 16 || memcmp(d, "BKHD", 4) != 0, ERR_FILE_CORRUPT, "Not a Wwise soundbank (missing BKHD)");

	uint32_t didx_offset = 0, didx_size = 0;
	uint32_t data_offset = 0, data_size = 0;
	uint32_t hirc_offset = 0, hirc_size = 0;
	bool has_data = false;
	for (uint32_t offset = 0; offset + 8 <= size;)
	{
		const uint32_t section_size = read_u32(d + offset + 4);
		const uint32_t body = offset + 8;
		ERR_FAIL_COND_V_MSG(uint64_t(body) + section_size > size, ERR_FILE_CORRUPT, "Soundbank section truncated");
		if (memcmp(d + offset, "BKHD", 4) == 0)
		{
			ERR_FAIL_COND_V(section_size < 8, ERR_FILE_CORRUPT);
			bank_version = read_u32(d + body);
			bank_id = read_u32(d + body + 4);
		}
		else if (memcmp(d + offset, "DIDX", 4) == 0)
		{
			didx_offset = body;
			didx_size = section_size;
		}
		else if (memcmp(d + offset, "DATA", 4) == 0)
		{
			data_offset = body;
			data_size = section_size;
			has_data = true;
		}
		else if (memcmp(d + offset, "HIRC", 4) == 0)
		{
			hirc_offset = body;
			hirc_size = section_size;
		}
		offset = body + section_size;
	}

	// embedded media: DIDX entries {u32 id, u32 offset, u32 size} into DATA
	ERR_FAIL_COND_V_MSG(didx_size > 0 && !has_data, ERR_FILE_CORRUPT, "Soundbank has DIDX but no DATA section");
	media = buffer.slice(data_offset, data_offset + data_size);
	for (uint32_t i = 0; i + 12 <= didx_size; i += 12)
	{
		const uint8_t *e = d + didx_offset + i;
		const uint32_t id = read_u32(e);
		MediaEntry entry;
		entry.offset = read_u32(e + 4);
		entry.size = read_u32(e + 8);
		ERR_FAIL_COND_V_MSG(uint64_t(entry.offset) + entry.size > data_size, ERR_FILE_CORRUPT, vformat("WEM %d is out of the DATA section", id));
		media_entries[id] = entry;
		wem_ids.push_back(id);
	}

	// media referenced by Sound/MusicTrack objects; streamed sources aren't in DIDX, or only as prefetch heads
	LocalVector<HircSource> hirc_sources;
	parse_hirc_sources(d, hirc_offset, hirc_size, hirc_sources);
	for (const HircSource &hirc_source : hirc_sources)
	{
		if ((hirc_source.plugin_id & AK_PLUGIN_TYPE_MASK) != AK_PLUGIN_TYPE_CODEC)
			continue;
		SourceEntry *entry = sources.getptr(hirc_source.id);
		if (!entry)
		{
			entry = &sources.insert(hirc_source.id, SourceEntry())->value;
			entry->plugin_id = hirc_source.plugin_id;
			if (hirc_source.stream_type != 0 && !media_entries.has(hirc_source.id))
				wem_ids.push_back(hirc_source.id);
		}
		entry->streamed |= hirc_source.stream_type != 0;
	}

	int unsupported = 0;
	int failed = 0;
	int missing = 0;
	for (const int64_t id : wem_ids)
	{
		const SourceEntry *source = sources.getptr(uint32_t(id));
		const MediaEntry *embedded = media_entries.getptr(uint32_t(id));
		if (source && source->streamed)
		{
			const Ref<WwisePackage> package = find_package(uint32_t(id));
			if (package.is_valid())
			{
				if (!is_supported_codec_plugin(source->plugin_id))
				{
					unsupported++;
					continue;
				}
				Ref<AudioStreamWwise> stream;
				stream.instantiate();
				stream->setup(package, uint32_t(id));
				streams[id] = stream;
				continue;
			}
			if (!embedded)
			{
				missing++;
				continue;
			}
			// only the embedded prefetch head is available
		}

		const uint8_t *wem = media.ptr() + embedded->offset;
		WemInfo info;
		if (parse_wem(wem, embedded->size, info) != OK)
		{
			failed++;
			continue;
		}
		if (!is_supported_codec(info.format))
		{
			unsupported++;
			continue;
		}
		const Ref<AudioStream> stream = decode_wem(wem, embedded->size);
		if (stream.is_null())
		{
			failed++;
			continue;
		}
		streams[id] = stream;
	}

	if (unsupported > 0)
		WARN_PRINT(vformat("Soundbank %d: %d WEM(s) use codecs Godot can't decode (e.g. Opus) and were skipped; raw data is available via get_wem_data().", bank_id, unsupported));
	if (failed > 0)
		WARN_PRINT(vformat("Soundbank %d: %d WEM(s) failed to convert.", bank_id, failed));
	if (missing > 0)
		WARN_PRINT(vformat("Soundbank %d: %d streamed WEM(s) were not found in any Wwise package.", bank_id, missing));
	return OK;
}

void WwiseSoundBank::add_package(const Ref<WwisePackage> &package)
{
	ERR_FAIL_COND(package.is_null());
	packages.push_back(package);
}

Ref<AudioStream> WwiseSoundBank::get_stream(int64_t wem_id) const
{
	return streams.get(wem_id, Ref<AudioStream>());
}

PackedByteArray WwiseSoundBank::get_wem_data(int64_t wem_id) const
{
	// streamed sources come from their package; embedded copies of those are only prefetch heads
	const MediaEntry *embedded = media_entries.getptr(uint32_t(wem_id));
	if (!embedded || is_streamed(wem_id))
	{
		const Ref<WwisePackage> package = find_package(uint32_t(wem_id));
		ERR_FAIL_COND_V_MSG(package.is_null(), PackedByteArray(), vformat("WEM %d not found in bank or packages", wem_id));
		return package->get_wem_data(wem_id);
	}
	return media.slice(embedded->offset, embedded->offset + embedded->size);
}

int WwiseSoundBank::get_wem_codec(int64_t wem_id) const
{
	WemInfo info;
	const MediaEntry *embedded = media_entries.getptr(uint32_t(wem_id));
	if (embedded)
		return parse_wem(media.ptr() + embedded->offset, embedded->size, info) == OK ? info.format : 0;

	const Ref<WwisePackage> package = find_package(uint32_t(wem_id));
	ERR_FAIL_COND_V_MSG(package.is_null(), 0, vformat("WEM %d not found in bank or packages", wem_id));
	const PackedByteArray header = package->get_wem_data(wem_id, WEM_HEADER_READ_SIZE);
	return parse_wem(header.ptr(), header.size(), info) == OK ? info.format : 0;
}

bool WwiseSoundBank::is_streamed(int64_t wem_id) const
{
	const SourceEntry *source = sources.getptr(uint32_t(wem_id));
	return source && source->streamed && find_package(uint32_t(wem_id)).is_valid();
}

Ref<AudioStream> WwiseSoundBank::load_wem(const PackedByteArray &wem)
{
	return decode_wem(wem.ptr(), wem.size());
}

PackedByteArray WwiseSoundBank::wem_to_ogg(const PackedByteArray &wem)
{
	WemInfo info;
	if (parse_wem(wem.ptr(), wem.size(), info) != OK)
		return PackedByteArray();
	ERR_FAIL_COND_V_MSG(info.format != CODEC_VORBIS, PackedByteArray(), vformat("WEM codec 0x%04x is not Vorbis", info.format));
	PackedByteArray ogg;
	return convert_vorbis(wem.ptr(), info, ogg) == OK ? ogg : PackedByteArray();
}

Ref<WwisePackage> WwiseSoundBank::find_package(uint32_t wem_id) const
{
	for (const Ref<WwisePackage> &package : packages)
	{
		if (package->has_wem(wem_id))
			return package;
	}
	return Ref<WwisePackage>();
}

// ResourceFormatLoaderBNK

PackedStringArray ResourceFormatLoaderBNK::_get_recognized_extensions() const
{
	return PackedStringArray({ "bnk" });
}

String ResourceFormatLoaderBNK::_get_resource_type(const String &path) const
{
	return path.get_extension().to_lower() == "bnk" ? "WwiseSoundBank" : "";
}

bool ResourceFormatLoaderBNK::_handles_type(const StringName &type) const
{
	return type == StringName("WwiseSoundBank");
}

Variant ResourceFormatLoaderBNK::_load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const
{
	Ref<WwiseSoundBank> bank;
	bank.instantiate();
	const Error err = bank->LoadFromFile(path);
	if (err != OK)
		return err;
	return bank;
}

// AudioStreamPreviewControl

void AudioStreamPreviewControl::_notification(int what)
{
	if (what == NOTIFICATION_PROCESS)
		_update_ui();
	else if (what == NOTIFICATION_EXIT_TREE && player)
		player->stop();
}

void AudioStreamPreviewControl::setup(const Ref<AudioStream> &p_stream)
{
	length = p_stream.is_valid() ? p_stream->get_length() : 0.0;

	play_button = memnew(Button);
	play_button->set_custom_minimum_size(Vector2(60, 0));
	play_button->connect("pressed", Callable(this, "_on_play_pressed"));
	add_child(play_button);

	time_label = memnew(Label);
	time_label->set_h_size_flags(SIZE_EXPAND_FILL);
	add_child(time_label);

	player = memnew(AudioStreamPlayer);
	player->set_stream(p_stream);
	player->connect("finished", Callable(this, "_on_finished"));
	add_child(player);

	_update_ui();
}

void AudioStreamPreviewControl::_on_play_pressed()
{
	if (player->is_playing())
		player->stop();
	else
		player->play();
	set_process(player->is_playing());
	_update_ui();
}

void AudioStreamPreviewControl::_on_finished()
{
	set_process(false);
	_update_ui();
}

void AudioStreamPreviewControl::_update_ui()
{
	const bool playing = player->is_playing();
	play_button->set_text(playing ? "Stop" : "Play");
	time_label->set_text(vformat("%.2f / %.2f s", playing ? player->get_playback_position() : 0.0, length));
}

// EditorInspectorPluginAudioStreamOgg

void EditorInspectorPluginAudioStreamOgg::_parse_begin(Object *object)
{
	AudioStreamPreviewControl *control = memnew(AudioStreamPreviewControl);
	control->setup(Ref<AudioStream>(Object::cast_to<AudioStream>(object)));
	add_custom_control(control);
}
