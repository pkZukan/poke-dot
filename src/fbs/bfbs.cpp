#include "bfbs.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cstring>

using namespace godot;

void FlatbufferSchema::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_root_type"), &FlatbufferSchema::get_root_type);
    ClassDB::bind_method(D_METHOD("decode", "data"), &FlatbufferSchema::decode);
    ClassDB::bind_method(D_METHOD("decode_file", "path"), &FlatbufferSchema::decode_file);
    ClassDB::bind_static_method("FlatbufferSchema", D_METHOD("decode_table", "path"), &FlatbufferSchema::decode_table);
}

namespace
{
constexpr int MAX_DEPTH = 64;

size_t scalar_size(reflection::BaseType type)
{
    switch (type)
    {
        case reflection::UType:
        case reflection::Bool:
        case reflection::Byte:
        case reflection::UByte:
            return 1;
        case reflection::Short:
        case reflection::UShort:
            return 2;
        case reflection::Int:
        case reflection::UInt:
        case reflection::Float:
            return 4;
        case reflection::Long:
        case reflection::ULong:
        case reflection::Double:
            return 8;
        default:
            return 0;
    }
}

String fb_string(const flatbuffers::String* str)
{
    return str ? String::utf8(str->c_str(), str->size()) : String();
}

class Reader
{
public:
    Reader(const reflection::Schema* p_schema, const uint8_t* p_data, size_t p_size)
        : schema(p_schema), data(p_data), size(p_size)
    {
    }

    Variant root() const
    {
        size_t table_pos;
        if (!deref(0, table_pos))
        {
            return Variant();
        }
        return table(schema->root_table(), table_pos, 0);
    }

private:
    const reflection::Schema* schema;
    const uint8_t* data;
    size_t size;

    bool in_range(size_t pos, size_t len) const
    {
        return pos <= size && len <= size - pos;
    }

    template <typename T> bool read(size_t pos, T& out) const
    {
        if (!in_range(pos, sizeof(T)))
        {
            return false;
        }

        memcpy(&out, data + pos, sizeof(T));
        return true;
    }

    bool deref(size_t pos, size_t& out) const
    {
        uint32_t offset;
        if (!read(pos, offset))
        {
            return false;
        }
        out = pos + offset;
        return out < size;
    }

    bool field_pos(size_t table_pos, uint16_t voffset, size_t& out) const
    {
        int32_t soffset;
        if (!read(table_pos, soffset))
        {
            return false;
        }
        const int64_t vtable = int64_t(table_pos) - soffset;
        uint16_t vtable_size, field_offset;
        if (vtable < 0 || !read(size_t(vtable), vtable_size) || voffset + sizeof(uint16_t) > vtable_size)
        {
            return false;
        }
        if (!read(size_t(vtable) + voffset, field_offset) || field_offset == 0)
        {
            return false;
        }
        out = table_pos + field_offset;
        return true;
    }

