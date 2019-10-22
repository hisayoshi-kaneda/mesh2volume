#version 330

#define EPS 1.0e-5

in vec4 f_coordinate;

out vec4 out_color;

uniform sampler2DArray LayeredDepthImagesX;
uniform sampler2DArray LayeredDepthImagesY;
uniform sampler2DArray LayeredDepthImagesZ;
uniform int layerN;
uniform int indexZ;
uniform vec3 size;
uniform float resolution;
uniform float depthNormalize;

void main(void) {
	float color = 0.0;
	for(int i=0;i<layerN;i+=2){
		color = 0.0;
		float depthFront = texelFetch(LayeredDepthImagesX ,ivec3(gl_FragCoord.y,indexZ,i), 0).r;
		float depthBack = texelFetch(LayeredDepthImagesX ,ivec3(gl_FragCoord.y,indexZ,i+1), 0).r;
		float fragDepth = depthNormalize * ((size.x * resolution / 2.0) - f_coordinate.x);
		if(depthFront <= fragDepth && fragDepth <= depthBack){
			if(1.0 - EPS < depthBack) continue;
			out_color = vec4(1.0);
			break;
		}
	}
	if(color < 0.5) discard;
	for(int i=0;i<layerN;i+=2){
		color = 0.0;
		float depthFront = texelFetch(LayeredDepthImagesY ,ivec3(indexZ,gl_FragCoord.x,i), 0).r;
		float depthBack = texelFetch(LayeredDepthImagesY ,ivec3(indexZ,gl_FragCoord.x,i+1), 0).r;
		float fragDepth = depthNormalize * ((size.y * resolution / 2.0) - f_coordinate.y);
		if(depthFront <= fragDepth && fragDepth <= depthBack){
			if(1.0 - EPS < depthBack) continue;
			out_color = vec4(1.0);
			break;
		}
	}
	if(color < 0.5) discard;
	for(int i=0;i<layerN;i+=2){
		color = 0.0;
		float depthFront = texelFetch(LayeredDepthImagesZ ,ivec3(gl_FragCoord.xy,i), 0).r;
		float depthBack = texelFetch(LayeredDepthImagesZ ,ivec3(gl_FragCoord.xy,i+1), 0).r;
		if(depthFront <= gl_FragCoord.z && gl_FragCoord.z <= depthBack){
			if(1.0 - EPS < depthBack) continue;
			out_color = vec4(1.0);
			break;
		}
	}
}