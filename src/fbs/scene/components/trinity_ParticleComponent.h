#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "generated/trinity_ParticleComponent_generated.h"
#include "utils.h"

namespace godot {

class TrinityParticleComponent : public Resource {
    GDCLASS(TrinityParticleComponent, Resource)
protected:
    static void _bind_methods();
public:
    void LoadFromBuffer(const void* buffer);
    GETTER_SETTER_DEFINE(String, particle_file)
    GETTER_SETTER_DEFINE(PackedInt64Array, unk_1)
    GETTER_SETTER_DEFINE(uint32_t, res_2)
    GETTER_SETTER_DEFINE(String, particle_name)
    GETTER_SETTER_DEFINE(String, particle_parent)
private:
    String particle_file;
    PackedInt64Array unk_1;
    uint32_t res_2 = 0;
    String particle_name;
    String particle_parent;
};

} // namespace godot
