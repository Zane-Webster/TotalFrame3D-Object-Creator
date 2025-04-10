#include "Cube.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Cube::Cube() {
    ;
}

Cube::Cube(std::string p_name, glm::vec3 p_position, float p_size, std::string p_path, GLuint p_shader_program, float p_aspect_ratio, std::string data_str){
    Cube::Create(p_name, p_position, p_size, p_path, p_shader_program, p_aspect_ratio, data_str);
}

//=============================
// BASIC FUNCTIONS
//=============================

void Cube::Create(std::string p_name, glm::vec3 p_position, float p_size, std::string p_path, GLuint p_shader_program, float p_aspect_ratio, std::string data_str) {
    name = p_name;
    size = glm::vec3(p_size);
    true_size = glm::vec3(p_size);
    shader_program = p_shader_program;
    aspect_ratio = p_aspect_ratio;
    path = p_path;

    glm::vec3 temp_position = glm::vec3(0.0f);

    Cube::Load(p_path, temp_position, data_str);
    
    // if position is being read from file, read from file then set position, otherwise set defined position
    if (p_position == TotalFrame::READ_POS_FROM_FILE) Cube::SetPositionNoTriangles(temp_position);
    else Cube::SetPosition(p_position);

    // adjust for aspect_ratio (vertices are updated in Cube::_Read())
    size.y *= aspect_ratio;

    Cube::UpdatePosition(glm::vec3(0.0f));

    Cube::_BuildLines();
}

void Cube::Load(std::string path, glm::vec3& p_position_out, std::string data_str) {
    glm::vec3 position_out = glm::vec3(0.0f);
    // if there is no data already read, read the file, then pass to _CreateFromStr()
    if (data_str == "") triangles[shader_program] = Cube::_Read(path, position_out);
    // create triangles
    else triangles[shader_program] = Cube::_CreateFromStr(data_str, position_out);
    p_position_out = position_out;
}

void Cube::Render() {
    Cube::_BuildRenderLines();

    // render all triangles
    for (auto [shader_program, triangles_i] : triangles) {
        glUseProgram(shader_program);

        // set model matrix
        GLuint model_location = glGetUniformLocation(shader_program, "model_matrix");
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));

        for (auto triangle : triangles_i) {
            triangle.Render();
        }
    }
    
    Cube::_RenderLines();
}

std::string Cube::GetData() {
    std::string temp_data = "";
    // get posiiton
    for (int i = 0; i < 3; i++) {
        temp_data += std::to_string(translated_true_position[i]);
        temp_data += ' ';
    }
    temp_data.pop_back();
    temp_data += '\n';

    // get vertices data
    for (auto [sp, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            temp_data += triangle.GetData();
            temp_data += '\n';
        } 
    }

    return temp_data;
}

std::string Cube::GetTrueData() {
    std::string temp_data = "";
    // get posiiton
    for (int i = 0; i < 3; i++) {
        temp_data += std::to_string(translated_true_position[i]);
        temp_data += ' ';
    }
    temp_data.pop_back();
    temp_data += '\n';

    // get true vertices data
    for (auto [sp, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            temp_data += triangle.GetTrueData();
            temp_data += '\n';
        }
    }
    return temp_data;
}

void Cube::Verify() {
    for (auto [shader_program, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            if (triangle.Verify() == false) {
                Util::ThrowError("INVALID VERTEX ARRAY", "Cube::Verify");
            }
        }
    }
}

//=============================
// EXPORTATION FUNCTIONS
//=============================
std::string Cube::GetExportData() {
    std::string temp_data = "";
    // get vertices data
    for (auto [sp, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            temp_data += triangle.GetData();
            temp_data += '\n';
        } 
    }

    return temp_data;
}

//=============================
// COLOR FUNCTIONS
//=============================

void Cube::SetColor(glm::vec3 color) {
    for (auto [sp, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            triangle.SetColor(color);
        }
    }
}

//=============================
// POSITIONAL FUNCTIONS
//=============================

