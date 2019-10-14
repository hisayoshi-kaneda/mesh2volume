#version 330
precision highp float;

vec3 positions[] = vec3[](
	vec3(-1.0, -1.0, 0.0),
	vec3(-1.0,  1.0, 0.0),
	vec3( 1.0, -1.0, 0.0),
	vec3( 1.0,  1.0, 0.0)
);

int indices[] = int[]( 0, 1, 3, 0, 3, 2 );

out vec3 f_posTexture;

uniform mat4 u_projMat;

void main(void) {
	gl_Position = vec4(positions[indices[gl_VertexID]], 1.0);

	f_posTexture = (vec3(gl_Position)+1.0)/2.0;
}
