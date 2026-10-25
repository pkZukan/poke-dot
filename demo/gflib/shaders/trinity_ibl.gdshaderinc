// The diffuse cube is already convolved irradiance. Sample by world-space
// surface normal, independently of the sky used for specular reflections.
global uniform samplerCube TrinityDiffuseProbe : filter_linear;
global uniform vec3 TrinityDiffuseProbeGain;
global uniform bool TrinityDiffuseProbeEnabled;
// The material's single lightmap slice uses the master's base lightmap tint
// and intensity. Multi-slice blending remains separate work.
global uniform vec3 TrinityLightMapGain;

vec4 trinity_irradiance(vec3 normal_view, mat4 inverse_view) {
    if (!TrinityDiffuseProbeEnabled) return vec4(0.0);
    vec3 direction = normalize(mat3(inverse_view) * normal_view);
    return vec4(texture(TrinityDiffuseProbe, direction).rgb * TrinityDiffuseProbeGain, 1.0);
}
