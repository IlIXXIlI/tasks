#version 460

in vec3 v_color;

uniform float time;

void main() {
    gl_FragColor = vec4(v_color.x, v_color.y, v_color.z + abs(sin(time)), 1.0);
}