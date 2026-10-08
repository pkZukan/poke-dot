#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_LightApplierComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityLightApplierComponent : public Resource {
    GDCLASS(TrinityLightApplierComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