void Cube::UpdatePosition(glm::vec3 camera_position) {
    // find distance from camera to cube
    float distance = glm::length(position - camera_position);

    // scale factor based on distance from cube. clamped to prevent extremes
    float scale_factor = glm::clamp(distance * 0.1f, 0.5f, 5.0f);

    // scale transformation based on model matrix scaling
    glm::vec3 scale_transformation = glm::vec3(
        glm::length(glm::vec3(model_matrix[0][0], model_matrix[0][1], model_matrix[0][2])), // X scale
        glm::length(glm::vec3(model_matrix[1][0], model_matrix[1][1], model_matrix[1][2])), // Y scale
        glm::length(glm::vec3(model_matrix[2][0], model_matrix[2][1], model_matrix[2][2]))  // Z scale
    );
    // combine for final scale amount
    glm::vec3 final_scale = scale_transformation * scale_factor;
    // rotate and scale the axes for obb
    glm::mat3 rotation_matrix = glm::mat3(model_matrix);
    for (int i = 0; i < 3; i++) {
        axes[i] = rotation_matrix[i] * final_scale[i];
    }

    // scale the camera size
    camera_scaled_size = size * final_scale;
    half_size = camera_scaled_size * 0.5f;

    // calculate corners of the cube
    corners = {
        position + rotation_matrix * glm::vec3(-camera_scaled_size.x, -camera_scaled_size.y, -camera_scaled_size.z), // min x, min y, min z
        position + rotation_matrix * glm::vec3( camera_scaled_size.x, -camera_scaled_size.y, -camera_scaled_size.z), // max x, min y, min z
        position + rotation_matrix * glm::vec3( camera_scaled_size.x,  camera_scaled_size.y, -camera_scaled_size.z), // max x, max y, min z
        position + rotation_matrix * glm::vec3(-camera_scaled_size.x,  camera_scaled_size.y, -camera_scaled_size.z), // min x, max y, min z
        position + rotation_matrix * glm::vec3(-camera_scaled_size.x, -camera_scaled_size.y,  camera_scaled_size.z), // min x, min y, max z
        position + rotation_matrix * glm::vec3( camera_scaled_size.x, -camera_scaled_size.y,  camera_scaled_size.z), // max x, min y, max z
        position + rotation_matrix * glm::vec3( camera_scaled_size.x,  camera_scaled_size.y,  camera_scaled_size.z), // max x, max y, max z
        position + rotation_matrix * glm::vec3(-camera_scaled_size.x,  camera_scaled_size.y,  camera_scaled_size.z)  // min x, max y, max z
    };

    std::vector<glm::vec3> lines_corners = {
        true_position + rotation_matrix * glm::vec3(-camera_scaled_size.x, -camera_scaled_size.y, -camera_scaled_size.z), // min x, min y, min z
        true_position + rotation_matrix * glm::vec3( camera_scaled_size.x, -camera_scaled_size.y, -camera_scaled_size.z), // max x, min y, min z
        true_position + rotation_matrix * glm::vec3( camera_scaled_size.x,  camera_scaled_size.y, -camera_scaled_size.z), // max x, max y, min z
        true_position + rotation_matrix * glm::vec3(-camera_scaled_size.x,  camera_scaled_size.y, -camera_scaled_size.z), // min x, max y, min z
        true_position + rotation_matrix * glm::vec3(-camera_scaled_size.x, -camera_scaled_size.y,  camera_scaled_size.z), // min x, min y, max z
        true_position + rotation_matrix * glm::vec3( camera_scaled_size.x, -camera_scaled_size.y,  camera_scaled_size.z), // max x, min y, max z
        true_position + rotation_matrix * glm::vec3( camera_scaled_size.x,  camera_scaled_size.y,  camera_scaled_size.z), // max x, max y, max z
        true_position + rotation_matrix * glm::vec3(-camera_scaled_size.x,  camera_scaled_size.y,  camera_scaled_size.z)  // min x, max y, max z
    };

    lines_vertices = {
        lines_corners[0], lines_corners[1], // min x, min y, min z -> max x, min y, min z
        lines_corners[1], lines_corners[2], // max x, min y, min z -> max x, max y, min z
        lines_corners[2], lines_corners[3], // max x, max y, min z -> min x, max y, min z
        lines_corners[3], lines_corners[0], // min x, max y, min z -> min x, min y, min z
        
        lines_corners[4], lines_corners[5], // min x, min y, max z -> max x, min y, max z
        lines_corners[5], lines_corners[6], // max x, min y, max z -> max x, max y, max z
        lines_corners[6], lines_corners[7], // max x, max y, max z -> min x, max y, max z
        lines_corners[7], lines_corners[4], // min x, max y, max z -> min x, min y, max z
        
        lines_corners[0], lines_corners[4], // min x, min y, min z -> min x, min y, max z
        lines_corners[1], lines_corners[5], // max x, min y, min z -> max x, min y, max z
        lines_corners[2], lines_corners[6], // max x, max y, min z -> max x, max y, max z
        lines_corners[3], lines_corners[7]  // min x, max y, min z -> min x, max y, max z
    };
}


glm::vec3 Cube::GetPosition() {
    return glm::vec3(model_matrix[3][0], model_matrix[3][1], model_matrix[3][2]);
}

glm::vec3 Cube::GetTTPosition() {
    return glm::vec3(translated_true_model_matrix[3][0], translated_true_model_matrix[3][1], translated_true_model_matrix[3][2]);
}

