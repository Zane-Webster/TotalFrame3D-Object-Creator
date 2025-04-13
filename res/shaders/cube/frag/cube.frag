#version 330 core

in vec3 frag_position;
in vec3 frag_normal;
in vec3 base_color;

out vec4 frag_color;

uniform vec3 light_position;  // Light position in world space
uniform vec3 view_position;   // Camera position in world space
uniform float light_intensity;

void main() {
    // Ambient
    vec3 ambient = 0.3 * base_color;

    // Diffuse
    vec3 light_dir = normalize(light_position - frag_position);
    float diff = max(dot(frag_normal, light_dir), 0.0);
    vec3 diffuse = diff * base_color * light_intensity;

    // Specular
    vec3 view_dir = normalize(view_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0); // shininess = 32
    vec3 specular = spec * vec3(1.0); // White specular highlights

    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0);
}