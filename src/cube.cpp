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
    shader_program = p_shader_program;
    aspect_ratio = p_aspect_ratio;
    path = p_path;

    glm::vec3 temp_position = glm::vec3(0.0f);

    Cube::Load(p_path, temp_position, data_str);

    // if position is being read from file, read from file then set position, otherwise set defined position
    if (p_position == TotalFrame::READ_POS_FROM_FILE) Cube::SetPosition(temp_position);
    else Cube::SetPosition(p_position);

    *initial_model_matrix = *model_matrix;

    Cube::UpdateStretch();

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

void Cube::Render(glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights) {
    Cube::_BuildRenderLines();

    // render all triangles
    for (auto& [shader_program, triangles_i] : triangles) {
        glUseProgram(shader_program);

        glUniformMatrix4fv(glGetUniformLocation(shader_program, "model_matrix"), 1, GL_FALSE, glm::value_ptr(*stretched_model_matrix));

        glUniform3fv(glGetUniformLocation(shader_program, "light_position"), 1, glm::value_ptr(lights[0]->position));
        glUniform1f(glGetUniformLocation(shader_program, "light_intensity"), lights[0]->intensity);
        glUniform3fv(glGetUniformLocation(shader_program, "view_position"), 1, glm::value_ptr(camera_position));

        // set normal matrix
        glUniformMatrix3fv(glGetUniformLocation(shader_program, "normal_matrix"), 1, GL_FALSE, glm::value_ptr(*normal_matrix));

        for (auto& triangle : triangles_i) {
            triangle.Render();
        }
    }
    
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "model_matrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    Cube::_RenderLines();
}

std::string Cube::GetData() {
    std::string temp_data = "";

    glm::vec3 temp_position = Cube::GetPosition();

    // get posiiton
    for (int i = 0; i < 3; i++) {
        temp_data += std::to_string(temp_position[i]);
        temp_data += ' ';
    }
    temp_data.pop_back();
    temp_data += '\n';

    // get vertices data
    for (auto& [sp, triangles_i] : triangles) {
        for (auto& triangle : triangles_i) {
            temp_data += triangle.GetData();
            temp_data += '\n';
        } 
    }

    return temp_data;
}

void Cube::Verify() {
    for (auto& [shader_program, triangles_i] : triangles) {
        for (auto& triangle : triangles_i) {
            if (triangle.Verify() == false) {
                Util::ThrowError("INVALID VERTEX ARRAY", "Cube::Verify");
            }
        }
    }
}

//=============================
// EXPORTATION FUNCTIONS
//=============================

std::vector<std::array<TotalFrame::Ray, 14>> Cube::GetCornersRays() {
    std::vector<glm::vec3> export_corners = {};

    glm::vec3 center = Cube::GetPosition();
    glm::vec3 half_size = size * 0.5f;

    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                glm::vec3 offset = glm::vec3(x, y, z) * half_size;
                export_corners.push_back(center + offset);
            }
        }
    }

    std::vector<glm::vec3> directions = {
        // Axis-aligned directions (±1, 0, 0), (0, ±1, 0), (0, 0, ±1)
        glm::vec3(1, 0, 0),  // +X
        glm::vec3(-1, 0, 0), // -X
        glm::vec3(0, 1, 0),  // +Y
        glm::vec3(0, -1, 0), // -Y
        glm::vec3(0, 0, 1),  // +Z
        glm::vec3(0, 0, -1), // -Z

        // Corner diagonals (±1, ±1, ±1)
        glm::normalize(glm::vec3(1, 1, 1)),   // +X +Y +Z
        glm::normalize(glm::vec3(-1, 1, 1)),  // -X +Y +Z
        glm::normalize(glm::vec3(1, -1, 1)),  // +X -Y +Z
        glm::normalize(glm::vec3(1, 1, -1)),  // +X +Y -Z
        glm::normalize(glm::vec3(-1, -1, 1)), // -X -Y +Z
        glm::normalize(glm::vec3(-1, 1, -1)), // -X +Y -Z
        glm::normalize(glm::vec3(1, -1, -1)), // +X -Y -Z
        glm::normalize(glm::vec3(-1, -1, -1)) // -X -Y -Z
    };

    std::vector<std::array<TotalFrame::Ray, 14>> temp_corners_rays = {};

    for (const auto& corner : export_corners) {
        std::array<TotalFrame::Ray, 14> temp_corner_rays;  // one array per corner

        for (size_t d = 0; d < directions.size(); d++) {
            temp_corner_rays[d].origin = corner;
            temp_corner_rays[d].direction = glm::normalize(directions[d]);
        }

        temp_corners_rays.push_back(temp_corner_rays);
    }

    return temp_corners_rays;
}

