#version 330 core

layout(location = 0) in vec3 aPos;       // Vertex position
layout(location = 1) in vec3 aNormal;    // Vertex normal

uniform mat4 modeling;
uniform mat4 camera;
uniform mat4 projection;

out vec3 wPos;      // Position in world space
out vec3 wNormal;       // Normal in world space

void main() {
    vec4 worldPos = modeling * vec4(aPos, 1.0);
    wPos = worldPos.xyz;
    wNormal = mat3(transpose(inverse(modeling))) * aNormal;
    gl_Position = projection * camera * worldPos;
}