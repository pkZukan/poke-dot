#include "register_types.h"

#include "model_fbs/trmdl.h"
#include "model_fbs/trmsh.h"
#include "model_fbs/trmbf.h"
#include "model_fbs/trskl.h"
#include "model_fbs/trmtr.h"
#include "model_fbs/trmmt.h"

#include "animation_fbs/anim_common.h"
#include "animation_fbs/tracn.h"
#include "animation_fbs/tranm.h"
#include "animation_fbs/tracp.h"
#include "animation_fbs/tracr.h"
#include "animation_fbs/tracm.h"

#include "catalog_fbs/trpmcatalog.h"
#include "actors/pokemon_catalog.h"

#include "actors/actor.h"
#include "actors/pokemon_actor.h"
#include "actors/trainer_actor.h"
#include "characters/pokemon_character.h"
#include "converters/animation_converter.h"
#include "converters/model_converter.h"

#include "middleware/bntx.h"

#include "utils.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>

using namespace godot;

DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRMDL)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRMSH)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRMBF)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRSKL)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRMTR)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRMMT)

DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRACN)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRANM)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRACP)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRACR)
DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRACM)

DEFINE_RESOURCE_LOADER(ResourceFormatLoaderTRPMCATALOG)

DEFINE_RESOURCE_LOADER(ResourceFormatLoaderBNTX)

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

		//anim_common
		GDREGISTER_CLASS(TRAnimationInfo)
		GDREGISTER_CLASS(FixedVectorTrack)
		GDREGISTER_CLASS(DynamicVectorTrack)
		GDREGISTER_CLASS(Framed8VectorTrack)
		GDREGISTER_CLASS(Framed16VectorTrack)
		GDREGISTER_CLASS(FixedRotationTrack)
		GDREGISTER_CLASS(DynamicRotationTrack)
		GDREGISTER_CLASS(Framed8RotationTrack)
		GDREGISTER_CLASS(Framed16RotationTrack)
		GDREGISTER_CLASS(FixedBoolTrack)
		GDREGISTER_CLASS(DynamicBoolTrack)
		GDREGISTER_CLASS(Framed8BoolTrack)
		GDREGISTER_CLASS(Framed16BoolTrack)
		GDREGISTER_CLASS(FixedFloatTrack)
		GDREGISTER_CLASS(DynamicFloatTrack)
		GDREGISTER_CLASS(Framed8FloatTrack)
		GDREGISTER_CLASS(Framed16FloatTrack)

		//tranm
		GDREGISTER_CLASS(BoneInit)
		GDREGISTER_CLASS(BoneTrack)
		GDREGISTER_CLASS(BoneAnimation)
		GDREGISTER_CLASS(TRAnimation)

		//tracn
		GDREGISTER_CLASS(TRAnimationChannelNames)
		GDREGISTER_CLASS(AnimationChannelNameEntry)

		//tracm
		GDREGISTER_CLASS(TRAnimationChannelMeshes)
		GDREGISTER_CLASS(TRMeshAnimeTrack)
		GDREGISTER_CLASS(TRTrackFlagsInfo)
		GDREGISTER_CLASS(TRBlendShapeTimeline)
		GDREGISTER_CLASS(TRVisibilityShapeTimeline)
		GDREGISTER_CLASS(TRTrackMaterialTimeline)
		GDREGISTER_CLASS(TRTrackFlagsInfo)
		GDREGISTER_CLASS(TRTrackBlendShape)
		GDREGISTER_CLASS(TRBlendTable)
		GDREGISTER_CLASS(TRTrackMaterial)
		GDREGISTER_CLASS(TRTrackMaterialAnim)
		GDREGISTER_CLASS(TRTrackMaterialInit)
		GDREGISTER_CLASS(TRTrackMaterialChannel)
		GDREGISTER_CLASS(TRTrackMaterialChannelVec4)
		GDREGISTER_CLASS(TRTrackMaterialValue)
		GDREGISTER_CLASS(TRTrackBlendshapeInit)

		//tracr
		GDREGISTER_CLASS(TRAnimationChannelResource)
		GDREGISTER_CLASS(TRAnimationTrackListTable)
		GDREGISTER_CLASS(TRAnimationTrack)
		GDREGISTER_CLASS(TRAnimationTrackResourceTable)
		GDREGISTER_CLASS(TRAnimationTrackResource)

		//tracp
		GDREGISTER_CLASS(TRAnimationChannelParams)
		GDREGISTER_CLASS(AnimationChannelParamEntry)
		GDREGISTER_CLASS(AnimationChannelParamInt)
		GDREGISTER_CLASS(AnimationChannelParamFloat)
		GDREGISTER_CLASS(AnimationChannelParamBool)
		GDREGISTER_CLASS(AnimationChannelParamTrigger)

		//catalog
		GDREGISTER_CLASS(TRPpokemonCatalog)
		GDREGISTER_CLASS(CatalogEntry)
		GDREGISTER_CLASS(SpeciesResourceInfo)
		GDREGISTER_CLASS(AnimationResourceInfo)
		GDREGISTER_CLASS(LocatorResourceInfo)

		//Entities
		GDREGISTER_CLASS(ActorObj)
		GDREGISTER_CLASS(PokemonActor)
		GDREGISTER_CLASS(TrainerActor)
		GDREGISTER_CLASS(PokemonCharacter)
		GDREGISTER_CLASS(TrinityAnimationConverter)
		GDREGISTER_CLASS(TrinityModel)

		//middleware
		GDREGISTER_CLASS(BinaryTexture)

		GDREGISTER_CLASS(Utils)

		//Resource loaders
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRMDL)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRMSH)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRMBF)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRSKL)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRMTR)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRMMT)

		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRACN)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRANM)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRACP)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRACR)
		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRACM)

		INIT_RESOURCE_LOADER(ResourceFormatLoaderTRPMCATALOG)

		INIT_RESOURCE_LOADER(ResourceFormatLoaderBNTX)

		//Singletons
		GDREGISTER_CLASS(PokemonCatalog)
		Engine::get_singleton()->register_singleton("PokemonCatalog", memnew(PokemonCatalog));

		//Init singletons
		PokemonCatalog::get_singleton()->load_catalog();
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

	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRMDL)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRMSH)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRMBF)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRSKL)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRMTR)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRMMT)

	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRACN)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRANM)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRACP)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRACR)
	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRACM)

	FINI_RESOURCE_LOADER(ResourceFormatLoaderTRPMCATALOG)

	FINI_RESOURCE_LOADER(ResourceFormatLoaderBNTX)

	if (Engine::get_singleton()->has_singleton("PokemonCatalog")) {
        PokemonCatalog* catalog = Object::cast_to<PokemonCatalog>(
            Engine::get_singleton()->get_singleton("PokemonCatalog")
        );
        Engine::get_singleton()->unregister_singleton("PokemonCatalog");
        memdelete(catalog);
    }
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
