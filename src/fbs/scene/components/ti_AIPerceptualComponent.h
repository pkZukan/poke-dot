#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/ti_AIPerceptualComponent_generated.h"
#include "utils.h"

namespace godot {

class TiAIPerceptualComponent : public Resource {
    GDCLASS(TiAIPerceptualComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(bool, value)
private:
    bool value = false;
};

} // namespace godot
