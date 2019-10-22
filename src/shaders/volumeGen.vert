#version 330

int indices[] = int[]( 0, 1, 3, 0, 3, 2 );

out vec3 f_posWorldSpace;

uniform mat4 u_mvpMat;
uniform vec3 size;
uniform float resolution;
uniform float coordZ;

vec3 positions[] = vec3[](
	vec3( -size[0] * resolution /2.0, -size[1] * resolution /2.0, coordZ),
	vec3( -size[0] * resolution / 2.0,  size[1] * resolution / 2.0, coordZ),
	vec3( size[0] * resolution / 2.0,  -size[1] * resolution /2.0, coordZ),
	vec3( size[0] * resolution / 2.0,  size[1] * resolution / 2.0, coordZ)
);

void main(void) {
	gl_Position = u_mvpMat * vec4(positions[indices[gl_VertexID]], 1.0);
	f_posWorldSpace = positions[indices[gl_VertexID]];
}
