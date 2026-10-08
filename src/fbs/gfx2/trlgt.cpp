#include "trlgt.h"

using namespace godot;

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
}

void TRVec4Parameter::_bind_methods()
{
    GETTER_SETTER_BIND(TRVec4Parameter, param_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRVec4Parameter, value, Variant::VECTOR4, PROPERTY_HINT_NONE)
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
}

void TRLGT::_bind_methods()
{
    GETTER_SETTER_BIND(TRLGT, version, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRLGT, lights, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "TRLight")
}

void TRLGT::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load LGT file: %s", file));
    auto lgt = Titan::gfx2::GetTRLGT(buf.ptr());
    ERR_FAIL_COND_MSG(lgt == nullptr, "Couldn't parse LGT");

    set_version(lgt->version());

    auto lgt_lights = lgt->lights();
    if(!lgt_lights) return;

    Array lightArr;
    for(int i = 0; i < lgt_lights->size(); i++)
    {
        auto l = lgt_lights->Get(i);
        if(!l) continue;

        Ref<TRLight> light;
        light.instantiate();
        light->set_Name(Utils::toGodotString(l->name()));
        light->set_Type(Utils::toGodotString(l->type()));

        // Matrix
        auto mat = l->matrix();
        if(mat)
        {
            Ref<BoundMatrix> matrix;
            matrix.instantiate();
            auto x = mat->x();
            auto y = mat->y();
            auto z = mat->z();
            if(x) matrix->set_x(Vector4(x->x(), x->y(), x->z(), x->w()));
            if(y) matrix->set_y(Vector4(y->x(), y->y(), y->z(), y->w()));
            if(z) matrix->set_z(Vector4(z->x(), z->y(), z->z(), z->w()));
            light->set_Matrix(matrix);
        }

        // Float params
        auto fps = l->float_params();
        if(fps)
        {
            Array fpArr;
            for(int j = 0; j < fps->size(); j++)
            {
                auto fp = fps->Get(j);
                if(!fp) continue;
                Ref<TRFloatParameter> param;
                param.instantiate();
                param->set_param_name(Utils::toGodotString(fp->param_name()));
                param->set_param_val(fp->param_val());
                fpArr.push_back(param);
            }
            light->set_float_params(fpArr);
        }

        // Vec4 params
        auto vps = l->vec4_params();
        if(vps)
        {
            Array vpArr;
            for(int j = 0; j < vps->size(); j++)
            {
                auto vp = vps->Get(j);
                if(!vp) continue;
                Ref<TRVec4Parameter> param;
                param.instantiate();
                param->set_param_name(Utils::toGodotString(vp->param_name()));
                auto v = vp->value();
                if(v) param->set_value(Vector4(v->x(), v->y(), v->z(), v->w()));
                vpArr.push_back(param);
            }
            light->set_vec4_params(vpArr);
        }

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
}

Variant ResourceFormatLoaderTRLGT::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRLGT> trlgt;
    trlgt.instantiate();
    trlgt->LoadFromFile(p_path);
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