void Cube::SetPosition(glm::vec3 p_position) {
    Cube::SetPositionNoTriangles(p_position);

    for (auto [sp, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            triangle.SetPosition(translated_true_position, aspect_ratio);
        }
    }
}

void Cube::SetPositionNoTriangles(glm::vec3 p_position) {
    translated_true_model_matrix[3][0] = p_position[0];
    translated_true_model_matrix[3][1] = p_position[1];
    translated_true_model_matrix[3][2] = p_position[2];
    translated_true_position = p_position;

    glm::vec3 stretched_position = p_position;
    stretched_position[1] *= aspect_ratio;

    model_matrix[3][0] = stretched_position[0];
    model_matrix[3][1] = stretched_position[1];
    model_matrix[3][2] = stretched_position[2];
    position = stretched_position;
}

bool Cube::IsVisible(glm::mat4 view_projection_matrix) {
    for (auto corner : corners) {
        glm::vec4 clip_space_position = view_projection_matrix * glm::vec4(corner, 1.0f);
        // normalize to -1 thru 1 by the transformation value
        clip_space_position /= clip_space_position.w;
    
        // return if within screen bounds
        if ((clip_space_position.x >= -1.0f && clip_space_position.x <= 1.0f) &&
            (clip_space_position.y >= -1.0f && clip_space_position.y <= 1.0f) &&
            (clip_space_position.z >= -1.0f && clip_space_position.z <= 1.0f)) {
            return true;
        }
    }

    return false;
}

//=============================
// TRANSLATION FUNCTIONS
//=============================

void Cube::Translate(glm::vec3 translation) {
    glm::vec3 translated_position = translated_true_position + translation;
    Cube::SetPosition(translated_position);
}

void Cube::ResetTranslation() {
    translated_true_position = true_position;
    translated_true_model_matrix = true_model_matrix;

    position = true_position;
    model_matrix = true_model_matrix;
}

//=============================
// RAY FUNCTIONS
//=============================

bool Cube::RayCollides(TotalFrame::Ray ray) {
    float dummy = 0.0f;
    return Cube::RayCollides(ray, dummy);
}

bool Cube::RayCollides(TotalFrame::Ray ray, float& tmin_out) {
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::max();

    // ray direction from ray origin to cube position (center)
    glm::vec3 ray_to_pos = position - ray.origin;

    // for each axes
    for (int i = 0; i < 3; i++) {
        // the difference in angle between the ray direction and axes direction
        float axis_projection = glm::dot(ray.direction, axes[i]);
        // the distance between this cubes center and the ray direction
        float distance = glm::dot(ray_to_pos, axes[i]);

        // if the float abs of axis_projection (difference in angle) is near 0 or greater, continue
        if (std::fabs(axis_projection) > 1e-6) {
            // calculate where the ray enters and exits the along this axis
            float t1 = (distance - half_size[i]) / axis_projection;
            float t2 = (distance + half_size[i]) / axis_projection;

            if (t1 > t2) std::swap(t1, t2);

            tmin = glm::max(tmin, t1);
            tmax = glm::min(tmax, t2);
            
            // if tmin > tmax, there is no overlap between the camera ray and cube
            if (tmin > tmax) return false;

        // if the distance is too far from the half_size, there is no overlap
        } else if (-distance > half_size[i] || distance > half_size[i]) {
            return false;
        }
    }

    // tmin output for determining the first cube hit
    tmin_out = tmin;

    // they do overlap
    return true;
}

bool Cube::RayCollidesWithFace(TotalFrame::Ray ray, float& tmin_out, glm::vec3& face_hit_normal_out) {
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::max();

    // ray direction from ray origin to cube position (center)
    glm::vec3 ray_to_pos = Cube::GetPosition() - ray.origin;

    face_hit_normal_out = glm::vec3(-1000.0f);

    // for each axes
    for (int i = 0; i < 3; i++) {
        // the difference in angle between the ray direction and axes direction
        float axis_projection = glm::dot(ray.direction, axes[i]);
        // the distance between this cube center and the ray direction
        float distance = glm::dot(ray_to_pos, axes[i]);

        // if the float abs of axis_projection (difference in angle) is near 0 or greater, continue
        if (std::fabs(axis_projection) > 1e-6) {
            // calculate where the ray enters and exits the along this axis
            float t1 = (distance - half_size[i]) / axis_projection;
            float t2 = (distance + half_size[i]) / axis_projection;

            bool entering_negative = t1 < t2;
            if (!entering_negative) std::swap(t1, t2);

            if (t1 > tmin) {
                tmin = t1;
                // if entering negative, -axes[i], otherwise axes[i]
                face_hit_normal_out = entering_negative ? -axes[i] : axes[i];
            }
            tmax = glm::min(tmax, t2);
            
            // if tmin > tmax, there is no overlap between the camera ray and cube
            if (tmin > tmax) return false;

        // if the distance is too far from the half_size, there is no overlap
        } else if (-distance > half_size[i] || distance > half_size[i]) {
            return false;
        }
    }

    // tmin output for determining the first cube hit
    tmin_out = tmin;

    // they do overlap
    return true;
}

