#ifndef SRC_TRIANGLE_H_
#define SRC_TRIANGLE_H_

#pragma once

#include <iostream>
#include <vector>

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"

/*
ABOUT:
A basic colored triangle. Contains vertices, vertex array and vertex buffer.
Typical lifecycle is construct, LoadVertices, Build then Render. 
*/

class Triangle {
    public:
        Triangle(std::vector<GLfloat> vertices);
        // do NOT try using ~ deconstructor. i spent 4 hours debugging this. 
        void FreeAll();

        //////// BASIC FUNCTIONS
        // verifys vertex_array and vertex_buffer is valid (non-zero)
        bool Verify();
        void LoadVertices(std::vector<GLfloat> vertices);
        void Build();
        void Render();

        std::string GetData(float aspect_ratio);

    private:
        TF_TRIANGLE_VERTICES vertices;
        GLuint vertex_array = 0;
        GLuint vertex_buffer = 0;

};

#endif // SRC_TRIANGLE_H_