#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include <filesystem>
#include <fstream>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Triangle.h"

class Object {
    public:
        Object(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, std::string obj_path, GLuint shader_program);

        //// BASIC ATTRIBUTES
        std::string name = "";
        TotalFrame::OBJECT_TYPE type = TotalFrame::OBJECT_TYPE::BASIC_OBJ;
        GLuint shader_program = 0;

        //// TRANSFORMATION
        glm::vec3 size = glm::vec3(0.1f);
        glm::vec3 position = glm::vec3(0.0f);
        glm::mat4 model_matrix = glm::mat4(1.0f); 

        //// BASIC FUNCTIONS
        void Verify();
        void Load(std::string obj_path);
        void Render();

        //// POSITIONAL FUNCTIONS
        glm::vec3 GetPosition();
        bool IsVisible(glm::mat4 view_projection_matrix);

        //// MEMORY MANAGEMENT
        void FreeAll();
    
    private:
        //// BASIC ATTRIBUTES
        std::unordered_map<GLuint, std::vector<Triangle>> triangles = {};

        //// BASIC FUNCTIONS
        std::vector<Triangle> _Read(std::string obj_path);
    
};

#endif // SRC_OBJECT_H_