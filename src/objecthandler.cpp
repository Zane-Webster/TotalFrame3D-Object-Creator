#include "ObjectHandler.h"
//=============================
// DEFAULT CONSTRUCTOR
//=============================

ObjectHandler::ObjectHandler() {
    ;
}

//=============================
// CREATION
//=============================

void ObjectHandler::Create(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, std::string obj_path, GLuint shader_program) {
    objects.push_back(Object(name, position, type, obj_path, shader_program));
    shader_program_groups[objects.back().shader_program].push_back(objects.back());
    shader_programs_need_update[objects.back().shader_program] = true;
}

void ObjectHandler::Add(Object object) {
    objects.push_back(object);
    shader_program_groups[object.shader_program].push_back(object);
    shader_programs_need_update[object.shader_program] = true;
}

//=============================
// SHADER PROGRAMS
//=============================

void ObjectHandler::UpdateSP(Object object, glm::mat4 view_projection_matrix) {
    // if visible, update map
    if (object.IsVisible(view_projection_matrix)) shader_programs_need_update[object.shader_program] = true;
}

void ObjectHandler::UpdateAllSP(glm::mat4 view_projection_matrix) {
    for (auto object : objects) {
        ObjectHandler::UpdateSP(object, view_projection_matrix);
    }
}

std::vector<GLuint> ObjectHandler::GetShaderProgramsUpdates(glm::mat4 view_projection_matrix) {
    UpdateAllSP(view_projection_matrix);

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
// RENDERING
//=============================

void ObjectHandler::Render(Object object, glm::mat4 view_projection_matrix) {
    // if visible, render
    //if (object.IsVisible(view_projection_matrix)) object.Render();
    object.Render();
}

void ObjectHandler::RenderAll(glm::mat4 view_projection_matrix) {
    for (auto object : objects) {
        ObjectHandler::Render(object, view_projection_matrix);
    }
}

//=============================
// MEMORY MANAGEMENT
//=============================

void ObjectHandler::FreeAll() {
    for (auto object : objects) {
        object.FreeAll();
    }
}