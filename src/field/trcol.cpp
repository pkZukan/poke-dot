#include "trcol.h"

using namespace godot;

void TRCOL::_bind_methods() 
{
	ClassDB::bind_method(D_METHOD("LoadFromFile", "file"), &TRCOL::LoadFromFile);
	ClassDB::bind_method(D_METHOD("get_tag"), &TRCOL::get_tag);
	ClassDB::bind_method(D_METHOD("get_mesh"), &TRCOL::get_mesh);
}

void TRCOL::LoadFromFile(String file)
{
	tag.instantiate();
	tag->LoadFromFile(file);
}

Ref<ArrayMesh> TRCOL::get_mesh()
{
	Ref<ArrayMesh> mesh;
	mesh.instantiate();

	ERR_FAIL_COND_V_MSG(tag.is_null(), mesh, "TRCOL: no tag loaded, call LoadFromFile first");

	Vector<HavokMeshSection> geometry_sections = tag->GetGeometrySections();

	for (int i = 0; i < geometry_sections.size(); i++)
	{
		const HavokMeshSection &section = geometry_sections[i];

		if (section.vertices.is_empty() || section.faceIndices.is_empty())
			continue;

		Array arrays;
		arrays.resize(Mesh::ARRAY_MAX);

		arrays[Mesh::ARRAY_VERTEX] = section.vertices;
		arrays[Mesh::ARRAY_INDEX] = section.faceIndices;

		mesh->add_surface_from_arrays(
			Mesh::PRIMITIVE_TRIANGLES,
			arrays
		);
	}

	return mesh;
}

Variant ResourceFormatLoaderTRCOL::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
	Ref<TRCOL> trcol;
	trcol.instantiate();
	trcol->LoadFromFile(p_path);
	return trcol;
}

PackedStringArray ResourceFormatLoaderTRCOL::_get_recognized_extensions() const
{
	PackedStringArray exts;
	exts.push_back("trcol");
	return exts;
}

bool ResourceFormatLoaderTRCOL::_handles_type(const StringName &p_type) const
{
	return p_type == StringName("TRCOL");
}