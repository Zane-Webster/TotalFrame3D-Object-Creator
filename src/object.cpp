#include "Object.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Object::Object(std::string p_name, glm::vec3 p_position, TotalFrame::OBJECT_TYPE p_type, std::string p_obj_path, GLuint p_shader_program) : name(p_name), position(p_position), type(p_type), shader_program(p_shader_program) {
    Object::Load(p_obj_path);
}

//=============================
// BASIC FUNCTIONS
//=============================

void Object::Verify() {
    for (auto [shader_program, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            if (triangle.Verify() == false) {
                Util::ThrowError("INVALID VERTEX ARRAY", "Object::Verify");
            }
        }
    }
}

void Object::Load(std::string obj_path) {
    triangles[shader_program] = Object::_Read(obj_path);
}

void Object::Render() {
    // render all triangles
    for (auto [shader_program, triangles_i] : triangles) {
        glUseProgram(shader_program);
        for (auto triangle : triangles_i) {
            triangle.Render();
        }
    }
}

//=============================
// POSITIONAL FUNCTIONS
//=============================

glm::vec3 Object::GetPosition() {
    return glm::vec3(model_matrix[3][0], model_matrix[3][1], model_matrix[3][2]);
}

bool Object::IsVisible(glm::mat4 view_projection_matrix) {
    glm::vec4 clip_space_position = view_projection_matrix * glm::vec4(Object::GetPosition(), 1.0f);

    // normalize to NDC (-1 to 1)
    clip_space_position /= clip_space_position.w;

    // return if within screen bounds
    return (clip_space_position.x >= -1.0f && clip_space_position.x <= 1.0f) &&
        (clip_space_position.y >= -1.0f && clip_space_position.y <= 1.0f) &&
        (clip_space_position.z >= -1.0f && clip_space_position.z <= 1.0f); 
}

//=============================
// PRIVATE FUNCTIONS
//=============================

std::vector<Triangle> Object::_Read(std::string obj_path) {
    // return and throw error if path doesn't exist
    if (!std::filesystem::exists(obj_path)) {
        Util::ThrowError("INVALID OBJECT PATH", "Object::_ReadObject");
        return {};
    }

    //// READ FILE AND ADD TO LINES VECTOR

    std::vector<std::string> lines = {};
    std::ifstream obj_file(obj_path);
    std::string temp_line = "";

    // read file line by line
    while (std::getline(obj_file, temp_line)) {
        lines.push_back(temp_line);
    }

    //// CONVERT LINES TO TRIANGLE VECTOR

    std::vector<Triangle> temp_triangles = {};

    for (auto line : lines) {
        std::vector<GLfloat> temp_vertices = {};
        std::string temp_number_str = "";

        // convert each line from str to float and add to temp_vertices, check for ' ' to seperate each number
        for (int i = 0; i < line.size(); i++) {
            if (line[i] == ' ') {
                temp_vertices.push_back(std::stof(temp_number_str));
                temp_number_str = "";
                continue;
            }
            temp_number_str += line[i];
        }

        // push the last number because there is no space at the end
        temp_vertices.push_back(std::stof(temp_number_str));

        temp_triangles.push_back(Triangle(temp_vertices));
        temp_triangles.back().Build();
    }

    return temp_triangles;
}

//=============================
// MEMORY MANAGEMENT
//=============================

void Object::FreeAll() {
    // free all triangles
    for (auto [shader_program, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            triangle.FreeAll();
        }
    } 
}
