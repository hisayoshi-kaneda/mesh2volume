#version 330

#define EPS 1.0e-5

in vec4 f_coordinate;

out vec4 out_color;

uniform sampler2D depthFront;
uniform sampler2D depthBack;

void main(void) {
	vec3 temp = (f_coordinate.xyz / f_coordinate.w + 1.0) / 2.0;
	float depthf = texture(depthFront,temp.xy).r;
	float depthb = texture(depthBack,temp.xy).r;
	if(1.0 - EPS < depthb) discard;
	if(depthf <= gl_FragCoord.z && gl_FragCoord.z <= depthb){
		out_color = vec4(1.0);
	}
	else{
		discard;
	}
}