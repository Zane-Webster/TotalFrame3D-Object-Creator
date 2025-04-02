#ifndef SRC_SHADERHANDLER_H_
#define SRC_SHADERHANDLER_H_

#pragma once

#include <iostream>

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include <filesystem>
#include <fstream>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include "TotalFrame.h"
#include "Util.h"

class ShaderHandler {
    public:
        ShaderHandler(SDL_GLContext context);

        //// BASIC SHADER FUNCTIONS
        // creates and returns shader program based on 
        GLuint CreateShaderProgram(std::string dir_path);

        //// MEMORY MANAGEMENT
        void FreeAll();

    private:
        //// BASICS
        SDL_GLContext context;
        std::vector<std::string> shader_exts = {".vert",".frag",".geom"};
        std::vector<GLenum> shader_types = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER};

        //// MEMORY MANAGEMENT
        std::vector<GLuint> shader_programs = {};

        //// SHADER SOURCE MAPS
        // vertex shaders
        std::unordered_map<GLenum, std::string> vert_shaders_sources = {};
        // fragment shaders
        std::unordered_map<GLenum, std::string> frag_shaders_sources = {};
        // fragment shaders
        std::unordered_map<GLenum, std::string> geom_shaders_sources = {};
        // all shaders source
        std::vector<std::shared_ptr<std::unordered_map<GLenum, std::string>>> shaders_sources = {};

        //// BASIC SHADER FUNCTIONS
        // reads shaders source folder and adds contents to proper source maps
        void _ReadShaderSourceFolder(std::string dir_path);

        // compiles shader to be used in shader program
        GLuint _CompileShader(GLenum type, std::string source);
        void _ClearShaderSources();
};


#endif // SRC_SHADERHANDLER_H_