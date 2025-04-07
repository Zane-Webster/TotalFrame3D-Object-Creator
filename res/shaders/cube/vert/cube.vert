#version 330 core

layout(location = 0) in vec3 position; // Vertex position
layout(location = 1) in vec3 color;    // Vertex color (used for filling)

uniform mat4 model_matrix; // Model transformation matrix
uniform mat4 view;         // View transformation matrix
uniform mat4 projection;   // Projection transformation matrix

out vec3 vertex_color; // Color passed to the fragment shader

void main() {
    // Apply the model, view, and projection matrices to transform the vertex position
    gl_Position = projection * view * model_matrix * vec4(position, 1.0);

    // Pass the color to the fragment shader
    vertex_color = color;
}