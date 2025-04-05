#include "Triangle.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Triangle::Triangle(std::vector<GLfloat> p_vertices) {
    LoadVertices(p_vertices);
}

//=============================
// BASIC FUNCTIONS
//=============================

bool Triangle::Verify() {
    if (vertex_array == 0 || vertex_buffer == 0) return false;
    return true;
}

void Triangle::LoadVertices(std::vector<GLfloat> p_vertices) {
    if (p_vertices.size() == 18) std::copy(p_vertices.begin(), p_vertices.end(), vertices.begin());
}

void Triangle::Build() {
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindVertexArray(vertex_array);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Triangle::Render() {
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::string Triangle::GetData(float aspect_ratio) {
    std::string temp_data = "";
    // undo aspect ratio stretching
    vertices[1] /= aspect_ratio;
    vertices[7] /= aspect_ratio;
    vertices[13] /= aspect_ratio;

    for (auto vertice : vertices) {
        temp_data += std::to_string(vertice);
        temp_data += ' ';
    }
    //get rid of the last space
    temp_data.pop_back();
    return temp_data;
}

//=============================
// MEMORY MANAGEMENT
//=============================

void Triangle::FreeAll() {
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    vertex_array = 0;
    vertex_buffer = 0;
}
