#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_GrassCollisionComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityGrassCollisionComponent : public Resource {
    GDCLASS(TrinityGrassCollisionComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, name)
    GETTER_SETTER_DEFINE(float, min)
    GETTER_SETTER_DEFINE(float, max)
private:
    String name;
    float min = 0.0f;
    float max = 0.0f;
};

} // namespace godot
