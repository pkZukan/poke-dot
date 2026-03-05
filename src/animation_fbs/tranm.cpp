#include "tranm.h"

#define VECTOR_TRACK "FixedVectorTrack,DynamicVectorTrack,Framed16VectorTrack,Framed8VectorTrack"
#define ROTATION_TRACK "FixedRotationTrack,DynamicRotationTrack,Framed16RotationTrack,Framed8RotationTrack"

using namespace godot;

void Framed8RotationTrack::_bind_methods()
{
    GETTER_SETTER_BIND(Framed8RotationTrack, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(Framed8RotationTrack, co, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Quaternion")
}

void Framed16RotationTrack::_bind_methods()
{
    GETTER_SETTER_BIND(Framed16RotationTrack, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(Framed16RotationTrack, co, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Quaternion")
}

void DynamicRotationTrack::_bind_methods()
{
    GETTER_SETTER_BIND(DynamicRotationTrack, co, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Quaternion")
}

void FixedRotationTrack::_bind_methods()
{
    GETTER_SETTER_BIND(FixedRotationTrack, co, Variant::QUATERNION, PROPERTY_HINT_NONE)
}

void Framed8VectorTrack::_bind_methods()
{
    GETTER_SETTER_BIND(Framed8VectorTrack, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(Framed8VectorTrack, co, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Vector3")
}

void Framed16VectorTrack::_bind_methods()
{
    GETTER_SETTER_BIND(Framed16VectorTrack, frames, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "int")
    GETTER_SETTER_BIND(Framed16VectorTrack, co, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Vector3")
}

void DynamicVectorTrack::_bind_methods()
{
    GETTER_SETTER_BIND(DynamicVectorTrack, co, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Vector3i")
}

void FixedVectorTrack::_bind_methods()
{
    GETTER_SETTER_BIND(FixedVectorTrack, co, Variant::VECTOR3I, PROPERTY_HINT_NONE)
}

void BoneTrack::_bind_methods()
{
    GETTER_SETTER_BIND(BoneTrack, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneTrack, scale, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, VECTOR_TRACK)
    GETTER_SETTER_BIND(BoneTrack, rotate, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, ROTATION_TRACK)
    GETTER_SETTER_BIND(BoneTrack, translate, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, VECTOR_TRACK)
}

void BoneInit::_bind_methods()
{
    GETTER_SETTER_BIND(BoneInit, isInit, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoneInit, transform, Variant::TRANSFORM3D, PROPERTY_HINT_NONE)
}

void BoneAnimation::_bind_methods()
{
    GETTER_SETTER_BIND(BoneAnimation, tracks, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "BoneTrack")
    GETTER_SETTER_BIND(BoneAnimation, initData, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "BoneInit")
}

void AnimationInfo::_bind_methods()
{
    GETTER_SETTER_BIND(AnimationInfo, does_loop, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationInfo, animation_count, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(AnimationInfo, animation_rate, Variant::INT, PROPERTY_HINT_NONE)
}

void TRAnimation::_bind_methods() 
{
    GETTER_SETTER_BIND(TRAnimation, Info, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "AnimationInfo")
    GETTER_SETTER_BIND(TRAnimation, Track, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "BoneAnimation")
}

float QuaternionHelper::expand_float(int32_t i) {
    return i * (PI_HALF / SCALE) - PI_QUARTER;
}

Quaternion QuaternionHelper::Unpack(const Vector3i &vec) {
    int32_t x = vec.x & 0xFFFF;
    int32_t y = vec.y & 0xFFFF;
    int32_t z = vec.z & 0xFFFF;
    
    int64_t pack = (static_cast<int64_t>(z) << 32) | (y << 16) | x;
    
    // Extract components
    float q1 = expand_float(static_cast<int32_t>((pack >> 3) & 0x7FFF));
    float q2 = expand_float(static_cast<int32_t>((pack >> 18) & 0x7FFF));
    float q3 = expand_float(static_cast<int32_t>((pack >> 33) & 0x7FFF));
    
    float sum = q1 * q1 + q2 * q2 + q3 * q3;
    float missing = Math::sqrt(Math::max(1.0f - sum, 0.0f));
    
    // Bit 2 (0b0100) is the sign of the missing component
    if ((pack & 0b0100) != 0) {
        missing = -missing;
    }
    
    int32_t m_idx = pack & 0b0011;
    Quaternion q;
    
    // Properly map components based on which was dropped
    switch (m_idx) {
        case 0: q = Quaternion(missing, q1, q2, q3); break;
        case 1: q = Quaternion(q1, missing, q2, q3); break;
        case 2: q = Quaternion(q1, q2, missing, q3); break;
        case 3: q = Quaternion(q1, q2, q3, missing); break;
        default: q = Quaternion(); break;
    }
    
    return q.normalized();
}

Ref<Resource> TRAnimation::ParseVectorTrack(Titan::Animation::VectorTrack type, const void* data)
{
    switch (type) {
        case Titan::Animation::VectorTrack_FixedVectorTrack: {
            auto src = static_cast<const Titan::Animation::FixedVectorTrack*>(data);
            Ref<FixedVectorTrack> t; 
            t.instantiate();
            t->set_co(Utils::toGodotVec3(src->co()));
            return t;
        }
        case Titan::Animation::VectorTrack_DynamicVectorTrack: {
            auto src = static_cast<const Titan::Animation::DynamicVectorTrack*>(data);
            Ref<DynamicVectorTrack> t; 
            t.instantiate();
            Array co;
            for (size_t i = 0; i < src->co()->size(); i++)
                co.append(Utils::toGodotVec3(src->co()->Get(i)));
            t->set_co(co);
            return t;
        }
        case Titan::Animation::VectorTrack_Framed16VectorTrack: {
            auto src = static_cast<const Titan::Animation::Framed16VectorTrack*>(data);
            Ref<Framed16VectorTrack> t; 
            t.instantiate();
            Array frames, co;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->co()->size(); i++)
                co.append(Utils::toGodotVec3(src->co()->Get(i)));
            t->set_frames(frames); t->set_co(co);
            return t;
        }
        case Titan::Animation::VectorTrack_Framed8VectorTrack: {
            auto src = static_cast<const Titan::Animation::Framed8VectorTrack*>(data);
            Ref<Framed8VectorTrack> t; 
            t.instantiate();
            Array frames, co;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->co()->size(); i++)
                co.append(Utils::toGodotVec3(src->co()->Get(i)));
            t->set_frames(frames); t->set_co(co);
            return t;
        }
        default: return Ref<Resource>();
    }
}

Ref<Resource> TRAnimation::ParseRotationTrack(Titan::Animation::RotationTrack type, const void* data)
{
    switch (type) {
        case Titan::Animation::RotationTrack_FixedRotationTrack: 
        {
            auto src = static_cast<const Titan::Animation::FixedRotationTrack*>(data);
            Ref<FixedRotationTrack> t; 
            t.instantiate();
            Quaternion quat = QuaternionHelper::Unpack(Vector3i(src->co()->x(), src->co()->y(), src->co()->z()));
            t->set_co(quat);
            return t;
        }
        case Titan::Animation::RotationTrack_DynamicRotationTrack: 
        {
            auto src = static_cast<const Titan::Animation::DynamicRotationTrack*>(data);
            Ref<DynamicRotationTrack> t; 
            t.instantiate();
            Array co;
            for (size_t i = 0; i < src->co()->size(); i++) 
            {
                auto v = src->co()->Get(i);
                Quaternion quat = QuaternionHelper::Unpack(Vector3i(v->x(), v->y(), v->z()));
                co.append(quat);
            }
            t->set_co(co);
            return t;
        }
        case Titan::Animation::RotationTrack_Framed16RotationTrack: 
        {
            auto src = static_cast<const Titan::Animation::Framed16RotationTrack*>(data);
            Ref<Framed16RotationTrack> t; 
            t.instantiate();
            Array frames, co;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->co()->size(); i++) 
            {
                auto v = src->co()->Get(i);
                Quaternion quat = QuaternionHelper::Unpack(Vector3i(v->x(), v->y(), v->z()));
                co.append(quat);
            }
            t->set_frames(frames); t->set_co(co);
            return t;
        }
        case Titan::Animation::RotationTrack_Framed8RotationTrack: 
        {
            auto src = static_cast<const Titan::Animation::Framed8RotationTrack*>(data);
            Ref<Framed8RotationTrack> t; 
            t.instantiate();
            Array frames, co;
            for (size_t i = 0; i < src->frames()->size(); i++)
                frames.append(src->frames()->Get(i));
            for (size_t i = 0; i < src->co()->size(); i++) 
            {
                auto v = src->co()->Get(i);
                Quaternion quat = QuaternionHelper::Unpack(Vector3i(v->x(), v->y(), v->z()));
                co.append(quat);
            }
            t->set_frames(frames); t->set_co(co);
            return t;
        }
        default: 
            return Ref<Resource>();
    }
}

void TRAnimation::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRANM file: %s", file));
    auto anim = Titan::Animation::GetTRANM(buf.ptr());
    ERR_FAIL_COND_MSG(anim == nullptr, "Couldn't load TRANM flatbuffer");

    // Get Info
    auto info = anim->info();
    Ref<AnimationInfo> animInfo;
    animInfo.instantiate();
    animInfo->set_does_loop(info->does_loop());
    animInfo->set_animation_count(info->animation_count());
    animInfo->set_animation_rate(info->animation_rate());
    set_Info(animInfo);

    // Get BoneAnimation
    auto track = anim->track();
    Ref<BoneAnimation> boneAnimation;
    boneAnimation.instantiate();

    // Parse BoneInit
    auto initData = track->initData();
    if (initData) 
    {
        Ref<BoneInit> boneInit;
        boneInit.instantiate();
        boneInit->set_isInit(initData->isInit());
        boneInit->set_transform(Utils::toGodotTransform(initData->transform()));
        boneAnimation->set_initData(boneInit);
    }

    // Parse BoneTracks
    auto tracks = track->tracks();
    Array trackArray;
    if (tracks) {
        for (size_t i = 0; i < tracks->size(); i++) {
            auto boneTrack = tracks->Get(i);
            Ref<BoneTrack> bt;
            bt.instantiate();
            bt->set_Name(boneTrack->bone_name()->c_str());

            bt->set_scale(ParseVectorTrack(boneTrack->scale_type(), boneTrack->scale()));
            bt->set_rotate(ParseRotationTrack(boneTrack->rotate_type(), boneTrack->rotate()));
            bt->set_translate(ParseVectorTrack(boneTrack->translate_type(), boneTrack->translate()));

            trackArray.append(bt);
        }
    }
    boneAnimation->set_tracks(trackArray);
    set_Track(boneAnimation);
}

Variant ResourceFormatLoaderTRANM::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRAnimation> tranm;
    tranm.instantiate();
    tranm->LoadFromFile(p_path);
    return tranm;
}

PackedStringArray ResourceFormatLoaderTRANM::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("tranm");

    return exts;
}

bool ResourceFormatLoaderTRANM::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRAnimation");
}