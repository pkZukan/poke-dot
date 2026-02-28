#include "trmbf.h"

using namespace godot;

void ModelBuffer::_bind_methods() 
{
    GETTER_SETTER_BIND(ModelBuffer, IndexBuffers, Variant::PACKED_BYTE_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(ModelBuffer, VertexBuffers, Variant::PACKED_BYTE_ARRAY, PROPERTY_HINT_NONE)
}

void TRModelBuffer::_bind_methods() 
{
    GETTER_SETTER_BIND(TRModelBuffer, Buffers, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "ModelBuffer")
}

void TRModelBuffer::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
    ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMBF file: %s", file));
    
    auto model = Titan::Model::GetTRMBF(buf.ptr());
    ERR_FAIL_COND_MSG(model == NULL, vformat("Couldn't load TRMBF flatbuffer"));

    //Has multiple vert/ind buffers (for each LOD)
    auto buffers = model->buffers();
    for(int i = 0; i < buffers->size(); i++)
    {
        Ref<ModelBuffer> mb;
        mb.instantiate();
        Array indBuffer, vertBuffer;
        auto indicies = buffers->Get(i)->index_buffer();
        auto verts = buffers->Get(i)->vertex_buffer();
        for(int j = 0; j < indicies->size(); j++)
        {
            auto ind = indicies->Get(j)->buffer();
            auto vert = verts->Get(j)->buffer();
            PackedByteArray inds, verts;
            for(int k = 0; k < ind->size(); k++)
                inds.push_back(ind->Get(k));
            for(int k = 0; k < vert->size(); k++)
                verts.push_back(vert->Get(k));
            indBuffer.push_back(inds);
            vertBuffer.push_back(verts);
        }
        mb->set_IndexBuffers(indBuffer);
        mb->set_VertexBuffers(vertBuffer);

        Buffers.push_back(mb);
    }
}

Variant ResourceFormatLoaderTRMBF::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRModelBuffer> trmbf;
    trmbf.instantiate();
    trmbf->LoadFromFile(p_path);
    return trmbf;
}

PackedStringArray ResourceFormatLoaderTRMBF::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trmbf");

    return exts;
}

bool ResourceFormatLoaderTRMBF::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRModelBuffer");
}