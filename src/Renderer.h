#ifndef SRC_RENDERER_H_
#define SRC_RENDERER_H_

#pragma once

#include <iostream>

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <thread>
#include <future>

#include "TotalFrame.h"
#include "Util.h"

// things that can be rendered
#include "Skybox.h"
#include "Object.h"
#include "Texture.h"

class Renderer {
    public:
        Renderer();

        void Add(const Skybox& item, int layer = -1);
        void Add(const Object& item, int layer = -1);
        void Add(const Texture& item, int layer = -1);

        void Clear();

        std::vector<GLuint> GetShaderProgramsUpdates();

        void RenderAll(glm::mat4 camera_view_matrix, glm::mat4 camera_projection_matrix, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights);

    private:
        std::vector<std::pair<Skybox, int>> skyboxes = {};
        std::vector<std::pair<Object, int>> objects = {};
        std::vector<std::pair<Texture, int>> textures = {};

        std::vector<TotalFrame::RenderItem> render_queue = {};
        
};

#endif // SRC_RENDERER_H_