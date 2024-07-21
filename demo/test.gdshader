shader_type spatial;

render_mode cull_front;

uniform sampler2D BaseColorMap : source_color;
uniform sampler2D LayerMaskMap;
uniform bool flipped_uvs;

void vertex() {
	//
}

void fragment() {
	vec2 uvs = UV;
	if(flipped_uvs)
		uvs = vec2(1.0 - UV.x, 1.0 - UV.y);
	ALBEDO = texture(BaseColorMap, uvs).rgb;
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
