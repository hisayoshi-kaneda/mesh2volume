#version 330

vec3 positions[] = vec3[](
	vec3(-1.0, -1.0, 0.0),
	vec3(-1.0,  1.0, 0.0),
	vec3( 1.0, -1.0, 0.0),
	vec3( 1.0,  1.0, 0.0)
);

int indices[] = int[]( 0, 1, 3, 0, 3, 2 );

out vec3 f_posCameraSpace;

uniform mat4 u_projMat;

void main(void) {
	gl_Position = vec4(positions[indices[gl_VertexID]], 1.0);

	vec4 temp = inverse(u_projMat) * vec4(positions[indices[gl_VertexID]], 1.0);
	f_posCameraSpace = temp.xyz / temp.w;
}
