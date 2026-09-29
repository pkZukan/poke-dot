// TRMTR/NVN addressing using one hardware-filtered sample.
// The half-texel inset is for the BNTX importer's current mip-0-only textures;
// mipmaps/anisotropic filtering would require native sampler support instead.
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

vec4 sample_trinity(sampler2D tex, vec2 uv, int slot) {
    vec2 half_texel = 0.5 / vec2(textureSize(tex, 0));
    vec2 u = trinity_uv_axis(uv.x, sampler_repeat_u[slot], half_texel.x);
    vec2 v = trinity_uv_axis(uv.y, sampler_repeat_v[slot], half_texel.y);
    return mix(sampler_border_color[slot], textureLod(tex, vec2(u.x, v.x), 0.0), u.y * v.y);
}
