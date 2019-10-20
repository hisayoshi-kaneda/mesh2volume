#version 330

#define EPS 1.0e-5

in vec4 f_coordinate;

out vec4 out_color;

uniform sampler2DArray LayeredDepthImages;
uniform int layerN;

void main(void) {
	vec3 temp = (f_coordinate.xyz / f_coordinate.w + 1.0) / 2.0;
	for(int i=0;i<layerN;i+=2){
		float depthf = texture(LayeredDepthImages ,vec3(temp.xy, i)).r;
		float depthb = texture(LayeredDepthImages ,vec3(temp.xy,i-1)).r;
		if(depthf <= gl_FragCoord.z && gl_FragCoord.z <= depthb){
			if(1.0 - EPS < depthb) discard;
			out_color = vec4(1.0);
		}
	}
}