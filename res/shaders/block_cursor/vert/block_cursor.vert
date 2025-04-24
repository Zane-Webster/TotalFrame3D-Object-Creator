#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model_matrix * vec4(aPos, 1.0f);
    vertexColor = vec3(1.0f, 1.0f, 1.0f);
}