//=============================
// PRIVATE FUNCTIONS
//=============================

std::vector<Triangle> Cube::_Read(std::string path, glm::vec3& p_position_out) {
    // return and throw error if path doesn't exist
    if (!std::filesystem::exists(path)) {
        Util::ThrowError("INVALID CUBE PATH", "Cube::_Read");
        return {};
    }

    //// read file, add to string then pass through Cube::_CreateFromStr
    std::ifstream file(path);
    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    glm::vec3 temp_position_out = glm::vec3(0.0f);
    std::vector<Triangle> temp_triangles = Cube::_CreateFromStr(data, temp_position_out); 

    p_position_out = temp_position_out;

    return temp_triangles;
}

std::vector<Triangle> Cube::_CreateFromStr(std::string data_str, glm::vec3& p_position_out) {
    std::vector<Triangle> temp_triangles = {};
    std::vector<std::string> temp_vertices_str = {};
    std::vector<GLfloat> temp_vertices = {};
    std::vector<GLfloat> temp_true_vertices = {};
    
    glm::vec3 position_out = glm::vec3(0.0f);
    std::vector<GLfloat> temp_position_out = {};
    
    // read through the entire cube data string and split it into triangle data lines
    std::string temp_number_str = "";
    for (auto letter : data_str) {
        if (letter == '\n') {
            temp_vertices_str.push_back(temp_number_str);
            temp_number_str = "";
            continue;
        }
        temp_number_str += letter;
    }

    // read through first line and set position out
    std::string temp_number_str_pos = "";
    for (auto letter : temp_vertices_str[0]) {
        if (letter == ' ') {
            temp_position_out.push_back(std::stof(temp_number_str_pos));
            temp_number_str_pos = "";
            continue;
        }
        temp_number_str_pos += letter;
    }

    temp_position_out.push_back(std::stof(temp_number_str_pos));

    position_out[0] = temp_position_out[0];
    position_out[1] = temp_position_out[1];
    position_out[2] = temp_position_out[2];

    // read through each set of vertices (triangle data), add to temp_vertices, create a triangle from the temp_vertices, and build the triangle
    std::string temp_number_str_vertices = "";
    bool first_line = true;
    for (auto line : temp_vertices_str) {
        if (!first_line) {
            temp_vertices = {};
            temp_true_vertices = {};
            temp_number_str_vertices = "";

            for (auto letter : line) {
                if (letter == ' ') {
                    GLfloat value = std::stof(temp_number_str_vertices);
                    temp_vertices.push_back(value);
                    temp_true_vertices.push_back(value);

                    // if the number is in that position, that means it is a y value. adjust the y value by aspect ratio
                    if (temp_vertices.size() == 2 || temp_vertices.size() == 8 || temp_vertices.size() == 14) temp_vertices.back() *= aspect_ratio;
                    
                    temp_number_str_vertices = "";
                    continue;
                }
                temp_number_str_vertices += letter;
            }

            GLfloat value = std::stof(temp_number_str_vertices);
            temp_vertices.push_back(value);
            temp_true_vertices.push_back(value);
            
            temp_triangles.push_back(Triangle(temp_vertices, temp_true_vertices));
        }
        first_line = false;
    }

    
    p_position_out = position_out;
    return temp_triangles;
}

//=============================
// LINE FUNCTIONS
//=============================

void Cube::_BuildRenderLines() {
    // update the VBO with new line data
    glBindVertexArray(lines_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, lines_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * lines_vertices.size(), lines_vertices.data());
    glBindVertexArray(0);
}

void Cube::_BuildLines() {
    // generate line vertex buffer and array
    glGenVertexArrays(1, &lines_vertex_array);
    glGenBuffers(1, &lines_vertex_buffer);

    glBindVertexArray(lines_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, lines_vertex_buffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * lines_vertices.size(), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Cube::_RenderLines() {
    glBindVertexArray(lines_vertex_array);
    glDrawArrays(GL_LINES, 0, lines_vertices.size());
    glBindVertexArray(0);
}

//=============================
// MEMORY MANAGEMENT
//=============================

void Cube::FreeAll() {
    // free all triangles
    for (auto [shader_program, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            triangle.FreeAll();
        }
    } 
    // free lines
    glDeleteVertexArrays(1, &lines_vertex_array);
    glDeleteBuffers(1, &lines_vertex_buffer);
    lines_vertex_array = 0;
    lines_vertex_buffer = 0;
}
