#version 330
precision highp float;

const float EPS = 1.0e-3;

in vec3 f_posCameraSpace;
in vec3 f_normalCameraSpace;

out vec4 out_color;

void main() {
    float depth = length(f_posCameraSpace);
    vec3 color = vec3(depth, depth, depth);
    
    vec3 V = normalize(-f_posCameraSpace);
    vec3 N = normalize(f_normalCameraSpace);

    float inner = dot(V, N);
    if(inner > 0) {
        out_color = vec4(-color,1.0);
    } else {
        out_color = vec4(color, 1.0);
    }
}