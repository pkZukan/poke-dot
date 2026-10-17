#include "trlgt.h"
#include <cmath>

using namespace godot;

void TRLightCurve::_bind_methods() 
{
    GETTER_SETTER_BIND(TRLightCurve, keys, Variant::ARRAY, PROPERTY_HINT_NONE)
    ClassDB::bind_method(D_METHOD("sample", "time"), &TRLightCurve::sample);
}

void TRLightCurve4::_bind_methods() 
{
    GETTER_SETTER_BIND(TRLightCurve4, x, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve")
    GETTER_SETTER_BIND(TRLightCurve4, y, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve")
    GETTER_SETTER_BIND(TRLightCurve4, z, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve")
    GETTER_SETTER_BIND(TRLightCurve4, w, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve")
    ClassDB::bind_method(D_METHOD("sample", "time"), &TRLightCurve4::sample);
}

void BoundMatrix::_bind_methods()
{
    GETTER_SETTER_BIND(BoundMatrix, x, Variant::VECTOR4, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoundMatrix, y, Variant::VECTOR4, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(BoundMatrix, z, Variant::VECTOR4, PROPERTY_HINT_NONE)
}

void TRFloatParameter::_bind_methods()
{
    GETTER_SETTER_BIND(TRFloatParameter, param_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRFloatParameter, param_val, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRFloatParameter, curve, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve")
    ClassDB::bind_method(D_METHOD("sample", "time"), &TRFloatParameter::sample);
}

void TRVec4Parameter::_bind_methods()
{
    GETTER_SETTER_BIND(TRVec4Parameter, param_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRVec4Parameter, value, Variant::VECTOR4, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRVec4Parameter, curve, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve4")
    ClassDB::bind_method(D_METHOD("sample", "time"), &TRVec4Parameter::sample);
}

void TRStringParameter::_bind_methods()
{
    GETTER_SETTER_BIND(TRStringParameter, map_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRStringParameter, map_file, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRLight::_bind_methods()
{
    GETTER_SETTER_BIND(TRLight, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRLight, Type, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRLight, Matrix, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "BoundMatrix")
    GETTER_SETTER_BIND(TRLight, float_params, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRFloatParameter")
    GETTER_SETTER_BIND(TRLight, vec4_params, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRVec4Parameter")
    GETTER_SETTER_BIND(TRLight, string_params, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRStringParameter")
    GETTER_SETTER_BIND(TRLight, scale_curve, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve4")
    GETTER_SETTER_BIND(TRLight, rotation_curve, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve4")
    GETTER_SETTER_BIND(TRLight, translation_curve, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "TRLightCurve4")
    ClassDB::bind_method(D_METHOD("sample_matrix", "time"), &TRLight::sample_matrix);
}

void TRLGT::_bind_methods()
{
    GETTER_SETTER_BIND(TRLGT, version, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRLGT, lights, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRLight")
}

namespace 
{
Ref<TRLightCurve> read_curve(const Titan::gfx2::LightCurve *source) 
{
    Ref<TRLightCurve> result;
    if (!source) return result;
    result.instantiate();
    Array keys;
    if (source->keys()) for (const auto *key : *source->keys()) {
        if (!key) continue;
        Dictionary entry;
        entry["time"] = key->time(); entry["value"] = key->value();
        entry["tangent_left"] = key->tangent_left(); entry["tangent_right"] = key->tangent_right();
        entry["interpolation"] = int(key->interpolation());
        keys.push_back(entry);
    }
    result->set_keys(keys);
    return result;
}

Ref<TRLightCurve4> read_curve4(const Titan::gfx2::LightCurve4 *source) 
{
    Ref<TRLightCurve4> result;
    if (!source) return result;
    result.instantiate();
    result->set_x(read_curve(source->x())); result->set_y(read_curve(source->y()));
    result->set_z(read_curve(source->z())); result->set_w(read_curve(source->w()));
    return result;
}

Ref<BoundMatrix> read_matrix(const Titan::gfx2::BoundMatrix *mat)
{
    Ref<BoundMatrix> matrix;
    if (!mat) return matrix;
    matrix.instantiate();
    auto x = mat->x();
    auto y = mat->y();
    auto z = mat->z();
    matrix->set_x(x ? Vector4(x->x(), x->y(), x->z(), 0) : Vector4(1, 1, 1, 0));
    if(y) matrix->set_y(Vector4(y->x(), y->y(), y->z(), 0));
    if(z) matrix->set_z(Vector4(z->x(), z->y(), z->z(), 0));
    return matrix;
}

Array read_float_params(const flatbuffers::Vector<flatbuffers::Offset<Titan::gfx2::FloatParameter>> *fps)
{
    Array fpArr;
    if (!fps) return fpArr;
    for (const auto *fp : *fps)
    {
        if(!fp) continue;
        Ref<TRFloatParameter> param;
        param.instantiate();
        param->set_param_name(Utils::toGodotString(fp->param_name()));
        param->set_param_val(fp->param_val());
        param->set_curve(read_curve(fp->curve()));
        fpArr.push_back(param);
    }
    return fpArr;
}

Array read_vec4_params(const flatbuffers::Vector<flatbuffers::Offset<Titan::gfx2::Vec4Parameter>> *vps)
{
    Array vpArr;
    if (!vps) return vpArr;
    for (const auto *vp : *vps)
    {
        if(!vp) continue;
        Ref<TRVec4Parameter> param;
        param.instantiate();
        param->set_param_name(Utils::toGodotString(vp->param_name()));
        auto v = vp->value();
        param->set_curve(read_curve4(vp->curve()));
        if(v) param->set_value(Vector4(v->x(), v->y(), v->z(), v->w()));
        vpArr.push_back(param);
    }
    return vpArr;
}
}

float TRLightCurve::sample(double time) const 
{
    if (keys.is_empty() || !std::isfinite(time)) 
        return 0;

    Dictionary last = keys[keys.size() - 1];
    if (keys.size() == 1 || time >= double(last.get("time", 0.0))) 
        return last.get("value", 0.0);

    int low = 0, high = keys.size() - 1;
    while (high - low > 1) 
    {
        int middle = (low + high) / 2;
        Dictionary key = keys[middle];
        if (double(key.get("time", 0.0)) <= time) 
            low = middle;
        else 
            high = middle;
    }

    Dictionary a = keys[low];
    int interpolation = a.get("interpolation", 0);
    double v0 = a.get("value", 0.0);
    if (interpolation == 0) 
        return v0;

    Dictionary b = keys[low + 1];
    double span = double(b.get("time", 0.0)) - double(a.get("time", 0.0));
    if (span <= 0) 
        return 0;

    double t = (time - double(a.get("time", 0.0))) / span;
    double v1 = b.get("value", 0.0);
    if (interpolation == 1) 
        return v0 + (v1 - v0) * t;
    
    double t2 = t * t, t3 = t2 * t;
    return (2*t3 - 3*t2 + 1)*v0 + (t3 - 2*t2 + t)*double(a.get("tangent_right", 0.0)) +
        (-2*t3 + 3*t2)*v1 + (t3 - t2)*double(b.get("tangent_left", 0.0));
}

Vector4 TRLightCurve4::sample(double time) const 
{
    return Vector4(x.is_valid() ? x->sample(time) : 0, y.is_valid() ? y->sample(time) : 0,
        z.is_valid() ? z->sample(time) : 0, w.is_valid() ? w->sample(time) : 0);
}

Ref<BoundMatrix> TRLight::sample_matrix(double time) const 
{
    if (Matrix.is_null() && scale_curve.is_null() && rotation_curve.is_null() && translation_curve.is_null()) return Ref<BoundMatrix>();
    Ref<BoundMatrix> result; result.instantiate();
    result->set_x(scale_curve.is_valid() ? scale_curve->sample(time) : (Matrix.is_valid() ? Matrix->get_x() : Vector4(1,1,1,0)));
    result->set_y(rotation_curve.is_valid() ? rotation_curve->sample(time) : (Matrix.is_valid() ? Matrix->get_y() : Vector4()));
    result->set_z(translation_curve.is_valid() ? translation_curve->sample(time) : (Matrix.is_valid() ? Matrix->get_z() : Vector4()));
    return result;
}

Error TRLGT::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_V_MSG(buf.is_empty(), ERR_CANT_OPEN, vformat("Couldn't load LGT file: %s", file));
    flatbuffers::Verifier verifier(buf.ptr(), buf.size());
    ERR_FAIL_COND_V_MSG(!Titan::gfx2::VerifyTRLGTBuffer(verifier), ERR_FILE_CORRUPT, "Invalid TRLGT buffer");
    auto lgt = Titan::gfx2::GetTRLGT(buf.ptr());

    set_version(lgt->version());

    auto lgt_lights = lgt->lights();
    if(!lgt_lights) { set_lights(Array()); return OK; }

    Array lightArr;
    for(int i = 0; i < lgt_lights->size(); i++)
    {
        auto l = lgt_lights->Get(i);
        if(!l) continue;

        Ref<TRLight> light;
        light.instantiate();
        light->set_Name(Utils::toGodotString(l->name()));
        light->set_Type(Utils::toGodotString(l->type()));

        light->set_Matrix(read_matrix(l->matrix()));

        if (auto curves = l->transform_curve()) {
            light->set_scale_curve(read_curve4(curves->scale()));
            light->set_rotation_curve(read_curve4(curves->rotation()));
            light->set_translation_curve(read_curve4(curves->translation()));
        }

        if (l->float_params()) light->set_float_params(read_float_params(l->float_params()));
        if (l->vec4_params()) light->set_vec4_params(read_vec4_params(l->vec4_params()));

        // String params
        auto sps = l->string_params();
        if(sps)
        {
            Array spArr;
            for(int j = 0; j < sps->size(); j++)
            {
                auto sp = sps->Get(j);
                if(!sp) continue;
                Ref<TRStringParameter> param;
                param.instantiate();
                param->set_map_name(Utils::toGodotString(sp->map_name()));
                param->set_map_file(Utils::toGodotString(sp->map_file()));
                spArr.push_back(param);
            }
            light->set_string_params(spArr);
        }

        lightArr.push_back(light);
    }
    set_lights(lightArr);
    return OK;
}

Variant ResourceFormatLoaderTRLGT::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRLGT> trlgt;
    trlgt.instantiate();
    Error error = trlgt->LoadFromFile(p_path);
    if (error != OK) return error;
    return trlgt;
}

PackedStringArray ResourceFormatLoaderTRLGT::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trlgt");

    return exts;
}

bool ResourceFormatLoaderTRLGT::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRLGT");
}

String ResourceFormatLoaderTRLGT::_get_resource_type(const String &path) const
{
    return path.get_extension().to_lower() == "trlgt" ? String("TRLGT") : String();
}
