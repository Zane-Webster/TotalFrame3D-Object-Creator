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
        Triangle(std::vector<GLfloat> vertices);
        void FreeAll();

        //////// BASIC ATTRIBUTES
        std::shared_ptr<TF_TRIANGLE_VERTICES> vertices = std::make_shared<TF_TRIANGLE_VERTICES>();

        //////// BASIC FUNCTIONS
        // verifys vertex_array and vertex_buffer is valid (non-zero)
        bool Verify();
        void LoadVertices(std::vector<GLfloat> vertices);
        void Build();
        void Render();
        void RenderOutline();
        std::string GetData();
        
        //////// EXPORTATION FUNCTIONS
        void Translate(glm::vec3 translation);
        std::vector<glm::vec3> GetPositions();

        //////// NORMAL FUNCTIONS
        glm::vec3 GetNormal();
        void UpdateNormal();
        void UpdateFullVertices();

        //////// COLOR FUNCTIONS
        void SetColor(glm::vec3 color);
        glm::vec3 GetColor();

    private:
        std::shared_ptr<TF_TRIANGLE_VERTICES_WITH_NORMAL> full_vertices = std::make_shared<TF_TRIANGLE_VERTICES_WITH_NORMAL>();

        glm::vec3 normal = glm::vec3(0.0f);
        
        GLuint vertex_array = 0;
        GLuint vertex_buffer = 0;
};

#endif // SRC_TRIANGLE_H_