    Variant scalar(reflection::BaseType type, size_t pos) const
    {
        switch (type)
        {
            case reflection::Bool:
            {
                uint8_t v;
                return read(pos, v) ? Variant(v != 0) : Variant();
            }
            case reflection::Byte:
            {
                int8_t v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::UType:
            case reflection::UByte:
            {
                uint8_t v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::Short:
            {
                int16_t v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::UShort:
            {
                uint16_t v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::Int:
            {
                int32_t v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::UInt:
            {
                uint32_t v;
                return read(pos, v) ? Variant(int64_t(v)) : Variant();
            }
            case reflection::Long:
            {
                int64_t v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::ULong:
            {
                uint64_t v;
                return read(pos, v) ? Variant(int64_t(v)) : Variant();
            }
            case reflection::Float:
            {
                float v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            case reflection::Double:
            {
                double v;
                return read(pos, v) ? Variant(v) : Variant();
            }
            default:
                return Variant();
        }
    }

    Variant string(size_t pos) const
    {
        uint32_t len;
        if (!read(pos, len) || !in_range(pos + sizeof(uint32_t), len))
        {
            return Variant();
        }
        return String::utf8(reinterpret_cast<const char*>(data + pos + sizeof(uint32_t)), len);
    }

    Variant table(const reflection::Object* object, size_t pos, int depth) const
    {
        if (!object || depth > MAX_DEPTH)
        {
            return Variant();
        }
        Dictionary out;
        for (const reflection::Field* field : *object->fields())
        {
            if (field->deprecated())
            {
                continue;
            }
            const reflection::Type* type = field->type();
            const reflection::BaseType base = type->base_type();
            const String name = fb_string(field->name());
            size_t fpos;
            const bool present = field_pos(pos, field->offset(), fpos);

            if (base == reflection::UType)
            {
                continue; // emitted alongside its union
            }
            if (scalar_size(base) > 0)
            {
                if (present)
                {
                    out[name] = scalar(base, fpos);
                }
                else if (base == reflection::Bool)
                {
                    out[name] = field->default_integer() != 0;
                }
                else if (base == reflection::Float || base == reflection::Double)
                {
                    out[name] = field->default_real();
                }
                else
                {
                    out[name] = field->default_integer();
                }
                continue;
            }
            if (!present)
            {
                continue;
            }

            size_t target;
            switch (base)
            {
                case reflection::String:
                    if (deref(fpos, target))
                    {
                        out[name] = string(target);
                    }
                    break;
                case reflection::Obj:
                {
                    const reflection::Object* sub = schema->objects()->Get(type->index());
                    if (sub->is_struct())
                    {
                        out[name] = structure(sub, fpos, depth + 1);
                    }
                    else if (deref(fpos, target))
                    {
                        out[name] = table(sub, target, depth + 1);
                    }
                    break;
                }
                case reflection::Vector:
                    if (deref(fpos, target))
                    {
                        out[name] = vector(type, target, depth + 1);
                    }
                    break;
                case reflection::Union:
                {
                    size_t tag_pos;
                    uint8_t tag = 0;
                    if (!field_pos(pos, field->offset() - sizeof(uint16_t), tag_pos) || !read(tag_pos, tag) || tag == 0)
                    {
                        break;
                    }
                    const reflection::EnumVal* member = schema->enums()->Get(type->index())->values()->LookupByKey(tag);
                    if (!member || !member->union_type() || !deref(fpos, target))
                    {
                        break;
                    }
                    out[name + String("_type")] = fb_string(member->name());
                    if (member->union_type()->base_type() == reflection::String)
                    {
                        out[name] = string(target);
                    }
                    else
                    {
                        out[name] = table(schema->objects()->Get(member->union_type()->index()), target, depth + 1);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        return out;
    }

    Variant structure(const reflection::Object* object, size_t pos, int depth) const
    {
        if (!object || depth > MAX_DEPTH || !in_range(pos, object->bytesize()))
        {
            return Variant();
        }
        Dictionary out;
        for (const reflection::Field* field : *object->fields())
        {
            const reflection::Type* type = field->type();
            const size_t fpos = pos + field->offset();
            const String name = fb_string(field->name());
            if (scalar_size(type->base_type()) > 0)
            {
                out[name] = scalar(type->base_type(), fpos);
            }
            else if (type->base_type() == reflection::Obj)
            {
                out[name] = structure(schema->objects()->Get(type->index()), fpos, depth + 1);
            }
            else if (type->base_type() == reflection::Array)
            {
                const bool is_struct = type->element() == reflection::Obj;
                const reflection::Object* sub = is_struct ? schema->objects()->Get(type->index()) : nullptr;
                const size_t element_size = is_struct ? sub->bytesize() : scalar_size(type->element());
                Array values;
                for (uint32_t i = 0; i < type->fixed_length(); i++)
                {
                    values.push_back(is_struct ? structure(sub, fpos + i * element_size, depth + 1)
                                               : scalar(type->element(), fpos + i * element_size));
                }
                out[name] = values;
            }
        }
        return out;
    }

    Variant vector(const reflection::Type* type, size_t pos, int depth) const
    {
        uint32_t len;
        if (depth > MAX_DEPTH || !read(pos, len))
        {
            return Variant();
        }
        const size_t elements = pos + sizeof(uint32_t);
        const reflection::BaseType element = type->element();

        if (element == reflection::UByte || element == reflection::Byte)
        {
            PackedByteArray bytes;
            if (!in_range(elements, len))
            {
                return Variant();
            }
            bytes.resize(len);
            memcpy(bytes.ptrw(), data + elements, len);
            return bytes;
        }

        const reflection::Object* sub = element == reflection::Obj ? schema->objects()->Get(type->index()) : nullptr;
        const bool inline_struct = sub && sub->is_struct();
        const size_t element_size = inline_struct              ? sub->bytesize()
                                    : scalar_size(element) > 0 ? scalar_size(element)
                                                               : sizeof(uint32_t);
        if (element_size == 0 || len > (size - elements) / element_size)
        {
            return Variant();
        }

        Array out;
        for (uint32_t i = 0; i < len; i++)
        {
            const size_t epos = elements + i * element_size;
            size_t target;
            if (scalar_size(element) > 0)
            {
                out.push_back(scalar(element, epos));
            }
            else if (element == reflection::String)
            {
                out.push_back(deref(epos, target) ? string(target) : Variant());
            }
            else if (inline_struct)
            {
                out.push_back(structure(sub, epos, depth + 1));
            }
            else if (sub)
            {
                out.push_back(deref(epos, target) ? table(sub, target, depth + 1) : Variant());
            }
            else
            {
                out.push_back(Variant());
            }
        }
        return out;
    }
};
}

Error FlatbufferSchema::LoadFromFile(const String& file)
{
    PackedByteArray bytes = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_V_MSG(bytes.is_empty(), ERR_FILE_CANT_OPEN, "Couldn't read BFBS file: " + file);
    flatbuffers::Verifier verifier(bytes.ptr(), bytes.size());
    ERR_FAIL_COND_V_MSG(!reflection::VerifySchemaBuffer(verifier), ERR_FILE_CORRUPT, "Invalid BFBS file: " + file);
    schema_data = bytes;
    return OK;
}

const reflection::Schema* FlatbufferSchema::get_schema() const
{
    return schema_data.is_empty() ? nullptr : reflection::GetSchema(schema_data.ptr());
}

String FlatbufferSchema::get_root_type() const
{
    const reflection::Schema* schema = get_schema();
    return schema && schema->root_table() ? fb_string(schema->root_table()->name()) : String();
}

Variant FlatbufferSchema::decode(const PackedByteArray& data) const
{
    const reflection::Schema* schema = get_schema();
    ERR_FAIL_COND_V_MSG(!schema || !schema->root_table(), Variant(), "Schema has no root table");
    return Reader(schema, data.ptr(), data.size()).root();
}

Variant FlatbufferSchema::decode_file(const String& path) const
{
    PackedByteArray bytes = FileAccess::get_file_as_bytes(path);
    ERR_FAIL_COND_V_MSG(bytes.is_empty(), Variant(), "Couldn't read flatbuffer file: " + path);
    return decode(bytes);
}

Variant FlatbufferSchema::decode_table(const String& path)
{
    Ref<FlatbufferSchema> schema = ResourceLoader::get_singleton()->load(path.get_basename() + ".bfbs");
    ERR_FAIL_COND_V_MSG(schema.is_null(), Variant(), "No schema next to " + path);
    return schema->decode_file(path);
}

PackedStringArray ResourceFormatLoaderBFBS::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("bfbs");
    return exts;
}

bool ResourceFormatLoaderBFBS::_handles_type(const StringName& p_type) const
{
    return p_type == StringName("FlatbufferSchema");
}

String ResourceFormatLoaderBFBS::_get_resource_type(const String& p_path) const
{
    return p_path.get_extension().to_lower() == "bfbs" ? String("FlatbufferSchema") : String();
}

Variant ResourceFormatLoaderBFBS::_load(const String& p_path, const String& p_original_path, bool p_use_sub_threads,
                                        int32_t p_cache_mode) const
{
    Ref<FlatbufferSchema> schema;
    schema.instantiate();
    Error error = schema->LoadFromFile(p_path);
    if (error != OK)
    {
        return error;
    }
    return schema;
}
