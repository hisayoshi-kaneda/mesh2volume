#version 330

int indices[] = int[]( 0, 1, 3, 0, 3, 2 );

out vec4 f_coordinate;

uniform mat4 u_mvpMat;
uniform vec2 size;
uniform vec3 center;
uniform float coordZ;

vec3 positions[] = vec3[](
	vec3(-size[0] / 2.0, -size[1] / 2.0, coordZ),
	vec3(-size[0] / 2.0,  size[1] / 2.0, coordZ),
	vec3( size[0] / 2.0, -size[1] / 2.0, coordZ),
	vec3( size[0] / 2.0,  size[1] / 2.0, coordZ)
);

void main(void) {
	gl_Position = u_mvpMat * vec4(positions[indices[gl_VertexID]] + center, 1.0);
	f_coordinate = gl_Position;
}
