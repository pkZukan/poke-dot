#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/trtml_generated.h"
#include <utils.h>

namespace godot
{

class TRTimelineValueInt : public Resource
{
    GDCLASS(TRTimelineValueInt, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineValueInt() = default;
    ~TRTimelineValueInt() = default;

    GETTER_SETTER_DEFINE(int32_t, value)

private:
    int32_t value = 0;
};

class TRTimelineValueFloat : public Resource
{
    GDCLASS(TRTimelineValueFloat, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineValueFloat() = default;
    ~TRTimelineValueFloat() = default;

    GETTER_SETTER_DEFINE(float, value)

private:
    float value = 0.0f;
};

class TRTimelineValueBool : public Resource
{
    GDCLASS(TRTimelineValueBool, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineValueBool() = default;
    ~TRTimelineValueBool() = default;

    GETTER_SETTER_DEFINE(bool, value)

private:
    bool value = false;
};

class TRTimelineValueString : public Resource
{
    GDCLASS(TRTimelineValueString, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineValueString() = default;
    ~TRTimelineValueString() = default;

    GETTER_SETTER_DEFINE(String, value)

private:
    String value;
};

class TRTimelineValueVector3 : public Resource
{
    GDCLASS(TRTimelineValueVector3, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineValueVector3() = default;
    ~TRTimelineValueVector3() = default;

    GETTER_SETTER_DEFINE(Vector3, value)

private:
    Vector3 value;
};

class TRTimelineParameter : public Resource
{
    GDCLASS(TRTimelineParameter, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineParameter() = default;
    ~TRTimelineParameter() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Ref<Resource>, value)
    GETTER_SETTER_DEFINE(bool, flag_4)
    GETTER_SETTER_DEFINE(bool, flag_5)

private:
    String Name;
    Ref<Resource> value;
    bool flag_4 = false;
    bool flag_5 = false;
};

class TRTimelineEvent : public Resource
{
    GDCLASS(TRTimelineEvent, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineEvent() = default;
    ~TRTimelineEvent() = default;

    GETTER_SETTER_DEFINE(String, track_name)
    GETTER_SETTER_DEFINE(String, command)
    GETTER_SETTER_DEFINE(int32_t, begin_frame)
    GETTER_SETTER_DEFINE(int32_t, end_frame)
    GETTER_SETTER_DEFINE(bool, flag_4)
    GETTER_SETTER_DEFINE(uint32_t, kind)
    GETTER_SETTER_DEFINE(Array, parameters)

private:
    String track_name;
    String command;
    int32_t begin_frame = 0;
    int32_t end_frame = 0;
    bool flag_4 = false;
    uint32_t kind = 0;
    Array parameters;
};

class TRTimelineEventGroup : public Resource
{
    GDCLASS(TRTimelineEventGroup, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineEventGroup() = default;
    ~TRTimelineEventGroup() = default;

    GETTER_SETTER_DEFINE(Array, events)

private:
    Array events;
};

class TRTimelineTrack : public Resource
{
    GDCLASS(TRTimelineTrack, Resource)
protected:
    static void _bind_methods();

public:
    TRTimelineTrack() = default;
    ~TRTimelineTrack() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Array, groups)
    GETTER_SETTER_DEFINE(PackedByteArray, empty_vector)

private:
    String Name;
    Array groups;
    PackedByteArray empty_vector;
};

class TRTimeline : public Resource
{
    GDCLASS(TRTimeline, Resource)
protected:
    static void _bind_methods();

public:
    TRTimeline() = default;
    ~TRTimeline() = default;

    Error LoadFromFile(String file);

    GETTER_SETTER_DEFINE(Array, tracks)
    GETTER_SETTER_DEFINE(uint32_t, frame_count)
    GETTER_SETTER_DEFINE(uint32_t, count_3)
    GETTER_SETTER_DEFINE(uint32_t, count_4)
    GETTER_SETTER_DEFINE(PackedStringArray, resource_paths)

private:
    Ref<Resource> _LoadParameterValue(Titan::Animation::ParameterValue type, const void* data);

    Array tracks;
    uint32_t frame_count = 0;
    uint32_t count_3 = 0;
    uint32_t count_4 = 0;
    PackedStringArray resource_paths;
};

class ResourceFormatLoaderTRTML : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderTRTML, ResourceFormatLoader)
protected:
    static void _bind_methods()
    {
    }

public:
    ResourceFormatLoaderTRTML() = default;
    ~ResourceFormatLoaderTRTML() = default;

    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual bool _handles_type(const StringName& p_type) const override;
    virtual Variant _load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                          int32_t p_cache_mode) const override;
};

}
