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

//=============================
// MEMORY MANAGEMENT
//=============================

void Triangle::FreeAll() {
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    vertex_array = 0;
    vertex_buffer = 0;
}
