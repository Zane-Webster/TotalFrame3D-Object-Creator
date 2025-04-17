#ifndef SRC_CUBE_H_
#define SRC_CUBE_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include <filesystem>
#include <fstream>

#include <string>
#include <sstream>

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
A collection of triangles with a shader program and positional/transformational attributes

NOTES:
Use Object to hold cubes.
*/

class Cube {
    public:
        Cube();
        Cube(std::string name, glm::vec3 position, float size, std::string path, GLuint shader_program, float aspect_ratio, std::string data_str = "");
        void FreeAll();

        //////// BASIC ATTRIBUTES
        std::string name = "";
        GLuint shader_program = 0;
        glm::vec3 size = glm::vec3(TotalFrame::TRIANGLE_SIZE);
        glm::vec3 stretched_size = glm::vec3(TotalFrame::TRIANGLE_SIZE);
        std::string path = "";

        //////// EXTERNAL ATTRIBUTES
        float aspect_ratio = 0.0f;

        //////// BASIC FUNCTIONS
        void Create(std::string name, glm::vec3 position, float size, std::string path, GLuint shader_program, float aspect_ratio, std::string data_str = "");
        void Load(std::string path, glm::vec3& position_out, std::string data_str = "");
        void Render(glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights);
        std::string GetData();
        void Verify();

        //////// EXPORTATION FUNCTIONS
        std::vector<std::array<TotalFrame::Ray, 14>> GetCornersRays();
        bool RayCollidesWithCorners(TotalFrame::Ray ray, glm::vec3 ignore_point);

        void RemoveTrianglesByCorners(std::vector<glm::vec3> removed_corners);

        //////// COLOR FUNCTIONS
        void SetColor(glm::vec3 color);
        glm::vec3 GetColor();

        //////// POSITIONAL FUNCTIONS
        void UpdatePosition(glm::vec3 camera_position);
        glm::vec3 GetPosition();
        glm::vec3 GetStretchedPosition();
        void SetPosition(glm::vec3 position);

        bool IsVisible(glm::mat4 view_projection_matrix);

        //////// TRANSLATION FUNCTIONS
        void Translate(glm::vec3 translation);
        void ResetTranslation();

        void Rotate(glm::vec3 rotation);

        void UpdateStretch();
        glm::vec3 Stretch(glm::vec3 vector);

        //////// RAY FUNCTIONS
        bool RayCollides(TotalFrame::Ray ray);
        bool RayCollides(TotalFrame::Ray ray, float& tmin_out);
        bool RayCollidesWithFace(TotalFrame::Ray ray, float& tmin_out, glm::vec3& face_hit_normal_out);
    
    private:
        //////// BASIC ATTRIBUTES
        std::unordered_map<GLuint, std::vector<Triangle>> triangles = {};
        std::vector<Triangle> export_triangles = {};

        //////// POSITION ATTRIBUTES
        std::vector<glm::vec3> corners = {};

        //////// LINE ATTRIBUTES
        std::vector<glm::vec3> lines_vertices = {};
        GLuint lines_vertex_array = 0;
        GLuint lines_vertex_buffer = 0;

        //////// TRANSFORMATION ATTRIBUTES
        std::shared_ptr<glm::mat4> stretched_model_matrix = std::make_shared<glm::mat4>(1.0f);
        std::shared_ptr<glm::mat4> model_matrix = std::make_shared<glm::mat4>(1.0f);
        std::shared_ptr<glm::mat4> initial_model_matrix = std::make_shared<glm::mat4>(1.0f);

        std::shared_ptr<glm::mat3> normal_matrix = std::make_shared<glm::mat3>(0.0f);

        glm::vec3 true_up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        //////// OBB ATTRIBUTES
        glm::vec3 stretched_half_size = glm::vec3(0.0f);

        glm::vec3 camera_scaled_size = glm::vec3(0.1f);
        glm::vec3 stretched_camera_scaled_size = glm::vec3(0.1f);

        glm::vec3 axes[3] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};
        glm::vec3 stretched_axes[3] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)};

        //////// BASIC FUNCTIONS
        std::vector<Triangle> _Read(std::string path, glm::vec3& position_out);
        std::vector<Triangle> _CreateFromStr(std::string data_str, glm::vec3& position_out);

        //////// TRANSLATION FUNCTIONS
        void _CalculateUp();

        //////// LINE FUNCTIONS
        void _BuildRenderLines();
        void _BuildLines();
        void _RenderLines();
};

#endif // SRC_CUBE_H_