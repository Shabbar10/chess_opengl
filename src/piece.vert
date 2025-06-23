#version 420 core

layout(location = 0) in vec2 aPos;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uProjection;
uniform mat4 uModel;
uniform vec2 uUV0;
uniform vec2 uUV1;

out vec2 vTexCoord;

void main() {
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
    vTexCoord = mix(uUV0, uUV1, aTexCoord);
}
