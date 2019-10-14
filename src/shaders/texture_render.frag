#version 330
precision highp float;

in vec3 f_posTexture;

out vec4 out_color;

uniform sampler2D sinogram;
 
void main(void){
    out_color = vec4(vec3(texture(sinogram, f_posTexture.xy).xyz), 1.0);
}
