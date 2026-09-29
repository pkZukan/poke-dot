#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_GroundPlaceComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityGroundPlaceComponent : public Resource {
    GDCLASS(TrinityGroundPlaceComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(uint32_t, index)
private:
    uint32_t index = 0;
};

} // namespace godot
