#version 330 core

in vec3 vertex_color;   // Color from the vertex shader for the fill
//in vec3 edge_color;   // Color for the outline (passed from geometry shader)

out vec4 frag_color;  // Final output color

void main() {
    // If the edge color is black, it's part of the outline
    //if (edge_color == vec3(0.0, 0.0, 0.0)) {
    //    frag_color = vec4(edge_color, 1.0); // Outline color (black)
    //} else {
    frag_color = vec4(vertex_color, 1.0); // Fill color
    //}
}