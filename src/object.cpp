#include "Object.h"
//=============================
// DEFAULT CONSTRUCTOR
//=============================

Object::Object(TotalFrame::OBJECT_TYPE p_type, float p_aspect_ratio) : type(p_type), aspect_ratio(p_aspect_ratio) {
    ;
}

//=============================
// BASIC FUNCTIONS
//=============================

void Object::UpdateAndRenderAll(glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights) {
    for (auto cube : cubes) {
        Object::UpdateAndRender(cube, camera_view_projection_matrix, camera_position, lights);
    }
}

void Object::UpdateAndRender(Cube cube, glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights) {
    if (cube.IsVisible(camera_view_projection_matrix)) {
        Object::UpdateSP(cube, true);
        Object::UpdateCubeCameraScale(cube, camera_position, true);
        Object::Render(cube, camera_position, lights, true);
    }
}

std::string Object::GetData() {
    std::string temp_data = "";
    for (auto cube : cubes) {
        temp_data += cube.GetData();
    }
    return temp_data;
}

std::string Object::GetTrueData() {
    std::string temp_data = "";
    for (auto cube : cubes) {
        temp_data += cube.GetTrueData();
    }
    return temp_data;
}

//=============================
// EXPORTATION FUNCTIONS
//=============================
std::string Object::GetExportData() {
    std::string temp_data = "";
    for (auto cube : cubes) {
        temp_data += cube.GetExportData();
    }
    return temp_data;
}


//=============================
// CREATION FUNCTIONS
//=============================

void Object::Create(std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program, std::string object_data_str) {
    Cube temp_object(name, position, size, obj_path, shader_program, aspect_ratio, object_data_str);
    cubes.push_back(temp_object);
    shader_program_groups[cubes.back().shader_program].push_back(cubes.back());
    shader_programs_need_update[cubes.back().shader_program] = true;
}

void Object::CreateLight(std::shared_ptr<TotalFrame::Light> p_light, std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program, std::string object_data_str) {
    Object::Create(name, position, size, obj_path, shader_program, object_data_str);
    Object::AttachLight(p_light);
}

void Object::ClearAndCreate(std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program) {
    cubes.clear();

    std::vector<std::string> cubes_data = Object::_SplitByCube(Object::_ReadData(obj_path));

    for (auto object_data : cubes_data) {
        Object::Create(name, position, size, obj_path, shader_program, object_data);
    }
}

void Object::Add(Cube cube) {
    cubes.push_back(cube);
    shader_program_groups[cube.shader_program].push_back(cube);
    shader_programs_need_update[cube.shader_program] = true;
}

void Object::CreateShape(Shape shape) {
    //FIXME: using shape.cube.path means color isn't updated. using shape.GetData() means position isn't properly changed
    for (auto position : shape.positions) {
        Object::Create(shape.cube.name, position, shape.cube.size[0], shape.cube.path, shape.cube.shader_program);
    }
} 

//=============================
// DESTRUCTION FUNCTIONS
//=============================

void Object::Destory(Cube* p_cube) {
    for (int i = 0; i < cubes.size(); i++) {
        if (&cubes[i] == p_cube) {
            cubes[i].FreeAll();
            cubes.erase(cubes.begin() + i);
            break;
        }
    }
}

//=============================
// TRANSLATION FUNCTIONS
//=============================

void Object::Translate(glm::vec3 translation) {
    position += translation;
    for (auto cube : cubes) {
        cube.Translate(translation);
    }
}

void Object::Rotate(glm::vec3 rotation, glm::vec3 camera_position) {
    for (auto cube : cubes) {
        //cube.Rotate(rotation, camera_position);
    }
}

//=============================
// SHADER PROGRAM FUNCTIONS
//=============================

void Object::UpdateSP(Cube cube, bool is_visible) {
    // update map
    if (is_visible) shader_programs_need_update[cube.shader_program] = true;
}

