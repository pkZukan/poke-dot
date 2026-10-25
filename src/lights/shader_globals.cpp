#include "shader_globals.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector4.hpp>

#include <vector>

using namespace godot;

namespace {
std::vector<StringName> owned_globals;
}

void register_trinity_shader_globals()
{
    RenderingServer *rs = RenderingServer::get_singleton();
    struct Global {
        const char *name;
        RenderingServer::GlobalShaderParameterType type;
        Variant default_value;
    };
    // Keep these available before any Trinity shaders are loaded, in the editor
    // as well as at runtime. Textures start unbound until the environment sets them.
    const Global globals[] = {
        {"TrinityLightMapGain", RenderingServer::GLOBAL_VAR_TYPE_VEC3, Vector3(1, 1, 1)},
        {"TrinityDiffuseProbe", RenderingServer::GLOBAL_VAR_TYPE_SAMPLERCUBE, Variant()},
        {"TrinityDiffuseProbeGain", RenderingServer::GLOBAL_VAR_TYPE_VEC3, Vector3(1, 1, 1)},
        {"TrinityDiffuseProbeEnabled", RenderingServer::GLOBAL_VAR_TYPE_BOOL, false},
        {"SkySunDirection", RenderingServer::GLOBAL_VAR_TYPE_VEC3, Vector3(0, 0.5, -0.866)},
        {"SkyMoonDirection", RenderingServer::GLOBAL_VAR_TYPE_VEC3, Vector3(0, 0, 1)},
        {"SkyZenithColor", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 1)},
        {"SkyHorizonColor", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 1)},
        {"SkyCloudColor", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 1)},
        {"SkyCloudColorRim", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 1)},
        {"SkySunTintColor", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 1)},
        {"SkyFogColor", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 1)},
        {"ScatteringFogColor", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(1, 1, 1, 0)},
        {"ScatteringFogFactor", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 0.7},
        {"SkyHorizonFalloff", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyCloudRim", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyCloudsFadeOut", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, -0.3},
        {"SkyTopCloudSpeed", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyTopBreakupSpeed", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyTopPlaneHeight", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyTopPlaneHorizon", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyTopPlaneTiling", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyTopCloudCoverage", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkySunRadius", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkySunShine", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkySunBrightness", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyMoonScale", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyMoonPhase", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyMoonShadingRotation", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyMoonBrightness", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyStarsTiling", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyStarsHorizonLift", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyStarsIntensity", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 0.0},
        {"SkyStarsMaskTiling", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyStarsMaskSpeed", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 0.0},
        {"SkyFogIntensity", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyFogOffset", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"SkyFogGradation", RenderingServer::GLOBAL_VAR_TYPE_FLOAT, 1.0},
        {"AtmosphereLUT", RenderingServer::GLOBAL_VAR_TYPE_SAMPLER2D, Variant()},
        {"TrinityWorldShadowMap", RenderingServer::GLOBAL_VAR_TYPE_SAMPLER2D, Variant()},
        {"TrinityWorldShadowWeights", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(0, 0, 0, 0)},
        {"TrinityWorldShadowTransform", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(0, 0, 0, 0)},
        {"TrinityWorldShadowParams", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(40, 10, 0, 0)},
        {"TrinitySunShadowDirection", RenderingServer::GLOBAL_VAR_TYPE_VEC4, Vector4(0, 0, 0, 0)},
        {"TrinitySunShadingDirection", RenderingServer::GLOBAL_VAR_TYPE_VEC3, Vector3(0, 1, 0)},
    };

    for (const Global &global : globals) {
        const StringName name(global.name);
        // Type queries are editor-only. On normal startup these defaults are
        // registered before Godot loads any project shader global overrides.
        if (Engine::get_singleton()->is_editor_hint() &&
                rs->global_shader_parameter_get_type(name) != RenderingServer::GLOBAL_VAR_TYPE_MAX)
            continue;
        rs->global_shader_parameter_add(name, global.type, global.default_value);
        owned_globals.push_back(name);
    }
}

void unregister_trinity_shader_globals()
{
    RenderingServer *rs = RenderingServer::get_singleton();
    for (const StringName &name : owned_globals)
        rs->global_shader_parameter_remove(name);
    owned_globals.clear();
}
