#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include "generated/subscene_generated.h"
#include <utils.h>

namespace godot {

class TRSubScene : public Resource {
	GDCLASS(TRSubScene, Resource)
protected:
	static void _bind_methods();
public:
	TRSubScene(){}
	~TRSubScene(){}

    GETTER_SETTER_DEFINE(String, FileName)
	GETTER_SETTER_DEFINE(int, unk1)

	void LoadFromBuffer(const void* data);

private:
    String FileName;
	int unk1;
};
}   