std::vector<GLuint> Object::GetShaderProgramsUpdates() {
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

void Object::UpdateCubeCameraScale(Cube cube, glm::vec3 camera_position, bool is_visible) {
    cube.UpdatePosition(camera_position);
}

//=============================
// RAY FUNCTIONS
//=============================

std::shared_ptr<Cube> Object::GetRayCollidingCube(TotalFrame::Ray ray) {
    // set closest_cube to the farthest possible
    float closest_distance = std::numeric_limits<float>::max();

    std::shared_ptr<Cube> closest_cube = nullptr;

    for (auto cube : cubes) {
        float distance;
        // if the cube collides with the ray
        if (cube.RayCollides(ray, distance)) {
            // if the cube is closer than the others
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_cube = std::make_shared<Cube>(cube);
            }
        }
    }

    return closest_cube;
}

Cube Object::GetRayCollidingCubeWithFace(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out) {
    // set closest_cube to the farthest possible
    float closest_distance = std::numeric_limits<float>::max();

    Cube closest_cube;

    glm::vec3 closest_face_hit_normal = glm::vec3(-1000.0f);

    for (auto cube : cubes) {
        float distance;
        glm::vec3 face_hit_normal;
        // if the cube collides with the ray
        if (cube.RayCollidesWithFace(ray, distance, face_hit_normal)) {
            // if the cube is closer than the others
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_cube = cube;
                closest_face_hit_normal = face_hit_normal;
            }
        }
    }
    
    face_hit_normal_out = closest_face_hit_normal;

    return closest_cube;
}

Cube* Object::GetRayCollidingCubeWithFacePtr(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out) {
    // set closest_cube to the farthest possible
    float closest_distance = std::numeric_limits<float>::max();

    Cube* closest_cube = nullptr;

    glm::vec3 closest_face_hit_normal = glm::vec3(-1000.0f);

    for (auto& cube : cubes) {  // Iterate by reference
        float distance;
        glm::vec3 face_hit_normal;
        
        // If the cube collides with the ray
        if (cube.RayCollidesWithFace(ray, distance, face_hit_normal)) {
            // If the cube is closer than the others
            if (distance < closest_distance) {
                closest_distance = distance;
                closest_cube = &cube;  // Correctly get the address of the object
                closest_face_hit_normal = face_hit_normal;
            }
        }
    }
    
    face_hit_normal_out = closest_face_hit_normal;

    return closest_cube;
}

std::vector<std::shared_ptr<Cube>> Object::GetRayCollidingCubes(TotalFrame::Ray ray) {
    std::vector<std::shared_ptr<Cube>> intersecting_cubes = {};
    for (auto cube : cubes) {
        for (auto cube : cubes) {
            if (cube.RayCollides(ray)) {
                intersecting_cubes.push_back(std::make_shared<Cube>(cube));
            }
        }
    }
    return intersecting_cubes;
}

//=============================
// LIGHTING FUNCTIONS
//=============================

void Object::AttachLight(std::shared_ptr<TotalFrame::Light> p_light) {
    light = p_light;
    light->position = position;
}

//=============================
// RENDERING FUNCTIONS
//=============================

void Object::Render(Cube cube, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights, bool is_visible) {
    // if visible, render
    if (is_visible){
        cube.Render(camera_position, lights);
    }
}

//=============================
// PRIVATE FUNCTIONS
//=============================

std::string Object::_ReadData(std::string path) {
    // return and throw error if path doesn't exist
    if (!std::filesystem::exists(path)) {
        Util::ThrowError("INVALID OBJECT PATH", "Object::_ReadData");
        return {};
    }

    //// read file, and add to string
    std::ifstream obj_file(path);
    std::string object_data((std::istreambuf_iterator<char>(obj_file)), std::istreambuf_iterator<char>());

    return object_data;
}

std::vector<std::string> Object::_SplitByCube(std::string object_data) {
    std::vector<std::string> cubes_data = {};
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
            cubes_data.push_back(temp_object_data);
            temp_object_data.clear();
        }

        temp_object_data += line + '\n';

        first_line = false;
    }

    // push the last object
    cubes_data.push_back(temp_object_data);

    return cubes_data;
}

//=============================
// MEMORY MANAGEMENT
//=============================

void Object::FreeAll() {
    for (auto object : cubes) {
        object.FreeAll();
    }
}