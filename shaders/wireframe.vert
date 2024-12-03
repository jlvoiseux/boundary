#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

uniform mat4 mvp;
uniform int renderMode;

out vec2 fragBary;
out vec3 fragNormal;

void main() {
    fragBary = vertexTexCoord;
    fragNormal = vertexNormal;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}