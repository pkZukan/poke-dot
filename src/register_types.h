#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

#define DEFINE_RESOURCE_LOADER(x) \
	Ref<x> m_##x;

#define INIT_RESOURCE_LOADER(x) \
    GDREGISTER_ABSTRACT_CLASS(x) \
	m_##x.instantiate(); \
    ResourceLoader::get_singleton()->add_resource_format_loader(m_##x);

#define FINI_RESOURCE_LOADER(x) \
	ResourceLoader::get_singleton()->remove_resource_format_loader(m_##x); \
	m_##x.unref();


void initialize_gen_module(ModuleInitializationLevel p_level);
void uninitialize_gen_module(ModuleInitializationLevel p_level);

