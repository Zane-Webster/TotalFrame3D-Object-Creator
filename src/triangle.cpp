#include "Triangle.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Triangle::Triangle(std::vector<GLfloat> p_vertices, std::vector<GLfloat> p_true_vertices) {
    Triangle::LoadVertices(p_vertices, p_true_vertices);
    Triangle::Build();
}

//=============================
// BASIC FUNCTIONS
//=============================

bool Triangle::Verify() {
    if (vertex_array == 0 || vertex_buffer == 0) return false;
    return true;
}

void Triangle::LoadVertices(std::vector<GLfloat> p_vertices, std::vector<GLfloat> p_true_vertices) {
    vertices = std::make_shared<TF_TRIANGLE_VERTICES>();
    true_vertices = std::make_shared<TF_TRIANGLE_VERTICES>();
    translated_true_vertices = std::make_shared<TF_TRIANGLE_VERTICES>();
    if (p_vertices.size() == 18) {
        std::copy(p_vertices.begin(), p_vertices.end(), vertices->begin());
        std::copy(p_true_vertices.begin(), p_true_vertices.end(), true_vertices->begin());
        std::copy(p_true_vertices.begin(), p_true_vertices.end(), translated_true_vertices->begin());
    }
}

void Triangle::Build() {
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindVertexArray(vertex_array);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), vertices->data(), GL_STATIC_DRAW);
    
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
    glDrawArrays(GL_TRIANGLES, 0, 3);  // Draw the triangle (filled)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Triangle::RenderOutline() {
    glBindVertexArray(vertex_array);
    glDrawArrays(GL_LINE_LOOP, 0, 3);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::string Triangle::GetData() {
    std::string temp_data = "";

    for (auto vertice : (*translated_true_vertices)) {
        temp_data += std::to_string(vertice);
        temp_data += ' ';
    }

    //get rid of the last space
    temp_data.pop_back();
    return temp_data;
}

std::string Triangle::GetTrueData() {
    std::string temp_data = "";

    for (auto vertice : (*true_vertices)) {
        temp_data += std::to_string(vertice);
        temp_data += ' ';
    }
    //get rid of the last space
    temp_data.pop_back();
    return temp_data;
}

//=============================
// MOVEMENT FUNCTIONS
//=============================

void Triangle::SetPosition(glm::vec3 position, float aspect_ratio) {
    int stride = 6;
    for (int i = 0; i < 3; ++i) {
        int index = i * stride;
        (*translated_true_vertices)[index + 0] = position[0] + (*true_vertices)[index + 0];
        (*translated_true_vertices)[index + 1] = position[1] + (*true_vertices)[index + 1];
        (*translated_true_vertices)[index + 2] = position[2] + (*true_vertices)[index + 2];

        glm::vec3 stretched_position = position;
        stretched_position[1] *= aspect_ratio;
        (*vertices)[index + 0] = stretched_position[0] + (*true_vertices)[index + 0];
        (*vertices)[index + 1] = stretched_position[1] + (*true_vertices)[index + 1];
        (*vertices)[index + 2] = stretched_position[2] + (*true_vertices)[index + 2];
    }

    Triangle::Build();
}

//=============================
// COLOR FUNCTIONS
//=============================

void Triangle::SetColor(glm::vec3 color) {
    int stride = 6;
    for (int i = 0; i < 3; ++i) {
        int index = i * stride + 3;
        (*translated_true_vertices)[index + 0] = color[0];
        (*translated_true_vertices)[index + 1] = color[1];
        (*translated_true_vertices)[index + 2] = color[2];

        (*true_vertices)[index + 0] = color[0];
        (*true_vertices)[index + 1] = color[1];
        (*true_vertices)[index + 2] = color[2];

        (*vertices)[index + 0] = color[0];
        (*vertices)[index + 1] = color[1];
        (*vertices)[index + 2] = color[2];
    }

    Triangle::Build();
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