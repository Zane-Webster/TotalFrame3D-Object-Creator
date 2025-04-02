#ifndef SRC_GRID_H_
#define SRC_GRID_H_

#pragma once

#include <iostream>
#include <vector>

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"

class Grid {
    public:
        Grid(float size = 0.05f);

        float size;

        std::vector<glm::vec3> coordinates;

        void Render();

    private:
    
};

#endif // SRC_GRID_H_