#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_ScriptComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityScriptComponent : public Resource {
    GDCLASS(TrinityScriptComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, FilePath)
    GETTER_SETTER_DEFINE(String, PackageName)
    GETTER_SETTER_DEFINE(bool, IsParallelized)
    GETTER_SETTER_DEFINE(float, Priority)
    GETTER_SETTER_DEFINE(bool, IsStatic)
    GETTER_SETTER_DEFINE(String, ResName)
private:
    String FilePath;
    String PackageName;
    bool IsParallelized = false;
    float Priority = 0.0f;
    bool IsStatic = false;
    String ResName;
};

} // namespace godot
