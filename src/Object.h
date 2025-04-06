#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include <filesystem>
#include <fstream>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Triangle.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

/*
ABOUT:
A collection of triangles with a shader program and positional/transformational attributes. Currently only capable of displaying colored Triangles (texture support maybe coming TBA)

NOTES:
Use ObjectHandler to control objects for ease of use and safety.
*/

class Object {
    public:
        Object();
        Object(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program, float aspect_ratio, std::string object_data_str = "");
        void FreeAll();

        //////// BASIC ATTRIBUTES
        std::string name = "";
        TotalFrame::OBJECT_TYPE type = TotalFrame::OBJECT_TYPE::BASIC_OBJ;
        GLuint shader_program = 0;
        glm::vec3 size = glm::vec3(0.1f);
        glm::vec3 true_size = glm::vec3(0.1f);

        /////// EXTERNAL ATTRIBUTES
        float aspect_ratio = 0.0f;

        //////// BASIC FUNCTIONS
        void Verify();
        void Load(std::string obj_path, std::string object_data_str = "");
        void Render();
        void Create(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program, float aspect_ratio, std::string object_data_str = "");
        std::string GetData();

        //////// COLOR FUNCTIONS
        void SetColor(glm::vec3 color);

        //////// POSITIONAL FUNCTIONS
        void UpdatePosition(glm::vec3 camera_position);
        glm::vec3 GetPosition();
        glm::vec3 GetTTPosition();
        void SetPosition(glm::vec3 position);
        bool IsVisible(glm::mat4 view_projection_matrix);

        void Translate(glm::vec3 translation);
        void ResetTranslation();

        //////// RAY FUNCTIONS
        bool RayCollides(TotalFrame::Ray ray);
        bool RayCollides(TotalFrame::Ray ray, float& tmin_out);
        bool RayCollidesWithFace(TotalFrame::Ray ray, float& tmin_out, glm::vec3& face_hit_normal_out);
    
    private:
        //////// BASIC ATTRIBUTES
        std::unordered_map<GLuint, std::vector<Triangle>> triangles = {};

        //////// POSITION ATTRIBUTES
        // position to render at during runtime
        glm::vec3 position = glm::vec3(0.0f);
        // the true position, not translated
        glm::vec3 true_position = glm::vec3(0.0f);
        // the true position, translated, but not stretched by aspect ratio
        glm::vec3 translated_true_position = glm::vec3(0.0f);
        std::vector<glm::vec3> corners = {};

        //////// TRANSFORMATION ATTRIBUTES
        glm::mat4 model_matrix = glm::mat4(1.0f);
        glm::mat4 true_model_matrix = glm::mat4(1.0f);
        glm::mat4 translated_true_model_matrix = glm::mat4(1.0f);

        //////// OBB ATTRIBUTES
        glm::vec3 half_size = glm::vec3(0.0f);
        glm::vec3 camera_scaled_size = glm::vec3(0.1f);
        glm::vec3 axes[3] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};

        //////// BASIC FUNCTIONS
        std::vector<Triangle> _Read(std::string obj_path);
        std::vector<Triangle> _CreateFromStr(std::string object_data_str);
};

#endif // SRC_OBJECT_H_