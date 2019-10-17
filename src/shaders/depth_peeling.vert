#version 330
precision highp float;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 f_fragColor;
out vec4 f_coordinate;
out vec3 f_normalCameraSpace;

uniform mat4 u_mvpMat;
uniform mat4 u_normMat;

void main() {
    gl_Position = u_mvpMat*vec4(in_position,1.0);
	f_coordinate = u_mvpMat*vec4(in_position,1.0);
    f_fragColor = in_normal * 0.5 + 0.5;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
}