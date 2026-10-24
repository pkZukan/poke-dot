shader_type sky;

#include "trinity_material.gdshaderinc"

// Specular IBL source for the Environment. The visible sky is the SkySimple dome;
// this only feeds Godot's radiance map. TrinityEnvironment binds the Probe light's
// SpecularProbe cube and its tint parameters.
uniform samplerCube SpecularProbe : filter_linear;
uniform vec3 SpecularProbeColor = vec3(1.0);
uniform float SpecularProbeIntensity = 1.0;
uniform float SpecularProbeSaturation = 1.0;

void sky() {
    vec3 color = material_saturation(texture(SpecularProbe, EYEDIR).rgb, SpecularProbeSaturation);
    COLOR = color * SpecularProbeColor * SpecularProbeIntensity;
}
