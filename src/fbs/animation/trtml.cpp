#include "trtml.h"
#include <cstring>

using namespace godot;

void TRTimelineValueInt::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineValueInt, value, Variant::INT, PROPERTY_HINT_NONE)
}

void TRTimelineValueFloat::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineValueFloat, value, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void TRTimelineValueBool::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineValueBool, value, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TRTimelineValueString::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineValueString, value, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRTimelineValueVector3::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineValueVector3, value, Variant::VECTOR3, PROPERTY_HINT_NONE)
}

void TRTimelineParameter::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineParameter, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineParameter, value, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE,
                       "TRTimelineValueInt,TRTimelineValueFloat,TRTimelineValueBool,TRTimelineValueString,TRTimelineValueVector3")
    GETTER_SETTER_BIND(TRTimelineParameter, flag_4, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineParameter, flag_5, Variant::BOOL, PROPERTY_HINT_NONE)
}

void TRTimelineEvent::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineEvent, track_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineEvent, command, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineEvent, begin_frame, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineEvent, end_frame, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineEvent, flag_4, Variant::BOOL, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineEvent, kind, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineEvent, parameters, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTimelineParameter")
}

void TRTimelineEventGroup::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineEventGroup, events, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTimelineEvent")
}

void TRTimelineTrack::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimelineTrack, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimelineTrack, groups, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTimelineEventGroup")
    GETTER_SETTER_BIND(TRTimelineTrack, empty_vector, Variant::PACKED_BYTE_ARRAY, PROPERTY_HINT_NONE)
}

void TRTimeline::_bind_methods()
{
    GETTER_SETTER_BIND(TRTimeline, tracks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRTimelineTrack")
    GETTER_SETTER_BIND(TRTimeline, frame_count, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimeline, count_3, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimeline, count_4, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRTimeline, resource_paths, Variant::PACKED_STRING_ARRAY, PROPERTY_HINT_NONE)
}

Ref<Resource> TRTimeline::_LoadParameterValue(Titan::Animation::ParameterValue type, const void* data)
{
    if (!data)
    {
        return Ref<Resource>();
    }

    switch (type)
    {
        case Titan::Animation::ParameterValue_IntegerValue:
        {
            auto src = static_cast<const Titan::Animation::IntegerValue*>(data);
            Ref<TRTimelineValueInt> value;
            value.instantiate();
            value->set_value(src->value());
            return value;
        }
        case Titan::Animation::ParameterValue_RealValue:
        {
            auto src = static_cast<const Titan::Animation::RealValue*>(data);
            Ref<TRTimelineValueFloat> value;
            value.instantiate();
            value->set_value(src->value());
            return value;
        }
        case Titan::Animation::ParameterValue_BooleanValue:
        {
            auto src = static_cast<const Titan::Animation::BooleanValue*>(data);
            Ref<TRTimelineValueBool> value;
            value.instantiate();
            value->set_value(src->value());
            return value;
        }
        case Titan::Animation::ParameterValue_TextValue:
        {
            auto src = static_cast<const Titan::Animation::TextValue*>(data);
            Ref<TRTimelineValueString> value;
            value.instantiate();
            value->set_value(Utils::toGodotString(src->value()));
            return value;
        }
        case Titan::Animation::ParameterValue_Float3Value:
        {
            auto src = static_cast<const Titan::Animation::Float3Value*>(data);
            Ref<TRTimelineValueVector3> value;
            value.instantiate();
            value->set_value(src->value() ? Utils::toGodotVec3(src->value()) : Vector3());
            return value;
        }
        default:
            return Ref<Resource>();
    }
}

Error TRTimeline::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_V_MSG(buf.is_empty(), ERR_CANT_OPEN, vformat("Couldn't load TRTML file: %s", file));
    flatbuffers::Verifier verifier(buf.ptr(), buf.size());
    ERR_FAIL_COND_V_MSG(!Titan::Animation::VerifyTRTMLBuffer(verifier), ERR_FILE_CORRUPT,
                        "Couldn't load TRTML flatbuffer");
    auto timeline = Titan::Animation::GetTRTML(buf.ptr());

    Array track_array;
    if (auto tracks = timeline->tracks())
    {
        for (auto track : *tracks)
        {
            Ref<TRTimelineTrack> track_obj;
            track_obj.instantiate();
            track_obj->set_Name(Utils::toGodotString(track->name()));

            Array group_array;
            if (auto groups = track->groups())
            {
                for (auto group : *groups)
                {
                    Ref<TRTimelineEventGroup> group_obj;
                    group_obj.instantiate();
                    Array event_array;
                    if (auto events = group->events())
                    {
                        for (auto event : *events)
                        {
                            Ref<TRTimelineEvent> event_obj;
                            event_obj.instantiate();
                            event_obj->set_track_name(Utils::toGodotString(event->track_name()));
                            event_obj->set_command(Utils::toGodotString(event->command()));
                            event_obj->set_begin_frame(event->begin_frame());
                            event_obj->set_end_frame(event->end_frame());
                            event_obj->set_flag_4(event->flag_4());
                            event_obj->set_kind(event->kind());

                            Array parameter_array;
                            if (auto parameters = event->parameters())
                            {
                                for (auto parameter : *parameters)
                                {
                                    Ref<TRTimelineParameter> parameter_obj;
                                    parameter_obj.instantiate();
                                    parameter_obj->set_Name(Utils::toGodotString(parameter->name()));
                                    parameter_obj->set_value(
                                        _LoadParameterValue(parameter->value_type(), parameter->value()));
                                    parameter_obj->set_flag_4(parameter->flag_4());
                                    parameter_obj->set_flag_5(parameter->flag_5());
                                    parameter_array.append(parameter_obj);
                                }
                            }
                            event_obj->set_parameters(parameter_array);
                            event_array.append(event_obj);
                        }
                    }
                    group_obj->set_events(event_array);
                    group_array.append(group_obj);
                }
            }
            track_obj->set_groups(group_array);

            PackedByteArray bytes;
            if (auto source = track->empty_vector())
            {
                bytes.resize(source->size());
                if (source->size() > 0)
                {
                    memcpy(bytes.ptrw(), source->data(), source->size());
                }
            }
            track_obj->set_empty_vector(bytes);
            track_array.append(track_obj);
        }
    }
    set_tracks(track_array);
    set_frame_count(timeline->frame_count());
    set_count_3(timeline->count_3());
    set_count_4(timeline->count_4());

    PackedStringArray paths;
    if (auto resources = timeline->resource_paths())
    {
        for (auto path : *resources)
        {
            paths.append(Utils::toGodotString(path));
        }
    }
    set_resource_paths(paths);
    return OK;
}

Variant ResourceFormatLoaderTRTML::_load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                                       int32_t p_cache_mode) const
{
    Ref<TRTimeline> timeline;
    timeline.instantiate();
    Error error = timeline->LoadFromFile(p_path);
    if (error != OK)
    {
        return error;
    }
    return timeline;
}

PackedStringArray ResourceFormatLoaderTRTML::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trtml");
    return exts;
}

bool ResourceFormatLoaderTRTML::_handles_type(const StringName& p_type) const
{
    return p_type == String("TRTimeline");
}
