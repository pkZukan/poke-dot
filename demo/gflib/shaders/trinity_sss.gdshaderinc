// Spherical-Gaussian diffusion and GGX equations checked against retail SSS.
// Godot supplies the lights, shadows and environment; see docs/shaders/retail.md.
vec3 trinity_diffusion_profile(float nl, vec3 subsurface) {
    vec3 sharpness = 1.0 / max(vec3(0.0001), subsurface);
    vec3 e = exp(-sharpness);
    vec3 e2 = e * e;
    vec3 inverse_sharpness = 1.0 / sharpness;
    vec3 scale = vec3(1.0) + 2.0 * e2 - inverse_sharpness;
    vec3 bias = (e - e2) * inverse_sharpness - e2;
    vec3 width = sqrt(max(vec3(1.0) - scale, vec3(0.00000001)));
    vec3 x0 = vec3(0.36 * nl);
    vec3 x1 = width / (4.0 * 0.36);
    vec3 n = x0 + x1;
    vec3 y = mix(vec3(clamp(nl, 0.0, 1.0)), n * n / width, step(abs(x0), x1));
    vec3 diffuse = scale * y + bias;
    vec3 x = max(vec3(0.0), diffuse - vec3(0.004));
    return (x * (6.2 * x + vec3(0.5))) / (x * (6.2 * x + vec3(1.7)) + vec3(0.06));
}
vec3 trinity_sss_ggx(float nl, float nv, float nh, float lh, float roughness, float f0, float f90) {
    float alpha = max(roughness * roughness, 0.001);
    float alpha2 = alpha * alpha;
    float lambda_v = nl * sqrt((-nv * alpha2 + nv) * nv + alpha2);
    float lambda_l = nv * sqrt((-nl * alpha2 + nl) * nl + alpha2);
    float visibility = 0.5 / max(lambda_v + lambda_l, 0.00000001);
    float denominator = (nh * alpha2 - nh) * nh + 1.0;
    float distribution = alpha2 / max(denominator * denominator, 0.000000000001);
    float fresnel = f0 + (f90 - f0) * pow(1.0 - lh, 5.0);
    return vec3(distribution * visibility * fresnel * nl / PI);
}
vec3 trinity_transmission(vec3 volume_albedo, float thickness) {
    float e = exp(-thickness / 3.0);
    return volume_albedo * e * (e * e + 3.0);
}
