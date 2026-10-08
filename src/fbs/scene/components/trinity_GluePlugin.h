#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_GluePlugin_generated.h"
#include "utils.h"

namespace godot {

class TrinityGluePlugin : public Resource {
    GDCLASS(TrinityGluePlugin, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
