#ifndef SRC_OBJECTHANDLER_H_
#define SRC_OBJECTHANDLER_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Object.h"

class ObjectHandler {
    public:
        ObjectHandler();

        //// BASIC ATTRIBUTES
        std::vector<Object> objects;
        // groups objects by which shader program they use
        std::unordered_map<GLuint, std::vector<Object>> shader_program_groups = {};
        // says which shader_programs need to be updated
        std::unordered_map<GLuint, bool> shader_programs_need_update = {};

        //// OBJECT CREATION
        // creates an object
        void Create(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, std::string obj_path, GLuint shader_program);
        // adds a pre-created object
        void Add(Object object);

        //// SHADER PROGRAMS
        // updates a shader program if it needs updated
        void UpdateSP(Object object, glm::mat4 view_projection_matrix);
        // updates all shader programs if they need updated
        void UpdateAllSP(glm::mat4 view_projection_matrix);
        // returns all shader programs that need updated
        std::vector<GLuint> GetShaderProgramsUpdates(glm::mat4 view_projection_matrix);

        //// RENDERING
        // renders an object if it is in view
        void Render(Object object, glm::mat4 view_projection_matrix);
        // renders all objects that are in view
        void RenderAll(glm::mat4 view_projection_matrix);

        //// MEMORY MANAGEMENT
        void FreeAll();

    private:

};

#endif // SRC_OBJECTHANDLER_H_