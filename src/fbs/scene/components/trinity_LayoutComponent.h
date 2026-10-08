#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_LayoutComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityLayoutComponent : public Resource {
    GDCLASS(TrinityLayoutComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, FilePath)
    GETTER_SETTER_DEFINE(String, LayoutName)
private:
    String FilePath;
    String LayoutName;
};

} // namespace godot
