#version 330
#define EPS 1e-3
precision highp float;

in vec3 f_fragColor;
in vec4 f_coordinate;
in vec3 f_normalCameraSpace;

out vec4 out_color;

uniform sampler2D texImage;
uniform bool is_front;

void main() {
	vec2 temp = (f_coordinate.xy / f_coordinate.w + 1.0) / 2.0;
	float depth = texture(texImage, temp.xy).r;
	if(gl_FragCoord.z <= depth) discard;
	if(f_normalCameraSpace.z > 0 && is_front)out_color = vec4(0.5);
	else if(f_normalCameraSpace.z < 0 && !is_front)out_color = vec4(1.0);
	else if(-EPS < f_normalCameraSpace.z && f_normalCameraSpace.z < EPS)out_color = vec4(0.8);
	else discard;
}