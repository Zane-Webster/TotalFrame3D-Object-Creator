#ifndef SRC_LIGHTHANDLER_H_
#define SRC_LIGHTHANDLER_H_

#pragma once

#include <iostream>
#include <memory>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"

class LightHandler {
    public:
        LightHandler();

        void Create(glm::vec3 position, glm::vec3 color, GLfloat intensity);
        std::shared_ptr<TotalFrame::Light> CreateAndReturn(glm::vec3 position, glm::vec3 color, GLfloat intensity);

        std::vector<std::shared_ptr<TotalFrame::Light>> lights = {};

    private:

};

#endif // SRC_LIGHTHANDLER_H_