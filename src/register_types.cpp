#include "register_types.h"

#include "model_fbs/trmdl.h"
#include "model_fbs/trmsh.h"
#include "model_fbs/trmbf.h"
#include "model_fbs/trskl.h"
#include "model_fbs/trmtr.h"
#include "model_fbs/trmmt.h"

#include "animation_fbs/tracn.h"
#include "animation_fbs/tranm.h"

#include "catalog_fbs/trpmcatalog.h"

#include "middleware/bntx.h"

#include "utils.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>

using namespace godot;

Ref<ResourceFormatLoaderTRMDL> resource_loader_trmdl;
Ref<ResourceFormatLoaderTRMSH> resource_loader_trmsh;
Ref<ResourceFormatLoaderTRMBF> resource_loader_trmbf;
Ref<ResourceFormatLoaderTRSKL> resource_loader_trskl;
Ref<ResourceFormatLoaderTRMTR> resource_loader_trmtr;
Ref<ResourceFormatLoaderTRMMT> resource_loader_trmmt;

Ref<ResourceFormatLoaderTRACN> resource_loader_tracn;
Ref<ResourceFormatLoaderTRANM> resource_loader_tranm;

Ref<ResourceFormatLoaderTRPMCATALOG> resource_loader_trpmcatalog;

Ref<ResourceFormatLoaderBNTX> resource_loader_bntx;

void initialize_gen_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) 
	{
		//model fbs
		GDREGISTER_CLASS(MeshShape)
		GDREGISTER_CLASS(ModelBuffer)
		GDREGISTER_CLASS(VertexAccessors)
		GDREGISTER_CLASS(SizeTable)
		GDREGISTER_CLASS(Accessors)
		GDREGISTER_CLASS(BoneEntry)
		GDREGISTER_CLASS(MaterialEntry)
		GDREGISTER_CLASS(TextureEntry)
		GDREGISTER_CLASS(ShaderEntry)
		GDREGISTER_CLASS(SamplerEntry)
		GDREGISTER_CLASS(ShaderStringParam)
		GDREGISTER_CLASS(ShaderIntParam)
		GDREGISTER_CLASS(MaterialInfo)
		GDREGISTER_CLASS(Sphere)
		GDREGISTER_CLASS(InfluenceEntry)
		GDREGISTER_CLASS(TransformNode)
		GDREGISTER_CLASS(IKControl)
		GDREGISTER_CLASS(LOD)
		GDREGISTER_CLASS(LodIndex)
		GDREGISTER_CLASS(ByteExtra)
		GDREGISTER_CLASS(IntExtra)
		GDREGISTER_CLASS(FloatParameter)
		GDREGISTER_CLASS(Float4Parameter)
		GDREGISTER_CLASS(MaterialTable)
		GDREGISTER_CLASS(MaterialProperty)
		GDREGISTER_CLASS(MaterialSwitch)

		//Model
		GDREGISTER_CLASS(TRModel)
		GDREGISTER_CLASS(TRMesh)
		GDREGISTER_CLASS(TRModelBuffer)
		GDREGISTER_CLASS(TRSkeleton)
		GDREGISTER_CLASS(TRModelMaterialTable)
		GDREGISTER_CLASS(TRMaterial)

		//tranm
		GDREGISTER_CLASS(AnimationInfo)
		GDREGISTER_CLASS(BoneInit)
		GDREGISTER_CLASS(FixedVectorTrack)
		GDREGISTER_CLASS(DynamicVectorTrack)
		GDREGISTER_CLASS(Framed16VectorTrack)
		GDREGISTER_CLASS(Framed8VectorTrack)
		GDREGISTER_CLASS(FixedRotationTrack)
		GDREGISTER_CLASS(DynamicRotationTrack)
		GDREGISTER_CLASS(Framed16RotationTrack)
		GDREGISTER_CLASS(Framed8RotationTrack)
		GDREGISTER_CLASS(BoneTrack)
		GDREGISTER_CLASS(BoneAnimation)
		GDREGISTER_CLASS(TRAnimation)

		//tracn
		GDREGISTER_CLASS(TRAnimationChannelNames)
		GDREGISTER_CLASS(AnimationEntry)

		//catalog
		GDREGISTER_CLASS(TRPpokemonCatalog)
		GDREGISTER_CLASS(CatalogEntry)
		GDREGISTER_CLASS(SpeciesResourceInfo)
		GDREGISTER_CLASS(AnimationResourceInfo)
		GDREGISTER_CLASS(LocatorResourceInfo)

		//middleware
		GDREGISTER_CLASS(BinaryTexture)

		GDREGISTER_CLASS(Utils)

		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRMDL)
		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRMSH)
		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRMBF)
		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRSKL)
		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRMTR)
		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRMMT)

		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRACN)
		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRANM)

		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderTRPMCATALOG)

		GDREGISTER_ABSTRACT_CLASS(ResourceFormatLoaderBNTX)

		resource_loader_trmdl.instantiate();
		resource_loader_trmsh.instantiate();
		resource_loader_trmbf.instantiate();
		resource_loader_trskl.instantiate();
		resource_loader_trmtr.instantiate();
		resource_loader_trmmt.instantiate();

		resource_loader_tracn.instantiate();
		resource_loader_tranm.instantiate();

		resource_loader_trpmcatalog.instantiate();

		resource_loader_bntx.instantiate();

		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trmdl);
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trmsh);
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trmbf);
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trskl);
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trmtr);
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trmmt);

		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_tracn);
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_tranm);

		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_trpmcatalog);

		ResourceLoader::get_singleton()->add_resource_format_loader(resource_loader_bntx);
	}
	
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) 
	{
		//
	}
#endif
}

void uninitialize_gen_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) return;

	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trmdl);
	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trmsh);
	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trmbf);
	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trskl);
	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trmtr);
	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trmmt);

	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_tracn);
	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_tranm);

	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_trpmcatalog);

	ResourceLoader::get_singleton()->remove_resource_format_loader(resource_loader_bntx);

	resource_loader_trmdl.unref();
	resource_loader_trmsh.unref();
	resource_loader_trmbf.unref();
	resource_loader_trskl.unref();
	resource_loader_trmtr.unref();
	resource_loader_trmmt.unref();

	resource_loader_tracn.unref();
	resource_loader_tranm.unref();

	resource_loader_trpmcatalog.unref();

	resource_loader_bntx.unref();
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT gflib_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_gen_module);
	init_obj.register_terminator(uninitialize_gen_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
