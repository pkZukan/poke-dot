#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_LayoutCommonResourceComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityLayoutCommonResourceComponent : public Resource {
    GDCLASS(TrinityLayoutCommonResourceComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
