#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/pe_SimpleNode_generated.h"
#include "utils.h"

namespace godot {

class PeSimpleNode : public Resource {
    GDCLASS(PeSimpleNode, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, Name)
private:
    String Name;
};

} // namespace godot
