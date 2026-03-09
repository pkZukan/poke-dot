shader_type spatial;

uniform sampler2D NightStarsMap;
uniform sampler2D NightStarsMaskMap;
uniform sampler2D TopCloudsShapeMap;
uniform sampler2D TopCloudsBreakupMap;
uniform sampler2D MoonColorMap;
uniform sampler2D MoonNormalMap;

uniform vec4 ZenithColor;
uniform vec4 HorizonColor;
uniform vec4 CloudColor;
uniform vec4 CloudColorRim;
uniform vec4 SunTintColor;
uniform vec4 FogColor;

void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	ALBEDO = texture(NightStarsMap, uvs).rgb;
}
