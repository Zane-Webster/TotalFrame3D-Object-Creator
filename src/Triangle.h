#ifndef SRC_TRIANGLE_H_
#define SRC_TRIANGLE_H_

#pragma once

#include <iostream>
#include <vector>
#include <memory>

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

/*
ABOUT:
A basic colored triangle. Contains vertices, vertex array and vertex buffer.
Typical lifecycle is construct, LoadVertices, Build then Render. 
*/

class Triangle {
    public:
        Triangle(std::vector<GLfloat> vertices, std::vector<GLfloat> true_vertices);
        // do NOT try using ~ deconstructor. i spent 4 hours debugging this. 
        void FreeAll();

        //////// BASIC FUNCTIONS
        // verifys vertex_array and vertex_buffer is valid (non-zero)
        bool Verify();
        void LoadVertices(std::vector<GLfloat> vertices, std::vector<GLfloat> true_vertices);
        void Build();
        void Render();
        void RenderOutline();
        std::string GetData();
        std::string GetTrueData();
        
        //////// MOVEMENT FUNCTIONS
        void SetPosition(glm::vec3 position, float aspect_ratio);
        void ResetTruePosition();

        //////// COLOR FUNCTIONS
        void SetColor(glm::vec3 color);

    private:
        std::shared_ptr<TF_TRIANGLE_VERTICES> vertices = std::make_shared<TF_TRIANGLE_VERTICES>();
        std::shared_ptr<TF_TRIANGLE_VERTICES> true_vertices = std::make_shared<TF_TRIANGLE_VERTICES>();
        std::shared_ptr<TF_TRIANGLE_VERTICES> translated_true_vertices = std::make_shared<TF_TRIANGLE_VERTICES>();
        GLuint vertex_array = 0;
        GLuint vertex_buffer = 0;
};

#endif // SRC_TRIANGLE_H_