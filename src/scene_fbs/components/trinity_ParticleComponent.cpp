#include "trinity_ParticleComponent.h"

using namespace godot;

void TrinityParticleComponent::_bind_methods()
{
    GETTER_SETTER_BIND(TrinityParticleComponent, particle_file, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityParticleComponent, unk_1, Variant::PACKED_INT64_ARRAY, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityParticleComponent, res_2, Variant::INT, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityParticleComponent, particle_name, Variant::STRING, PROPERTY_HINT_NONE)
    GETTER_SETTER_BIND(TrinityParticleComponent, particle_parent, Variant::STRING, PROPERTY_HINT_NONE)
}

void TrinityParticleComponent::LoadFromBuffer(const void* buffer)
{
    auto component = Titan::TrinityScene::GetTrinityParticleComponent(buffer);
    ERR_FAIL_COND_MSG(component == nullptr, "Couldn't parse TrinityParticleComponent");
    set_particle_file(Utils::toGodotString(component->particle_file()));
    PackedInt64Array unk_1_values;
    if (auto values = component->unk_1()) {
        for (auto value : *values) unk_1_values.push_back(value);
    }
    set_unk_1(unk_1_values);
    set_res_2(component->res_2());
    set_particle_name(Utils::toGodotString(component->particle_name()));
    set_particle_parent(Utils::toGodotString(component->particle_parent()));
}
