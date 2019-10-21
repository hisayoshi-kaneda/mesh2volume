#version 330
#define EPS 1e-5
precision highp float;

in vec3 f_fragColor;
in vec4 f_coordinate;
in vec3 f_normalCameraSpace;

out vec4 out_color;

uniform sampler2DArray texImage;
uniform bool is_front;
uniform int layer;

void main() {
	float depth = texelFetch(texImage, ivec3(gl_FragCoord.xy, layer),0).r;
	if(gl_FragCoord.z <= depth) discard;
	if(f_normalCameraSpace.z > 0 && is_front)out_color = vec4(0.5);
	else if(f_normalCameraSpace.z < 0 && !is_front)out_color = vec4(1.0);
	else discard;
}