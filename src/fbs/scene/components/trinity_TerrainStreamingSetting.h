#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_TerrainStreamingSetting_generated.h"
#include "utils.h"

namespace godot {

class TrinityTerrainStreamingSetting : public Resource {
    GDCLASS(TrinityTerrainStreamingSetting, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(float, LowLoadRadius)
    GETTER_SETTER_DEFINE(float, MediumLoadRadius)
    GETTER_SETTER_DEFINE(float, HighLoadRadius)
    GETTER_SETTER_DEFINE(float, CollisionLoadRadius)
    GETTER_SETTER_DEFINE(float, TreeLoadRadius)
private:
    float LowLoadRadius = 0.0f;
    float MediumLoadRadius = 0.0f;
    float HighLoadRadius = 0.0f;
    float CollisionLoadRadius = 0.0f;
    float TreeLoadRadius = 0.0f;
};

} // namespace godot
