#version 460

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 v_color;
uniform float time;

void main() {
    gl_Position = vec4(a_pos.x + abs(cos(time))-0.5, a_pos.y, a_pos.z, 1.0);
    v_color = a_color;
}