#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/pe_TextComponent_generated.h"
#include "utils.h"

namespace godot {

class PeTextComponent : public Resource {
    GDCLASS(PeTextComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, FilePath)
private:
    String FilePath;
};

} // namespace godot
