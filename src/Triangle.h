#ifndef SRC_TRIANGLE_H_
#define SRC_TRIANGLE_H_

#pragma once

#include <iostream>
#include <memory>
#include <array>
#include <vector>

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"

/*
Triangle 
*/

class Triangle {
    public:
        Triangle(std::vector<GLfloat> vertices);

        //// BASIC FUNCTIONS
        // verifys vertex_array and vertex_buffer is valid (non-zero)
        bool Verify();
        void LoadVertices(std::vector<GLfloat> vertices);
        void Build();
        void Render();

        //// MEMORY MANAGEMENT
        void FreeAll();

    private:
        std::array<GLfloat, 18> vertices;
        GLuint vertex_array = 0;
        GLuint vertex_buffer = 0;

};

#endif // SRC_TRIANGLE_H_