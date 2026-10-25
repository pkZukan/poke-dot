#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/classes/audio_stream_wav.hpp>
#include <godot_cpp/classes/audio_stream_ogg_vorbis.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/editor_inspector_plugin.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace godot
{

// Wwise file package (AKPK .pck) holding streamed WEMs and/or soundbanks.
// Only the lookup tables are read up front; file data is read from disk on request.
class WwisePackage : public Resource
{
	GDCLASS(WwisePackage, Resource)

protected:
	static void _bind_methods();

public:
	Error LoadFromFile(const String &path);

	PackedInt64Array get_wem_ids() const;
	bool has_wem(int64_t wem_id) const;
	// max_size > 0 reads only the start of the file, e.g. for its header
	PackedByteArray get_wem_data(int64_t wem_id, int64_t max_size = 0) const;
	Ref<AudioStream> get_stream(int64_t wem_id) const;

	PackedInt64Array get_bank_ids() const;
	PackedByteArray get_bank_data(int64_t bank_id) const;

private:
	struct FileEntry
	{
		uint64_t offset = 0;
		uint32_t size = 0;
	};

	String file_path;
	HashMap<uint32_t, FileEntry> wems;
	HashMap<uint32_t, FileEntry> banks;
	PackedInt64Array wem_ids;
	PackedInt64Array bank_ids;

	PackedByteArray read_entry(const FileEntry &entry, uint32_t max_size) const;
};

// Streamed WEM from a WwisePackage. Nothing is read until the stream is played (each
// playback decodes its own copy, freed with the playback) or its length is queried (header only).
class AudioStreamWwise : public AudioStream
{
	GDCLASS(AudioStreamWwise, AudioStream)

protected:
	static void _bind_methods();

public:
	void setup(const Ref<WwisePackage> &p_package, uint32_t p_wem_id);
	int64_t get_wem_id() const { return wem_id; }
	// Decodes a fresh copy on every call; nothing is cached.
	Ref<AudioStream> get_decoded_stream() const;

	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	String _get_stream_name() const override;
	double _get_length() const override;

private:
	Ref<WwisePackage> package;
	uint32_t wem_id = 0;
	mutable double length = -1.0;
};

// Wwise soundbank (.bnk). Media is exposed as AudioStreams keyed by WEM id:
//  - Wwise Vorbis (0xFFFF) -> AudioStreamOggVorbis (ww2ogg rebuild, aoTuV 6.03 codebooks)
//  - PCM (0x0001/0xFFFE)   -> AudioStreamWAV
//  - PTADPCM (0x8311)      -> AudioStreamWAV (decoded to 16-bit PCM)
// Embedded media is decoded on load. Streamed sources referenced by the bank's Sound and
// MusicTrack objects (HIRC) are looked up in the Wwise packages and exposed as lazy
// AudioStreamWwise, which also replaces the truncated prefetch copies embedded in the bank.
// Other codecs (e.g. Switch Opus 0x3039) are only available as raw data via get_wem_data().
class WwiseSoundBank : public Resource
{
	GDCLASS(WwiseSoundBank, Resource)

protected:
	static void _bind_methods();

public:
	enum Codec
	{
		CODEC_PCM = 0x0001,
		CODEC_PCM_AUTHORING = 0xFFFE,
		CODEC_VORBIS = 0xFFFF,
		CODEC_OPUS_NX = 0x3039,
		CODEC_OPUS = 0x3040,
		CODEC_OPUS_WEM = 0x3041,
		CODEC_PTADPCM = 0x8311,
	};

	// LoadFromFile uses every .pck in the bank's directory; LoadFromBuffer only
	// uses packages registered with add_package() beforehand.
	Error LoadFromFile(const String &path);
	Error LoadFromBuffer(const PackedByteArray &buffer);
	void add_package(const Ref<WwisePackage> &package);

	uint32_t get_bank_version() const { return bank_version; }
	uint32_t get_bank_id() const { return bank_id; }
	TypedDictionary<int64_t, AudioStream> get_streams() const { return streams; }
	Ref<AudioStream> get_stream(int64_t wem_id) const;
	// Embedded media first, then streamed sources, in bank order.
	PackedInt64Array get_wem_ids() const { return wem_ids; }
	PackedByteArray get_wem_data(int64_t wem_id) const;
	int get_wem_codec(int64_t wem_id) const;
	bool is_streamed(int64_t wem_id) const;

	static Ref<AudioStream> load_wem(const PackedByteArray &wem);
	static PackedByteArray wem_to_ogg(const PackedByteArray &wem);

private:
	struct MediaEntry
	{
		uint32_t offset = 0;
		uint32_t size = 0;
	};

	struct SourceEntry
	{
		uint32_t plugin_id = 0;
		bool streamed = false;
	};

	PackedByteArray media;
	HashMap<uint32_t, MediaEntry> media_entries;
	HashMap<uint32_t, SourceEntry> sources;
	PackedInt64Array wem_ids;
	Vector<Ref<WwisePackage>> packages;
	TypedDictionary<int64_t, AudioStream> streams;
	uint32_t bank_version = 0;
	uint32_t bank_id = 0;

	Ref<WwisePackage> find_package(uint32_t wem_id) const;
};

}

VARIANT_ENUM_CAST(godot::WwiseSoundBank::Codec);

namespace godot
{

class ResourceFormatLoaderBNK : public ResourceFormatLoader
{
	GDCLASS(ResourceFormatLoaderBNK, ResourceFormatLoader)

protected:
	static void _bind_methods() {}

public:
	PackedStringArray _get_recognized_extensions() const override;
	String _get_resource_type(const String &path) const override;
	bool _handles_type(const StringName &type) const override;
	Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};

// Godot's built-in inspector player only handles AudioStreamWAV, so Ogg and lazy Wwise
// streams (e.g. from WwiseSoundBank) get this minimal play/stop preview instead.
class AudioStreamPreviewControl : public HBoxContainer
{
	GDCLASS(AudioStreamPreviewControl, HBoxContainer)

protected:
	static void _bind_methods();
	void _notification(int what);

public:
	void setup(const Ref<AudioStream> &p_stream);

private:
	Button *play_button = nullptr;
	Label *time_label = nullptr;
	AudioStreamPlayer *player = nullptr;
	double length = 0.0;

	void _on_play_pressed();
	void _on_finished();
	void _update_ui();
};

class EditorInspectorPluginAudioStreamOgg : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginAudioStreamOgg, EditorInspectorPlugin)

protected:
	static void _bind_methods() {}

public:
	bool _can_handle(Object *object) const override
	{
		return Object::cast_to<AudioStreamOggVorbis>(object) || Object::cast_to<AudioStreamWwise>(object);
	}

	void _parse_begin(Object *object) override;
};

class WwiseEditorPlugin : public EditorPlugin
{
	GDCLASS(WwiseEditorPlugin, EditorPlugin)

private:
	Ref<EditorInspectorPluginAudioStreamOgg> inspector_plugin;

protected:
	static void _bind_methods() {}

public:
	void _enter_tree() override
	{
		inspector_plugin.instantiate();
		add_inspector_plugin(inspector_plugin);
	}

	void _exit_tree() override
	{
		remove_inspector_plugin(inspector_plugin);
		inspector_plugin.unref();
	}
};

}
