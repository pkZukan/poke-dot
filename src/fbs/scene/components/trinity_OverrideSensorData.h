#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_OverrideSensorData_generated.h"
#include "utils.h"

namespace godot {

class TrinityOverrideSensorData : public Resource {
    GDCLASS(TrinityOverrideSensorData, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(float, RealizingDistance)
    GETTER_SETTER_DEFINE(float, UnrealizingDistance)
    GETTER_SETTER_DEFINE(float, LoadingDistance)
    GETTER_SETTER_DEFINE(float, UnloadingDistance)
private:
    float RealizingDistance = 0.0f;
    float UnrealizingDistance = 0.0f;
    float LoadingDistance = 0.0f;
    float UnloadingDistance = 0.0f;
};

} // namespace godot
