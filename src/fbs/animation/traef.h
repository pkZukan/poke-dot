#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/traef_generated.h"
#include <utils.h>

namespace godot
{

class TREffectValueInt : public Resource
{
    GDCLASS(TREffectValueInt, Resource)
protected:
    static void _bind_methods();

public:
    TREffectValueInt() = default;
    ~TREffectValueInt() = default;

    GETTER_SETTER_DEFINE(uint32_t, value)

private:
    uint32_t value = 0;
};

class TREffectValueFloat : public Resource
{
    GDCLASS(TREffectValueFloat, Resource)
protected:
    static void _bind_methods();

public:
    TREffectValueFloat() = default;
    ~TREffectValueFloat() = default;

    GETTER_SETTER_DEFINE(float, value)

private:
    float value = 0.0f;
};

class TREffectValueByte : public Resource
{
    GDCLASS(TREffectValueByte, Resource)
protected:
    static void _bind_methods();

public:
    TREffectValueByte() = default;
    ~TREffectValueByte() = default;

    GETTER_SETTER_DEFINE(uint8_t, value)

private:
    uint8_t value = 0;
};

class TREffectValueString : public Resource
{
    GDCLASS(TREffectValueString, Resource)
protected:
    static void _bind_methods();

public:
    TREffectValueString() = default;
    ~TREffectValueString() = default;

    GETTER_SETTER_DEFINE(String, value)

private:
    String value;
};

class TREffect : public Resource
{
    GDCLASS(TREffect, Resource)
protected:
    static void _bind_methods();

public:
    TREffect() = default;
    ~TREffect() = default;

    GETTER_SETTER_DEFINE(String, Name)
    GETTER_SETTER_DEFINE(Ref<Resource>, value)

private:
    String Name;
    Ref<Resource> value;
};

class TRAnimationEffect : public Resource
{
    GDCLASS(TRAnimationEffect, Resource)
protected:
    static void _bind_methods();

public:
    TRAnimationEffect() = default;
    ~TRAnimationEffect() = default;

    GETTER_SETTER_DEFINE(uint32_t, res_0)
    GETTER_SETTER_DEFINE(String, path)
    GETTER_SETTER_DEFINE(uint32_t, start)
    GETTER_SETTER_DEFINE(uint32_t, duration)
    GETTER_SETTER_DEFINE(Array, effects)

private:
    uint32_t res_0 = 0;
    String path;
    uint32_t start = 0;
    uint32_t duration = 0;
    Array effects;
};

class TRAnimationEffects : public Resource
{
    GDCLASS(TRAnimationEffects, Resource)
protected:
    static void _bind_methods();

public:
    TRAnimationEffects() = default;
    ~TRAnimationEffects() = default;

    void LoadFromFile(String file);
    Array get_events(double frame_rate = 0.0) const;

    GETTER_SETTER_DEFINE(Array, effects)

private:
    Ref<Resource> _LoadEffectValue(Ikkaku::Animation::EffectValue type, const void* data);

    Array effects;
};

class ResourceFormatLoaderTRAEF : public ResourceFormatLoader
{
    GDCLASS(ResourceFormatLoaderTRAEF, ResourceFormatLoader)
protected:
    static void _bind_methods()
    {
    }

public:
    ResourceFormatLoaderTRAEF() = default;
    ~ResourceFormatLoaderTRAEF() = default;

    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual bool _handles_type(const StringName& p_type) const override;
    virtual Variant _load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                          int32_t p_cache_mode) const override;
};

}
