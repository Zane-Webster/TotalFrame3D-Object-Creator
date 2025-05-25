#version 330 core

in vec3 frag_position;
in vec3 frag_normal;
in vec3 base_color;

out vec4 frag_color;

uniform vec3 light_position;  // Light position in world space
uniform vec3 view_position;   // Camera position in world space
uniform float light_intensity;

int dither4x4[16] = int[16](
     0,  8,  2, 10,
    12,  4, 14,  6,
     3, 11,  1,  9,
    15,  7, 13,  5
);

// Function to fetch Bayer value based on pixel position
float GetDitherValue(ivec2 pixel_coords) {
    int x = pixel_coords.x % 4;
    int y = pixel_coords.y % 4;
    return float(dither4x4[y * 4 + x]) / 16.0;
}

void main() {
    // Ambient
    vec3 ambient = 0.15 * base_color;

    // Diffuse
    vec3 light_dir = normalize(light_position - frag_position);
    float diff = max(dot(frag_normal, light_dir), 0.0);
    vec3 diffuse = diff * base_color * light_intensity;

    // Specular
    vec3 view_dir = normalize(view_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0); // shininess = 32
    vec3 specular = spec * vec3(1.0); // White specular highlights

    // Combine lighting
    vec3 result = ambient + diffuse + specular;

    // Apply dithering-based color quantization
    float levels = 4.0; // Try 2.0, 3.0, or 4.0 for stronger effect
    float dither = GetDitherValue(ivec2(gl_FragCoord.xy));
    vec3 quantized = floor(result * levels + dither) / levels;

    frag_color = vec4(quantized, 1.0);
}