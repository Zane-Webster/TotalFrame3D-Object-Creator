#ifndef SRC_TEXTURE_H_
#define SRC_TEXTURE_H_

#pragma once

//// GENERAL LIBRARIES
#include <string>

//// SDL
#include <SDL3/SDL.h>

//// GLEW
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"

class Texture {
    public:
        Texture(std::string path);
        ~Texture();

        GLuint id;

        int width = 0;
        int height = 0;
        int channels = 0;

        void Build(std::string path);

        void Bind(int unit = 0);

    private:

};

#endif // SRC_TEXTURE_H_