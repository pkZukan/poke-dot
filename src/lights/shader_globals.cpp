#include "shader_globals.h"

#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector4.hpp>

#include <vector>

using namespace godot;

namespace
{
std::vector<StringName> owned_globals;
}

void register_trinity_shader_globals()
{
    ProjectSettings* settings = ProjectSettings::get_singleton();

    struct Global
    {
        const char* name;
        const char* type;
        Variant default_value;
    };

    // RenderingServer is not exposed to extensions at scene initialization yet.
    // Godot loads these settings into the renderer later during startup.
    // Textures start unbound until the environment sets them.
    const Global globals[] = {
        {"TrinityLightMapGain", "vec3", Vector3(1, 1, 1)},
        {"TrinityDiffuseProbe", "samplerCube", String()},
        {"TrinityDiffuseProbeGain", "vec3", Vector3(1, 1, 1)},
        {"TrinityDiffuseProbeEnabled", "bool", false},
        {"SkySunDirection", "vec3", Vector3(0, 0.5, -0.866)},
        {"SkyMoonDirection", "vec3", Vector3(0, 0, 1)},
        {"SkyZenithColor", "vec4", Vector4(1, 1, 1, 1)},
        {"SkyHorizonColor", "vec4", Vector4(1, 1, 1, 1)},
        {"SkyCloudColor", "vec4", Vector4(1, 1, 1, 1)},
        {"SkyCloudColorRim", "vec4", Vector4(1, 1, 1, 1)},
        {"SkySunTintColor", "vec4", Vector4(1, 1, 1, 1)},
        {"SkyFogColor", "vec4", Vector4(1, 1, 1, 1)},
        {"ScatteringFogColor", "vec4", Vector4(1, 1, 1, 0)},
        {"ScatteringFogFactor", "float", 0.7},
        {"SkyHorizonFalloff", "float", 1.0},
        {"SkyCloudRim", "float", 1.0},
        {"SkyCloudsFadeOut", "float", -0.3},
        {"SkyTopCloudSpeed", "float", 1.0},
        {"SkyTopBreakupSpeed", "float", 1.0},
        {"SkyTopPlaneHeight", "float", 1.0},
        {"SkyTopPlaneHorizon", "float", 1.0},
        {"SkyTopPlaneTiling", "float", 1.0},
        {"SkyTopCloudCoverage", "float", 1.0},
        {"SkySunRadius", "float", 1.0},
        {"SkySunShine", "float", 1.0},
        {"SkySunBrightness", "float", 1.0},
        {"SkyMoonScale", "float", 1.0},
        {"SkyMoonPhase", "float", 1.0},
        {"SkyMoonShadingRotation", "float", 1.0},
        {"SkyMoonBrightness", "float", 1.0},
        {"SkyStarsTiling", "float", 1.0},
        {"SkyStarsHorizonLift", "float", 1.0},
        {"SkyStarsIntensity", "float", 0.0},
        {"SkyStarsMaskTiling", "float", 1.0},
        {"SkyStarsMaskSpeed", "float", 0.0},
        {"SkyFogIntensity", "float", 1.0},
        {"SkyFogOffset", "float", 1.0},
        {"SkyFogGradation", "float", 1.0},
        {"AtmosphereLUT", "sampler2D", String()},
        {"TrinityWorldShadowMap", "sampler2D", String()},
        {"TrinityWorldShadowWeights", "vec4", Vector4(0, 0, 0, 0)},
        {"TrinityWorldShadowTransform", "vec4", Vector4(0, 0, 0, 0)},
        {"TrinityWorldShadowParams", "vec4", Vector4(40, 10, 0, 0)},
        {"TrinitySunShadowDirection", "vec4", Vector4(0, 0, 0, 0)},
        {"TrinitySunShadingDirection", "vec3", Vector3(0, 1, 0)},
    };

    for (const Global& global : globals)
    {
        const StringName name(String("shader_globals/") + global.name);
        if (settings->has_setting(name))
        {
            continue;
        }
        Dictionary value;
        value["type"] = global.type;
        value["value"] = global.default_value;
        settings->set_setting(name, value);
        settings->set_initial_value(name, value);
        owned_globals.push_back(name);
    }
}

void unregister_trinity_shader_globals()
{
    ProjectSettings* settings = ProjectSettings::get_singleton();
    for (const StringName& name : owned_globals)
    {
        settings->set_setting(name, Variant());
    }
    owned_globals.clear();
}
