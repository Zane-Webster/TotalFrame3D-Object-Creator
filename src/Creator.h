#ifndef SRC_CREATOR_H_
#define SRC_CREATOR_H_

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>

//// TFD
#include "tinyfiledialogs.h"

#include <filesystem>
#include <fstream>

#include "TotalFrame.h"
#include "Util.h"
#include "Shape.h"
#include "Cube.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

class Creator {
    public:
        Creator(std::string objects_path, std::string exports_path);

        //////// BASIC ATTRIBUTES
        glm::vec4 color = glm::vec4(1.0f);

        //////// BASIC FUNCTIONS
        std::shared_ptr<std::string> GetName();

        //////// CUBE DEFAULT FUNCTIONS
        void SetCubeDefault(Cube cube);
        Cube GetCubeDefault();
        glm::vec3 GetCubeDefaultPosition();
        void UpdateCubeDefaultPosition(glm::vec3 position);

        //////// SHAPE CREATION FUNCTIONS
        Shape GetShape();
        void SetShape(TotalFrame::SHAPE_TYPE type, Cube cube);

        void ToggleSymmetry(Cube cube, TotalFrame::SYMMETRY_TYPE symmetry_type = TotalFrame::SYMMETRY_TYPE::ALL_AXIS);

        //////// SAVING FUNCTIONS
        void Save(std::string object_data);
        bool NewObject();

        void Export(std::string object_data);
        bool NewExport();

        //////// LOADING FUNCTIONS
        std::string Load();

        //////// COLOR FUNCTIONS
        void ChooseColor();

    private:
        std::string objects_path = "";
        std::string exports_path = "";
        std::string object_path = "";
        std::shared_ptr<std::string> object_name = std::make_shared<std::string>("untitled");

        Shape shape;

        // the cube that will be placed by default
        Cube cube_default;
        Cube adjusted_cube_default;

        const char* filter_patterns[1] = {"*.tfobj_dev"};
        const char* export_filter_patterns[1] = {"*.tfobj"};
};

#endif // SRC_CREATOR_H_