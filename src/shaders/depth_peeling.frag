#version 330
#define EPS1 1e-6
#define EPS2 1e-6
precision highp float;

in vec3 f_fragColor;
in vec4 f_coordinate;
in vec3 f_normalCameraSpace;

out vec4 out_color;

uniform sampler2D texImage;

void main() {
	vec2 temp = (f_coordinate.xy / f_coordinate.w + 1.0) / 2.0;
	float depth = texture(texImage, temp.xy).r;
	if(gl_FragCoord.z <= depth+EPS1) discard;
	if(f_normalCameraSpace.z > EPS2)out_color = vec4(0.5);
	else if(f_normalCameraSpace.z < -EPS2)out_color = vec4(1.0);
	else out_color = vec4(0.0);
	//out_color = vec4(gl_FragCoord.z);
		
	//out_color = vec4(f_fragColor, 1.0);
}