#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_CharacterCreationMasterComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityCharacterCreationMasterComponent : public Resource {
    GDCLASS(TrinityCharacterCreationMasterComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
};

} // namespace godot
