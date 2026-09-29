#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/pe_UikitViewComponent_generated.h"
#include "utils.h"

namespace godot {

class PeUikitViewComponent : public Resource {
    GDCLASS(PeUikitViewComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, FilePath)
    GETTER_SETTER_DEFINE(String, BluaPath)
private:
    String FilePath;
    String BluaPath;
};

} // namespace godot