bool Cube::RayCollidesWithCorners(TotalFrame::Ray ray, glm::vec3 ignore_point) {
    glm::vec3 center = Cube::GetPosition();
    glm::vec3 half_size = size * 0.5f;

    for (int x = -1; x <= 1; x += 2) {
        for (int y = -1; y <= 1; y += 2) {
            for (int z = -1; z <= 1; z += 2) {
                glm::vec3 corner = center + glm::vec3(x, y, z) * half_size;

                if (glm::all(glm::epsilonEqual(corner, ignore_point, 0.001f))) continue;
            
                glm::vec3 to_corner = corner - ray.origin;
                float t = glm::dot(to_corner, ray.direction); // projection of corner onto ray direction
            
                if (t < 0.0f) continue; // corner is behind ray origin
            
                glm::vec3 closest_point = ray.origin + t * ray.direction;
                float dist = glm::distance(closest_point, corner);
            
                if (dist < 0.01f) return true; // adjust threshold as needed
            }
        }
    }

    return false;
}

void Cube::RemoveTrianglesByCorners(std::vector<glm::vec3> removed_corners) {
    glm::vec3 cube_pos = GetPosition();

    for (auto& [sp, triangles_i] : triangles) {
        // erase triangle if condition returns true
        triangles_i.erase(
            std::remove_if(triangles_i.begin(), triangles_i.end(),
                [&](const Triangle& tri) {

                    auto raw = tri.vertices;

                    // if not valid vertices
                    if (!raw || raw->size() < 18) return false;

                    // Check each of the 3 triangle vertices
                    for (int i = 0; i < 18; i += 6) {
                        glm::vec3 translated_vertex = glm::vec3((*raw)[i], (*raw)[i+1], (*raw)[i+2]) + cube_pos;

                        // compare against removed_corners, if matches, return true and erase
                        for (const auto& corner : removed_corners) {
                            // gives 0.001 error margin
                            if (glm::all(glm::epsilonEqual(translated_vertex, corner, 0.001f))) {
                                return true; // Remove this triangle
                            }
                        }
                    }

                    return false;
                }),
            triangles_i.end()
        );
    }
}

//=============================
// COLOR FUNCTIONS
//=============================

void Cube::SetColor(glm::vec3 color) {
    for (auto& [sp, triangles_i] : triangles) {
        for (auto& triangle : triangles_i) {
            triangle.SetColor(color);
        }
    }
}

glm::vec3 Cube::GetColor() {
    return triangles.begin()->second[0].GetColor();
}

//=============================
// POSITIONAL FUNCTIONS
//=============================

void Cube::UpdatePosition(glm::vec3 camera_position) {
    glm::vec3 position = Cube::GetStretchedPosition();

    // find distance from camera to cube
    float distance = glm::length(position - camera_position);

    // scale transformation based on model matrix scaling
    glm::vec3 stretched_scale = glm::vec3(
        glm::length(glm::vec3((*stretched_model_matrix)[0])),
        glm::length(glm::vec3((*stretched_model_matrix)[1])),
        glm::length(glm::vec3((*stretched_model_matrix)[2]))
    );
    stretched_scale *= 0.5f;

    glm::vec3 scale = glm::vec3(
        glm::length(glm::vec3((*model_matrix)[0])),
        glm::length(glm::vec3((*model_matrix)[1])),
        glm::length(glm::vec3((*model_matrix)[2]))
    );
    scale *= 0.5f;
    
    // rotate and scale the axes for obb
    glm::mat3 rotation_matrix = glm::mat3(*model_matrix);
    for (int i = 0; i < 3; i++) {
        axes[i] = rotation_matrix[i] * scale[i];
        stretched_axes[i] = rotation_matrix[i] * stretched_scale[i];
    }

    // scale the camera size
    camera_scaled_size = size * scale;
    stretched_camera_scaled_size = size * stretched_scale;

    // scale the stretched half size
    stretched_half_size = stretched_size * stretched_scale * 0.5f;

    // calculate corners of the cube
    corners = {
        position + rotation_matrix * glm::vec3(-stretched_camera_scaled_size.x, -stretched_camera_scaled_size.y, -stretched_camera_scaled_size.z), // min x, min y, min z
        position + rotation_matrix * glm::vec3( stretched_camera_scaled_size.x, -stretched_camera_scaled_size.y, -stretched_camera_scaled_size.z), // max x, min y, min z
        position + rotation_matrix * glm::vec3( stretched_camera_scaled_size.x,  stretched_camera_scaled_size.y, -stretched_camera_scaled_size.z), // max x, max y, min z
        position + rotation_matrix * glm::vec3(-stretched_camera_scaled_size.x,  stretched_camera_scaled_size.y, -stretched_camera_scaled_size.z), // min x, max y, min z
        position + rotation_matrix * glm::vec3(-stretched_camera_scaled_size.x, -stretched_camera_scaled_size.y,  stretched_camera_scaled_size.z), // min x, min y, max z
        position + rotation_matrix * glm::vec3( stretched_camera_scaled_size.x, -stretched_camera_scaled_size.y,  stretched_camera_scaled_size.z), // max x, min y, max z
        position + rotation_matrix * glm::vec3( stretched_camera_scaled_size.x,  stretched_camera_scaled_size.y,  stretched_camera_scaled_size.z), // max x, max y, max z
        position + rotation_matrix * glm::vec3(-stretched_camera_scaled_size.x,  stretched_camera_scaled_size.y,  stretched_camera_scaled_size.z)  // min x, max y, max z
    };

    // set the line vertices based on corners
    lines_vertices = {
        corners[0], corners[1], // min x, min y, min z -> max x, min y, min z
        corners[1], corners[2], // max x, min y, min z -> max x, max y, min z
        corners[2], corners[3], // max x, max y, min z -> min x, max y, min z
        corners[3], corners[0], // min x, max y, min z -> min x, min y, min z
        
        corners[4], corners[5], // min x, min y, max z -> max x, min y, max z
        corners[5], corners[6], // max x, min y, max z -> max x, max y, max z
        corners[6], corners[7], // max x, max y, max z -> min x, max y, max z
        corners[7], corners[4], // min x, max y, max z -> min x, min y, max z
        
        corners[0], corners[4], // min x, min y, min z -> min x, min y, max z
        corners[1], corners[5], // max x, min y, min z -> max x, min y, max z
        corners[2], corners[6], // max x, max y, min z -> max x, max y, max z
        corners[3], corners[7]  // min x, max y, min z -> min x, max y, max z
    };
}


