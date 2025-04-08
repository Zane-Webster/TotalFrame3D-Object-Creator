#include "ObjectHandler.h"
//=============================
// DEFAULT CONSTRUCTOR
//=============================

ObjectHandler::ObjectHandler(float p_aspect_ratio) : aspect_ratio(p_aspect_ratio) {
    ;
}

//=============================
// BASIC FUNCTIONS
//=============================

void ObjectHandler::UpdateAndRenderAll(glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position) {
    for (auto object : objects) {
        ObjectHandler::UpdateAndRender(object, camera_view_projection_matrix, camera_position);
    }
}

void ObjectHandler::UpdateAndRender(Object object, glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position) {
    if (object.IsVisible(camera_view_projection_matrix)) {
        ObjectHandler::UpdateSP(object, true);
        ObjectHandler::UpdateObjectCameraScale(object, camera_position, true);
        ObjectHandler::Render(object, true);
    }
}

std::string ObjectHandler::GetData() {
    std::string temp_data = "";
    for (auto object : objects) {
        temp_data += object.GetData();
    }
    return temp_data;
}

std::string ObjectHandler::GetTrueData() {
    std::string temp_data = "";
    for (auto object : objects) {
        temp_data += object.GetTrueData();
    }
    return temp_data;
}

//=============================
// CREATION FUNCTIONS
//=============================

void ObjectHandler::Create(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program, std::string object_data_str) {
    Object temp_object(name, position, type, size, obj_path, shader_program, aspect_ratio, object_data_str);
    objects.push_back(temp_object);
    shader_program_groups[objects.back().shader_program].push_back(objects.back());
    shader_programs_need_update[objects.back().shader_program] = true;
}

void ObjectHandler::Add(Object object) {
    objects.push_back(object);
    shader_program_groups[object.shader_program].push_back(object);
    shader_programs_need_update[object.shader_program] = true;
}

void ObjectHandler::ClearAndCreate(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program) {
    objects.clear();

    std::vector<std::string> objects_data = ObjectHandler::_SplitByObject(ObjectHandler::_ReadData(obj_path));

    for (auto object_data : objects_data) {
        ObjectHandler::Create(name, position, type, size, obj_path, shader_program, object_data);
    }
}

//=============================
// SHADER PROGRAM FUNCTIONS
//=============================

void ObjectHandler::UpdateSP(Object object, bool is_visible) {
    // update map
    if (is_visible) shader_programs_need_update[object.shader_program] = true;
}

std::vector<GLuint> ObjectHandler::GetShaderProgramsUpdates() {
    std::vector<GLuint> temp_shader_programs = {};
    for (auto [shader_program, need_update] : shader_programs_need_update) {
        if (need_update) {
            temp_shader_programs.push_back(shader_program);
            need_update = false;
        }
    }
    return temp_shader_programs;
}

//=============================
// CAMEAR SCALING FUNCTIONS
//=============================

void ObjectHandler::UpdateObjectCameraScale(Object object, glm::vec3 camera_position, bool is_visible) {
    object.UpdatePosition(camera_position);
}

//=============================
// RAY FUNCTIONS
//=============================

std::shared_ptr<Object> ObjectHandler::GetRayCollidingObject(TotalFrame::Ray ray) {
    // set closest_object to the farthest possible
    float closest_distance = std::numeric_limits<float>::max();

    std::shared_ptr<Object> closest_object = nullptr;

    for (auto object : objects) {
        float distance;
        // if the object collides with the ray
        if (object.RayCollides(ray, distance)) {
            // if the object is closer than the others
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_object = std::make_shared<Object>(object);
            }
        }
    }

    return closest_object;
}

Object ObjectHandler::GetRayCollidingObjectWithFace(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out) {
    // set closest_object to the farthest possible
    float closest_distance = std::numeric_limits<float>::max();

    Object closest_object;

    glm::vec3 closest_face_hit_normal = glm::vec3(-1000.0f);

    for (auto object : objects) {
        float distance;
        glm::vec3 face_hit_normal;
        // if the object collides with the ray
        if (object.RayCollidesWithFace(ray, distance, face_hit_normal)) {
            // if the object is closer than the others
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_object = object;
                closest_face_hit_normal = face_hit_normal;
            }
        }
    }
    
    face_hit_normal_out = closest_face_hit_normal;

    return closest_object;
}

std::vector<std::shared_ptr<Object>> ObjectHandler::GetRayCollidingObjects(TotalFrame::Ray ray) {
    std::vector<std::shared_ptr<Object>> intersecting_objects = {};
    for (auto object : objects) {
        for (auto object : objects) {
            if (object.RayCollides(ray)) {
                intersecting_objects.push_back(std::make_shared<Object>(object));
            }
        }
    }
    return intersecting_objects;
}

//=============================
// RENDERING FUNCTIONS
//=============================

void ObjectHandler::Render(Object object, bool is_visible) {
    // if visible, render
    if (is_visible){
        object.Render();
    }
}

//=============================
// PRIVATE FUNCTIONS
//=============================

std::string ObjectHandler::_ReadData(std::string path) {
    // return and throw error if path doesn't exist
    if (!std::filesystem::exists(path)) {
        Util::ThrowError("INVALID OBJECT PATH", "ObjectHandler::_ReadData");
        return {};
    }

    //// read file, and add to string
    std::ifstream obj_file(path);
    std::string object_data((std::istreambuf_iterator<char>(obj_file)), std::istreambuf_iterator<char>());

    return object_data;
}

std::vector<std::string> ObjectHandler::_SplitByObject(std::string object_data) {
    std::vector<std::string> objects_data = {};
    std::string temp_object_data = "";
    std::istringstream stream(object_data);
    std::string line = "";
    bool first_line = true;

    while (std::getline(stream, line)) {
        int spaces = 0;
        for (auto letter : line) {
            if (letter == ' ') spaces++;
        }

        // if there are only 2 spaces, then that is a position vertice, and the next object can be started 
        if (spaces == 2 && !first_line) {
            objects_data.push_back(temp_object_data);
            temp_object_data.clear();
        }

        temp_object_data += line + '\n';

        first_line = false;
    }

    // push the last object
    objects_data.push_back(temp_object_data);

    return objects_data;
}

//=============================
// MEMORY MANAGEMENT
//=============================

void ObjectHandler::FreeAll() {
    for (auto object : objects) {
        object.FreeAll();
    }
}