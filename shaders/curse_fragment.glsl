#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform float curseStrength;

out vec4 finalColor;

void main()
{
	vec4 texel = texture(texture0, fragTexCoord) * fragColor;
	vec3 curseTint = vec3(0.35, 0.85, 0.55);
	float s = clamp(curseStrength, 0.0, 1.0);
	vec3 mixed = mix(texel.rgb, texel.rgb * curseTint + vec3(0.08, 0.12, 0.1), s * 0.55);
	finalColor = vec4(mixed, texel.a);
}
