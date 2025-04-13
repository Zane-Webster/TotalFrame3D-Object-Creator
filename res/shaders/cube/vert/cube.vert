#version 330 core

layout(location = 0) in vec3 position; // Vertex position
layout(location = 1) in vec3 color;    // Vertex color (used for base color)
layout(location = 2) in vec3 normal;   // Vertex normal

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix; // Inverse transpose of the model matrix (for normals)

out vec3 frag_position;     // Position in world space
out vec3 frag_normal;       // Normal in world space
out vec3 base_color;        // Color passed to the fragment shader

void main() {
    vec4 world_position = model_matrix * vec4(position, 1.0);
    frag_position = world_position.xyz;

    frag_normal = normalize(normal_matrix * normal);
    base_color = color;

    gl_Position = projection * view * world_position;
}