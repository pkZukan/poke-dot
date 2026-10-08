#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_LightDirectApplierComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityLightDirectApplierComponent : public Resource {
    GDCLASS(TrinityLightDirectApplierComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
