shader_type spatial;

uniform sampler2D NightStarsMap;
uniform sampler2D NightStarsMaskMap;
uniform sampler2D TopCloudsShapeMap;
uniform sampler2D TopCloudsBreakupMap;
uniform sampler2D MoonColorMap;
uniform sampler2D MoonNormalMap : hint_normal;

uniform vec4 ZenithColor = vec4(1.0f);
uniform vec4 HorizonColor = vec4(1.0f);
uniform vec4 CloudColor = vec4(1.0f);
uniform vec4 CloudColorRim = vec4(1.0f);
uniform vec4 SunTintColor = vec4(1.0f);
uniform vec4 FogColor = vec4(1.0f);

uniform float HorizonFalloff;
uniform float StarsIntensity;
uniform float StarsTiling;
uniform float StarsHorizonLift;
uniform float StarsMaskTiling;
uniform float StarsMaskSpeed;
uniform float CloudRim;
uniform float CloudsFadeOut;
uniform float TopCloudSpeed;
uniform float TopBreakupSpeed;
uniform float TopPlaneHeight;
uniform float TopPlaneHorizon;
uniform float TopPlaneTiling;
uniform float TopCloudCoverage;
uniform float SunRadius;
uniform float SunShine;
uniform float SunPitch;
uniform float SunYaw;
uniform float SunIntensity;
uniform float MoonScale;
uniform float MoonPhase;
uniform float MoonShadingRotation;
uniform float MoonPitch;
uniform float MoonYaw;
uniform float MoonIntensity;
uniform float FogIntensity;
uniform float FogOffset;
uniform float FogGradation;


void vertex() {
	//
}

void fragment() {
	vec2 uvs = vec2(UV.x, 1.0 - UV.y); //Flip
	
	float Moon_phase = MoonPhase * 2.0 * PI;
	
	vec3 moon_alb = texture(MoonColorMap, uvs).rgb;
	
	vec3 star_alb = texture(NightStarsMap, uvs).rgb;
	
	float horizon_falloff = smoothstep(0.0, 1.0, clamp(HorizonFalloff, 0.0, 1.0));
    vec4 sky_color = mix(HorizonColor, ZenithColor, horizon_falloff);
	
	ALBEDO = sky_color.rgb;
}
