#version 330

#define EPS 1.0e-5

in vec4 f_coordinate;

out vec4 out_color;

uniform sampler2DArray LayeredDepthImages;
uniform int layerN;

void main(void) {
	for(int i=0;i<layerN;i+=2){
		float depthFront = texelFetch(LayeredDepthImages ,ivec3(gl_FragCoord.xy,i), 0).r;
		float depthBack = texelFetch(LayeredDepthImages ,ivec3(gl_FragCoord.xy,i+1), 0).r;
		if(depthFront <= gl_FragCoord.z && gl_FragCoord.z <= depthBack){
			if(1.0 - EPS < depthBack) discard;
			out_color = vec4(1.0);
		}
	}
}