#pragma once

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include "generated/trmbf_generated.h"
#include <utils.h>

namespace godot {

class PokemonEntity : public Node {
    GDCLASS(PokemonEntity, Node)
protected:
	static void _bind_methods();
public:
	PokemonEntity(){}
	~PokemonEntity(){}

};

}