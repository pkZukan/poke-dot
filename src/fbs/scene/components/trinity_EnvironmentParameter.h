#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_EnvironmentParameter_generated.h"
#include "utils.h"

namespace godot {

class TrinityEnvironmentParameter : public Resource {
    GDCLASS(TrinityEnvironmentParameter, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