glm::vec3 Cube::GetPosition() {
    return glm::vec3((*model_matrix)[3][0], (*model_matrix)[3][1], (*model_matrix)[3][2]);
}

glm::vec3 Cube::GetStretchedPosition() {
    return glm::vec3((*stretched_model_matrix)[3][0], (*stretched_model_matrix)[3][1], (*stretched_model_matrix)[3][2]);
}

void Cube::SetPosition(glm::vec3 p_position) {
    (*model_matrix)[3][0] = p_position[0];
    (*model_matrix)[3][1] = p_position[1];
    (*model_matrix)[3][2] = p_position[2];

    Cube::UpdateStretch();
}

bool Cube::IsVisible(glm::mat4 view_projection_matrix) {
    for (auto& corner : corners) {
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
    glm::vec3 translated_position = Cube::GetPosition() + translation;
    Cube::SetPosition(translated_position);
}

void Cube::ResetTranslation() {
    *model_matrix = *initial_model_matrix;
    *stretched_model_matrix = *initial_model_matrix;
}

void Cube::Rotate(glm::vec3 rotation) {
    // Create a rotation matrix (starting from identity)
    glm::mat4 rotation_matrix = glm::mat4(1.0f);

    // Apply rotations around each axis (in degrees, converted to radians)
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0)); // Rotate around X-axis
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0)); // Rotate around Y-axis
    rotation_matrix = glm::rotate(rotation_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1)); // Rotate around Z-axis

    // Accumulate the rotation by multiplying the current model_matrix by the new rotation_matrix
    *model_matrix *= rotation_matrix;
    
    Cube::_CalculateUp();
    Cube::UpdateStretch();
}

void Cube::UpdateStretch() {
    *stretched_model_matrix = *model_matrix;

    // create a scaling matrix that scales each axis according to 'up' and the aspect ratio
    glm::mat4 stretch_matrix = glm::mat4(1.0f);

    // scale each axis based on the 'up' vector and the aspect ratio
    stretch_matrix[0][0] = 1.0f + up[0] * (aspect_ratio - 1.0f);  // Scale factor for X
    stretch_matrix[1][1] = 1.0f + up[1] * (aspect_ratio - 1.0f);  // Scale factor for Y
    stretch_matrix[2][2] = 1.0f + up[2] * (aspect_ratio - 1.0f);  // Scale factor for Z

    // apply the scaling transformation
    *stretched_model_matrix *= stretch_matrix;

    (*stretched_model_matrix)[3] *= glm::vec4(stretch_matrix[0][0], stretch_matrix[1][1], stretch_matrix[2][2], 1.0f);

    *normal_matrix = glm::transpose(glm::inverse(glm::mat3(*stretched_model_matrix)));

    stretched_size = size * glm::vec3(stretch_matrix[0][0], stretch_matrix[1][1], stretch_matrix[2][2]);

    Cube::UpdatePosition(glm::vec3(0.0f));
}

