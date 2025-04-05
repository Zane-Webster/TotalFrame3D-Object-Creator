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
#include "Object.h"

class BlockCursor {
    public:
        BlockCursor(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program, float aspect_ratio, std::string object_data_str = "");

        Object object;
        bool visible = false;

        glm::vec3 current_translation = glm::vec3(0.0f);

        void PlaceOnFace(glm::vec3 face_pos);

    private:

};

#endif // SRC_BLOCKCURSOR_H_