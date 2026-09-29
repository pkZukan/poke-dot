#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/pe_InputEventTriggerComponent_generated.h"
#include "utils.h"

namespace godot {

class PeInputEventTriggerComponent : public Resource {
    GDCLASS(PeInputEventTriggerComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, InputName)
    GETTER_SETTER_DEFINE(String, ResourceName)
private:
    String InputName;
    String ResourceName;
};

} // namespace godot
