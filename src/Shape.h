#ifndef SRC_SHAPE_H_
#define SRC_SHAPE_H_

#pragma once

#include <iostream>
#include <vector>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Cube.h"

class Shape {
    public:
        Shape();

        Cube cube;
        std::vector<glm::vec3> positions = {};
        TotalFrame::SHAPE_TYPE type = TotalFrame::SHAPE_TYPE::SHAPE_NONE;
        TotalFrame::SYMMETRY_TYPE symmetry_type = TotalFrame::SYMMETRY_TYPE::SYMMETRY_NONE;

        void SetSymmetryPositions(glm::vec3 position);

        void Clear();

    private:

};

#endif // SRC_SHAPE_H_