#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/pe_UikitManagerComponent_generated.h"
#include "utils.h"

namespace godot {

class PeUikitManagerComponent : public Resource {
    GDCLASS(PeUikitManagerComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
