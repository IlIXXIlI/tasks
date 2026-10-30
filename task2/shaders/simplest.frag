#version 460

in vec3 v_color;

uniform float time;

void main() {
    gl_FragColor = vec4(v_color, 1.0);
}