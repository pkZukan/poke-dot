// TRMTR/NVN addressing using one hardware-filtered sample. Fragment samples
// keep gradients from before address folding/clamping so mip selection remains
// stable at mirror seams and texture borders.
vec2 trinity_uv_axis(float uv, int mode, float half_texel) {
    if (mode == 1) return vec2(uv, 1.0); // Leave repeating axes to the sampler.
    if (mode == 6) uv = 1.0 - abs(mod(uv, 2.0) - 1.0);
    if (mode == 2 || mode == 3 || mode == 4) uv = abs(uv);
    if (mode == 0 || mode == 2) uv = clamp(uv, 0.0, 1.0);

    // Border modes blend in the border color over the edge's half texel.
    float coverage = 1.0;
    if (mode == 0 || mode == 2 || mode == 4 || mode == 5) {
        coverage = clamp(min(uv + half_texel, 1.0 + half_texel - uv)
                         / (2.0 * half_texel), 0.0, 1.0);
    }
    // Keep the filter footprint from reaching the opposite texture edge.
    return vec2(clamp(uv, half_texel, 1.0 - half_texel), coverage);
}

// slot is the texture's index among the shader's sampler2D uniforms in
// declaration order, skipping screen/depth textures; TrinityModel derives the
// same order from the shader's uniform list to fill the sampler_* arrays.
vec4 sample_trinity_lod(sampler2D tex, vec2 uv, int slot, float lod) {
    vec2 half_texel = 0.5 / vec2(textureSize(tex, int(ceil(lod))));
    vec2 u = trinity_uv_axis(uv.x, sampler_repeat_u[slot], half_texel.x);
    vec2 v = trinity_uv_axis(uv.y, sampler_repeat_v[slot], half_texel.y);
    return mix(sampler_border_color[slot], textureLod(tex, vec2(u.x, v.x), lod), u.y * v.y);
}

float trinity_uv_direction(float uv, int mode) {
    if (mode == 6) return mod(uv, 2.0) < 1.0 ? 1.0 : -1.0;
    if (mode == 2 || mode == 3 || mode == 4) return uv < 0.0 ? -1.0 : 1.0;
    return 1.0;
}

vec4 sample_trinity(sampler2D tex, vec2 uv, int slot) {
    int mode_u = sampler_repeat_u[slot], mode_v = sampler_repeat_v[slot];
    if (mode_u == 1 && mode_v == 1) return texture(tex, uv);

    vec2 dx = dFdx(uv), dy = dFdy(uv);
    // Query the sampler's clamped LOD, including textures with only one level.
    float lod = max(textureQueryLod(tex, uv).x, 0.0);
    vec2 half_texel = 0.5 / vec2(textureSize(tex, int(ceil(lod))));
    // Leave space for the anisotropic footprint on manually clamped axes,
    // whose hardware sampler must still repeat for the other axis.
    half_texel = min(vec2(0.5), max(half_texel, 0.5 * (abs(dx) + abs(dy))));
    vec2 u = trinity_uv_axis(uv.x, mode_u, half_texel.x);
    vec2 v = trinity_uv_axis(uv.y, mode_v, half_texel.y);
    vec2 direction = vec2(trinity_uv_direction(uv.x, mode_u), trinity_uv_direction(uv.y, mode_v));
    return mix(sampler_border_color[slot], textureGrad(tex, vec2(u.x, v.x), dx * direction, dy * direction), u.y * v.y);
}
