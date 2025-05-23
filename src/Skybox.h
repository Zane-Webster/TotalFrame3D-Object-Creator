#ifndef SRC_SKYBOX_H_
#define SRC_SKYBOX_H_

#pragma once

#include <iostream>
#include <vector>

#include <filesystem>
#include <fstream>

//// SDL LIBRARIES
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>

//// GLEW
#include <GL/glew.h>

//// GLM
#include <glm/glm.hpp>
#include <glm/ext.hpp>

//// TOTALFRAME LIBRARIES
#include "TotalFrame.h"
#include "Texture.h"

/*
ABOUT:
Basic skybox function.
IMPORTANT: Ensure you use FreeAll() before exiting program 
*/

class Skybox {
    public:
        Skybox(std::string skybox_folder_path, GLuint shader_program);

        //////// BASIC FUNCTIONS
        void Build();
        void Render(const glm::mat4& view, const glm::mat4& projection);

        //////// MEMORY MANAGEMENT
        void FreeAll();

    private:
        //////// BASIC ATTRIBUTES
        GLuint cubemap_texture = 0;
        GLuint shader_program = 0;

        std::vector<GLfloat> vertices = {};

        GLuint vertex_array = 0;
        GLuint vertex_buffer = 0;

        TF_SKYBOX_PATHS faces_paths = {};

        //////// BASIC FUNCTIONS
        void _LoadPaths(std::string skybox_folder_path);
        void _LoadCubeMap();
        void _CreateVertices();

};

#endif // SRC_SKYBOX_H_