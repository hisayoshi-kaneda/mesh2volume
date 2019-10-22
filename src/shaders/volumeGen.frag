#version 330

#define EPS 1.0e-5

in vec3 f_posWorldSpace;

out vec4 out_color;

uniform sampler2DArray LayeredDepthImagesX;
uniform sampler2DArray LayeredDepthImagesY;
uniform sampler2DArray LayeredDepthImagesZ;
uniform int layerN;
uniform int indexZ;
uniform vec3 size;
uniform float resolution;
uniform float cameraPosX;
uniform float cameraPosY;
uniform float zNear;
uniform float zFar;

void main(void) {
	float depthNormalize = 1.0 / (zFar - zNear);
	float color = 0.0;
	for(int i=0;i<layerN;i+=2){
		color = 0.0;
		float depthFront = texelFetch(LayeredDepthImagesX ,ivec3(gl_FragCoord.y,indexZ,i), 0).r;
		float depthBack = texelFetch(LayeredDepthImagesX ,ivec3(gl_FragCoord.y,indexZ,i+1), 0).r;
		float fragDepth = depthNormalize * (cameraPosX - f_posWorldSpace.x - zNear);
		if(depthFront <= fragDepth && fragDepth <= depthBack){
			if(1.0 - EPS < depthBack) continue;
			color = 1.0;
			break;
		}
	}
	if(color < 0.5) discard;
	for(int i=0;i<layerN;i+=2){
		color = 0.0;
		float depthFront = texelFetch(LayeredDepthImagesY ,ivec3(indexZ,gl_FragCoord.x,i), 0).r;
		float depthBack = texelFetch(LayeredDepthImagesY ,ivec3(indexZ,gl_FragCoord.x,i+1), 0).r;
		float fragDepth = depthNormalize * (cameraPosY- f_posWorldSpace.y - zNear);
		if(depthFront <= fragDepth && fragDepth <= depthBack){
			if(1.0 - EPS < depthBack) continue;
			color = 1.0;
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
			color = 1.0;
			break;
		}
	}
	if(color < 0.5) discard;
	else out_color = vec4(1.0);
}