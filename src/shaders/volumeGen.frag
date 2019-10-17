#version 330

#define EPS 1.0e-5

in vec4 f_coordinate;

out vec4 out_color;

uniform sampler2D depthFront;
uniform sampler2D depthBack;

void main(void) {
	vec3 temp = (f_coordinate.xyz / f_coordinate.w + 1.0) / 2.0;
	if(texture(depthFront,temp.xy).r < gl_FragCoord.z && gl_FragCoord.z < texture(depthBack,temp.xy).r){
		out_color = vec4(1.0);
	}
	else{
		discard;
	}
}