glm::vec3 Cube::Stretch(glm::vec3 p_vector) {
    float stretch_factor = 1.0f + (aspect_ratio - 1.0f) * glm::length(up);

    glm::vec3 stretch_axis = glm::normalize(up);
    glm::mat3 identity = glm::mat3(1.0f);

    glm::mat3 stretch_matrix = identity + (stretch_factor - 1.0f) * glm::outerProduct(stretch_axis, stretch_axis);

    return stretch_matrix * p_vector;
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
    glm::vec3 ray_to_pos = Cube::GetStretchedPosition() - ray.origin;

    // for each axes
    for (int i = 0; i < 3; i++) {
        // the difference in angle between the ray direction and axes direction
        float axis_projection = glm::dot(ray.direction, stretched_axes[i]);
        // the distance between this cubes center and the ray direction
        float distance = glm::dot(ray_to_pos, stretched_axes[i]);

        // if the float abs of axis_projection (difference in angle) is near 0 or greater, continue
        if (std::fabs(axis_projection) > 1e-6) {
            // calculate where the ray enters and exits the along this axis
            float t1 = (distance - stretched_half_size[i]) / axis_projection;
            float t2 = (distance + stretched_half_size[i]) / axis_projection;

            if (t1 > t2) std::swap(t1, t2);

            tmin = glm::max(tmin, t1);
            tmax = glm::min(tmax, t2);
            
            // if tmin > tmax, there is no overlap between the camera ray and cube
            if (tmin > tmax) return false;

        // if the distance is too far from the half_size, there is no overlap
        } else if (-distance > stretched_half_size[i] || distance > stretched_half_size[i]) {
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
    glm::vec3 ray_to_pos = Cube::GetStretchedPosition() - ray.origin;

    face_hit_normal_out = glm::vec3(-1000.0f);

    // for each axes
    for (int i = 0; i < 3; i++) {
        // the difference in angle between the ray direction and axes direction
        float axis_projection = glm::dot(ray.direction, stretched_axes[i]);
        // the distance between this cube center and the ray direction
        float distance = glm::dot(ray_to_pos, stretched_axes[i]);

        // if the float abs of axis_projection (difference in angle) is near 0 or greater, continue
        if (std::fabs(axis_projection) > 1e-6) {
            // calculate where the ray enters and exits the along this axis
            float t1 = (distance - stretched_half_size[i]) / axis_projection;
            float t2 = (distance + stretched_half_size[i]) / axis_projection;

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
        } else if (-distance > stretched_half_size[i] || distance > stretched_half_size[i]) {
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
    
    glm::vec3 position_out = glm::vec3(0.0f);
    std::vector<GLfloat> temp_position_out = {};
    
    // read through the entire cube data string and split it into triangle data lines
    std::string temp_number_str = "";
    for (const auto& letter : data_str) {
        if (letter == '\n') {
            temp_vertices_str.push_back(temp_number_str);
            temp_number_str = "";
            continue;
        }
        temp_number_str += letter;
    }

    // read through first line and set position out
    std::string temp_number_str_pos = "";
    for (const auto& letter : temp_vertices_str[0]) {
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
    for (const auto& line : temp_vertices_str) {
        if (!first_line) {
            temp_vertices = {};
            temp_number_str_vertices = "";

            for (const auto& letter : line) {
                if (letter == ' ') {
                    GLfloat value = std::stof(temp_number_str_vertices);
                    temp_vertices.push_back(value);
                    
                    temp_number_str_vertices = "";
                    continue;
                }
                temp_number_str_vertices += letter;
            }

            GLfloat value = std::stof(temp_number_str_vertices);
            temp_vertices.push_back(value);
            
            temp_triangles.push_back(Triangle(temp_vertices));
        }
        first_line = false;
    }

    
    p_position_out = position_out;
    return temp_triangles;
}

//=============================
// TRANSLATION FUNCTIONS
//=============================

void Cube::_CalculateUp() {
    glm::vec3 rotated_up = glm::normalize(glm::vec3(*model_matrix * glm::vec4(true_up, 0.0f)));
    up = glm::abs(rotated_up);
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
    for (auto& [shader_program, triangles_i] : triangles) {
        for (auto& triangle : triangles_i) {
            triangle.FreeAll();
        }
    } 
    // free lines
    glDeleteVertexArrays(1, &lines_vertex_array);
    glDeleteBuffers(1, &lines_vertex_buffer);
    lines_vertex_array = 0;
    lines_vertex_buffer = 0;
}
