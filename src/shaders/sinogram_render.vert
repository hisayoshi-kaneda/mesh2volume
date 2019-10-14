#version 330
precision highp float;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 f_posCameraSpace;
out vec3 f_normalCameraSpace;

uniform mat4 u_mvMat;
uniform mat4 u_mvpMat;
uniform mat4 u_normMat;

void main() {
    gl_Position = u_mvpMat * vec4(in_position, 1.0);
    f_posCameraSpace = (u_mvMat * vec4(in_position, 1.0)).xyz;
    f_normalCameraSpace = (u_normMat * vec4(in_normal, 0.0)).xyz;
}
