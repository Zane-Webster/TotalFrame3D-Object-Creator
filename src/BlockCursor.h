#ifndef SRC_BLOCKCURSOR_H_
#define SRC_BLOCKCURSOR_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Cube.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

class BlockCursor {
    public:
        BlockCursor(std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program, float aspect_ratio, std::string object_data_str = "");

        Cube cube;
        bool visible = false;

        glm::vec3 current_translation = glm::vec3(0.0f);

        void PlaceOnFace(glm::vec3 cube_position, glm::vec3 face_pos);

        glm::vec3 NextCubePosition();

    private:

};

#endif // SRC_BLOCKCURSOR_H_