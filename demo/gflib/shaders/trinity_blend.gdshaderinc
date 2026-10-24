// NVN takes the blend state from the material (TRMTR alpha_type) while Godot
// takes it from render_mode, so transparent shaders use blend_premul_alpha and
// rebuild each state: Blend = src * a + dst * (1 - a), Add = src * a + dst,
// BlendPreMultiAlpha = src + dst * (1 - a). The importer sets TrinityAlphaType:
// 0 Opaque, 1 Blend, 2 Add, 3 BlendPreMultiAlpha.
uniform int TrinityAlphaType = 1;

// Returns (PREMUL_ALPHA_FACTOR, ALPHA). `premultiplied` is the factor the
// retail shader itself already applied to its output color.
vec2 trinity_blend(float alpha, float premultiplied) {
    if (TrinityAlphaType == 0) return vec2(premultiplied, 1.0);
    if (TrinityAlphaType == 2) return vec2(premultiplied * alpha, 0.0);
    if (TrinityAlphaType == 3) return vec2(premultiplied, alpha);
    return vec2(premultiplied * alpha, alpha);
}
