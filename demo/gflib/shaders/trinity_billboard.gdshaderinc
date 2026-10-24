// Retail BillboardType=AxisXYZ: local XY is scaled by the model axis lengths and
// laid in the view plane, rolled so world up stays upright on screen; local Z
// moves along the view axis. Other billboard types keep the regular transform.
uniform int BillboardType;

mat4 trinity_billboard(mat4 modelview, mat4 model, mat4 view) {
    if (BillboardType != 1) return modelview;
    vec3 up = (view * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
    float roll = atan(up.x / up.y);
    float c = cos(roll), s = sin(roll);
    vec3 scale = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
    return mat4(vec4(c * scale.x, -s * scale.x, 0.0, 0.0), vec4(s * scale.y, c * scale.y, 0.0, 0.0),
                vec4(0.0, 0.0, scale.z, 0.0), modelview[3]);
}
