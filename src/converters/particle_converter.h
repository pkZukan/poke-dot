#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/cpu_particles3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include "middleware/ptcl.h"
#include <vector>

namespace godot
{

class TrinityParticles : public Node3D
{
    GDCLASS(TrinityParticles, Node3D)

    struct EmitterPlayback
    {
        CPUParticles3D* particles = nullptr;
        Ref<StandardMaterial3D> material;
        Vector2 uv_offset;
        Vector2 uv_scroll;
        double start = 0.0;
        double end = 0.0;
        double lifetime = 0.0;
        bool started = false;
        bool stopped = false;
        bool burst = false;
    };

    Ref<ParticleArchive> archive;
    std::vector<EmitterPlayback> emitters;
    PackedStringArray warnings;
    double elapsed = 0.0;
    double finish_time = 0.0;
    bool playing = false;
    bool draining = false;

    void clear_emitters();
    void convert_emitter(const Dictionary& emitter, double unit_scale, double frame_rate);
    void warn(const Dictionary& emitter, const String& message);

protected:
    static void _bind_methods();

public:
    Error load_effect(const String& path, const String& emitter_set = String(),
                      double unit_scale = 1.0, double frame_rate = 60.0);
    Error convert(const Ref<ParticleArchive>& source, const String& emitter_set = String(),
                  double unit_scale = 1.0, double frame_rate = 60.0);
    void play();
    void stop(bool clear = false);
    void _process(double delta) override;

    bool is_playing() const
    {
        return playing;
    }

    PackedStringArray get_warnings() const
    {
        return warnings;
    }
};

}
