#include "trmsh.h"

using namespace godot;

void MaterialInfo::_bind_methods()
{
    GETTER_SETTER_BIND(MaterialInfo, PolyCount, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialInfo, PolyOffset, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialInfo, unk0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialInfo, MaterialName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MaterialInfo, unk1, Variant::INT, PROPERTY_HINT_NONE)
}

void Sphere::_bind_methods()
{
    GETTER_SETTER_BIND(Sphere, X, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Sphere, Y, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Sphere, Z, Variant::FLOAT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Sphere, Radius, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void InfluenceEntry::_bind_methods()
{
    GETTER_SETTER_BIND(InfluenceEntry, Index, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(InfluenceEntry, Scale, Variant::FLOAT, PROPERTY_HINT_NONE)
}

void SizeTable::_bind_methods()
{
    GETTER_SETTER_BIND(SizeTable, Size, Variant::INT, PROPERTY_HINT_NONE)
}

void Accessors::_bind_methods()
{
    GETTER_SETTER_BIND(Accessors, attr_0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Accessors, Attribute, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Accessors, AttributeLayer, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Accessors, Type, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(Accessors, Position, Variant::INT, PROPERTY_HINT_NONE)
}

void VertexAccessors::_bind_methods()
{
    GETTER_SETTER_BIND(VertexAccessors, Accessors, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "Accessors")
    GETTER_SETTER_BIND(VertexAccessors, Strides, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "SizeTable")
}

void MeshShape::_bind_methods() 
{
    GETTER_SETTER_BIND(MeshShape, Name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MeshShape, BBox, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "BoundingBox")
    GETTER_SETTER_BIND(MeshShape, PolygonType, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(MeshShape, Attributes, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "VertexAccessors")
    GETTER_SETTER_BIND(MeshShape, Materials, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MaterialInfo")
    GETTER_SETTER_BIND(MeshShape, ClipSphere, Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "Sphere")
    GETTER_SETTER_BIND(MeshShape, Influences, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "InfluenceEntry")
    GETTER_SETTER_BIND(MeshShape, MeshName, Variant::STRING, PROPERTY_HINT_NONE)
}

void TRMesh::_bind_methods() 
{
    GETTER_SETTER_BIND(TRMesh, unk0, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMesh, BufferName, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TRMesh, MeshDescriptors, Variant::ARRAY, PROPERTY_HINT_ARRAY_TYPE, "MeshShape")
}

void TRMesh::LoadFromFile(String file)
{
    PackedByteArray buf = FileAccess::get_file_as_bytes(file);
	ERR_FAIL_COND_MSG(buf.is_empty(), vformat("Couldn't load TRMSH file: %s", file));

    auto mesh = Titan::Model::GetTRMSH(buf.ptr());
    ERR_FAIL_COND_MSG(mesh == NULL, vformat("Couldn't load TRMSH flatbuffer"));

    set_BufferName(Utils::toGodotString(mesh->buffer_name()));
    set_unk0(mesh->unk0());

    //Array of meshes
    auto meshShapes = mesh->meshes();
    for(int i = 0; i < meshShapes->size(); i++)
    {
        //Meshes have name and array of attributes
        Ref<MeshShape> mshape;
        mshape.instantiate();

        auto msh = meshShapes->Get(i);
        mshape->set_Name(Utils::toGodotString(msh->mesh_shape_name()));

        Ref<BoundingBox> bb;
        bb.instantiate();
        bb->set_Max(Utils::toGodotVec3(msh->bounds()->max()));
        bb->set_Min(Utils::toGodotVec3(msh->bounds()->min()));
        mshape->set_BBox(bb);
        mshape->set_PolygonType(msh->polygon_type());
        mshape->set_MeshName(Utils::toGodotString(msh->mesh_name()));

        //Get attributes
        auto attributes = msh->attributes();
        Array attribs;
        for(int j = 0; j < attributes->size(); j++)
        {
            Ref<VertexAccessors> att;
            att.instantiate();

            auto atts = attributes->Get(j)->attrs();
            Array accessors;
            for(int k = 0; k < atts->size(); k++)
            {
                Ref<Accessors> accs;
                accs.instantiate();
                accs->set_attr_0(atts->Get(k)->attr_0());
                accs->set_Attribute(Titan::Model::EnumNameVertexAttribute(atts->Get(k)->attribute()));
                accs->set_AttributeLayer(atts->Get(k)->attribute_layer());
                accs->set_Type(Titan::Model::EnumNameType(atts->Get(k)->type()));
                accs->set_Position(atts->Get(k)->position());
                accessors.push_back(accs);
            }
            att->set_Accessors(accessors);

            auto attSizes = attributes->Get(j)->size();
            Array strides;
            for(int k = 0; k < attSizes->size(); k++)
            {
                Ref<SizeTable> st;
                st.instantiate();
                st->set_Size(attSizes->Get(k)->size());
                strides.push_back(st);
            }
            att->set_Strides(strides);

            attribs.push_back(att);
        }
        mshape->set_Attributes(attribs);

        //Get Materials
        auto mats = msh->materials();
        Array matInfo;
        for(int j = 0; j < mats->size(); j++)
        {
            Ref<MaterialInfo> minfo;
            minfo.instantiate();
            minfo->set_PolyCount(mats->Get(j)->poly_count());
            minfo->set_PolyOffset(mats->Get(j)->poly_offset());
            minfo->set_unk0(mats->Get(j)->sh_unk3());
            minfo->set_MaterialName(Utils::toGodotString(mats->Get(j)->material_name()));
            minfo->set_unk1(mats->Get(j)->sh_unk4());
            matInfo.push_back(minfo);
        }
        mshape->set_Materials(matInfo);

        //Get Influences
        auto influences = msh->influence();
        Array influence;
        for(int j = 0; j < influences->size(); j++)
        {
            Ref<InfluenceEntry> infEnt;
            infEnt.instantiate();
            infEnt->set_Index(influences->Get(j)->index());
            infEnt->set_Scale(influences->Get(j)->scale());
            influence.push_back(infEnt);
        }
        mshape->set_Influences(influence);
        
        MeshDescriptors.push_back(mshape);
    }
}

Variant ResourceFormatLoaderTRMSH::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
    Ref<TRMesh> trmsh;
    trmsh.instantiate();
    trmsh->LoadFromFile(p_path);
    return trmsh;
}

PackedStringArray ResourceFormatLoaderTRMSH::_get_recognized_extensions() const
{
    PackedStringArray exts;
    exts.push_back("trmsh");

    return exts;
}

bool ResourceFormatLoaderTRMSH::_handles_type(const StringName &p_type) const 
{
    return p_type == String("TRMesh");
}