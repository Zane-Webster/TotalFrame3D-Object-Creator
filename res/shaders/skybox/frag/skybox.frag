#version 330 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;

// 4x4 Bayer matrix as a 1D array
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

void main()
{
    vec4 color = texture(skybox, TexCoords);

    // Apply dithering-based color quantization
    float levels = 4.0; // number of quantization levels
    float dither = GetDitherValue(ivec2(gl_FragCoord.xy));

    vec3 quantized = floor(color.rgb * levels + dither) / levels;

    FragColor = vec4(quantized, color.a);
}