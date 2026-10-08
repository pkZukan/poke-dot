#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_TextureBufferComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityTextureBufferComponent : public Resource {
    GDCLASS(TrinityTextureBufferComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
