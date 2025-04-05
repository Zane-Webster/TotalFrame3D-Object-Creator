#include "Object.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Object::Object(std::string p_name, glm::vec3 p_position, TotalFrame::OBJECT_TYPE p_type, float p_size, std::string p_obj_path, GLuint p_shader_program, float p_aspect_ratio, std::string object_data_str) : name(p_name), position(p_position), type(p_type), size(p_size), shader_program(p_shader_program), aspect_ratio(p_aspect_ratio) {
    Object::Load(p_obj_path, object_data_str);

    // adjust for aspect_ratio (vertices are updated in Object::_Read())
    size.y *= aspect_ratio;

    Object::UpdatePosition(glm::vec3(0.0f));
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

void Object::Load(std::string obj_path, std::string object_data_str) {
    if (object_data_str == "") triangles[shader_program] = Object::_Read(obj_path);
    else triangles[shader_program] = Object::_CreateFromStr(object_data_str);
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

std::string Object::GetData() {
    std::string temp_data = "";
    for (auto [sp, triangles_i] : triangles) {
        for (auto triangle : triangles_i) {
            temp_data += triangle.GetData(aspect_ratio);
            temp_data += '\n';
        }
    }
    return temp_data;
}

//=============================
// POSITIONAL FUNCTIONS
//=============================

void Object::UpdatePosition(glm::vec3 camera_position) {
    // find distance from camera to object
    float distance = glm::length(position - camera_position);

    // scale factor based on distance from object. clamped to prevent extremes
    float scale_factor = glm::clamp(distance * 0.1f, 0.5f, 5.0f);

    // scale transformation based on model matrix scaling
    glm::vec3 scale_transformation = glm::vec3(glm::length(glm::vec3(model_matrix[0])), 
                                    glm::length(glm::vec3(model_matrix[1])), 
                                    glm::length(glm::vec3(model_matrix[2])));

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

    // calculate corners of the object
    corners = {
        position + glm::vec3(-half_size.x, -half_size.y, -half_size.z), // min x, min y, min z
        position + glm::vec3( half_size.x, -half_size.y, -half_size.z), // max x, min y, min z
        position + glm::vec3( half_size.x,  half_size.y, -half_size.z), // max x, max y, min z
        position + glm::vec3(-half_size.x,  half_size.y, -half_size.z), // min x, max y, min z
        position + glm::vec3(-half_size.x, -half_size.y,  half_size.z), // min x, min y, max z
        position + glm::vec3( half_size.x, -half_size.y,  half_size.z), // max x, min y, max z
        position + glm::vec3( half_size.x,  half_size.y,  half_size.z), // max x, max y, max z
        position + glm::vec3(-half_size.x,  half_size.y,  half_size.z)  // min x, max y, max z
    };

}


glm::vec3 Object::GetPosition() {
    return glm::vec3(model_matrix[3][0], model_matrix[3][1], model_matrix[3][2]);
}

bool Object::IsVisible(glm::mat4 view_projection_matrix) {
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
// RAY FUNCTIONS
//=============================

bool Object::RayCollides(TotalFrame::Ray ray) {
    float dummy = 0.0f;
    return Object::RayCollides(ray, dummy);
}

bool Object::RayCollides(TotalFrame::Ray ray, float& tmin_out) {
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::max();

    // ray direction from ray origin to object position (center)
    glm::vec3 ray_to_pos = position - ray.origin;

    // for each axes
    for (int i = 0; i < 3; i++) {
        // the difference in angle between the ray direction and axes direction
        float axis_projection = glm::dot(ray.direction, axes[i]);
        // the distance between this objects center and the ray direction
        float distance = glm::dot(ray_to_pos, axes[i]);

        // if the float abs of axis_projection (difference in angle) is near 0 or greater, continue
        if (std::fabs(axis_projection) > 1e-6) {
            // calculate where the ray enters and exits the along this axis
            float t1 = (distance - half_size[i]) / axis_projection;
            float t2 = (distance + half_size[i]) / axis_projection;

            if (t1 > t2) std::swap(t1, t2);

            tmin = glm::max(tmin, t1);
            tmax = glm::min(tmax, t2);
            
            // if tmin > tmax, there is no overlap between the camera ray and object
            if (tmin > tmax) return false;

        // if the distance is too far from the half_size, there is no overlap
        } else if (-distance > half_size[i] || distance > half_size[i]) {
            return false;
        }
    }

    // tmin output for determining the first object hit
    tmin_out = tmin;

    // they do overlap
    return true;
}

bool Object::RayCollidesWithFace(TotalFrame::Ray ray, float& tmin_out, glm::vec3& face_hit_normal_out) {
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::max();

    // ray direction from ray origin to object position (center)
    glm::vec3 ray_to_pos = position - ray.origin;

    // for each axes
    for (int i = 0; i < 3; i++) {
        // the difference in angle between the ray direction and axes direction
        float axis_projection = glm::dot(ray.direction, axes[i]);
        // the distance between this objects center and the ray direction
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
            
            // if tmin > tmax, there is no overlap between the camera ray and object
            if (tmin > tmax) return false;

        // if the distance is too far from the half_size, there is no overlap
        } else if (-distance > half_size[i] || distance > half_size[i]) {
            return false;
        }
    }

    // tmin output for determining the first object hit
    tmin_out = tmin;

    // they do overlap
    return true;
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

    //// read file, add to string then pass through Object::_CreateFromStr
    std::ifstream obj_file(obj_path);
    std::string object_data((std::istreambuf_iterator<char>(obj_file)), std::istreambuf_iterator<char>());
    
    return Object::_CreateFromStr(object_data);
}

std::vector<Triangle> Object::_CreateFromStr(std::string object_data_str) {
    // define temporary variables
    std::vector<Triangle> temp_triangles = {};
    std::vector<std::string> temp_vertices_str = {};
    std::vector<GLfloat> temp_vertices = {};
    std::string temp_number_str = "";

    // read through the entire object data string and split it into triangle data lines
    for (auto letter : object_data_str) {
        if (letter == '\n') {
            temp_vertices_str.push_back(temp_number_str);
            temp_number_str = "";
            continue;
        }
        temp_number_str += letter;
    }

    // read through each set of vertices (triangle data), add to temp_vertices, create a triangle from the temp_vertices, and build the triangle
    for (auto line : temp_vertices_str) {
        temp_vertices = {};
        temp_number_str = {};

        for (auto letter : line) {
            if (letter == ' ') {
                temp_vertices.push_back(std::stof(temp_number_str));

                // if the number is in that position, that means it is a y value. adjust the y value by aspect ratio
                if (temp_vertices.size() == 2 || temp_vertices.size() == 8 || temp_vertices.size() == 14) temp_vertices.back() *= aspect_ratio;
                
                temp_number_str = "";
                continue;
            }
            temp_number_str += letter;
        }

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
