#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
// layout(location = 2) in vec2 aTexCoord;

out vec3 ourColor;
// out vec2 vTexCoord;

uniform mat4 uProjection;
// uniform vec2 uUV0;
// uniform vec2 uUV1;

void main() {
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    ourColor = aColor;
    // vTexCoord = mix(uUV0, uUV1, aTexCoord);
}
