#version 330 core

in vec3 vColor;
in vec2 fragPos;

out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 0.8);
}
