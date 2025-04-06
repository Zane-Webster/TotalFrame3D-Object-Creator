#ifndef SRC_CREATOR_H_
#define SRC_CREATOR_H_

#pragma once

#include <iostream>
#include <memory>
#include <string>

//// TFD
#include "tinyfiledialogs.h"

#include <filesystem>
#include <fstream>

#include "TotalFrame.h"
#include "Util.h"
#include "ObjectHandler.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

class Creator {
    public:
        Creator(std::string objects_path);

        glm::vec4 color = glm::vec4(1.0f);
        bool saved = false;

        void SetCubeDefault(Object object);
        Object GetCubeDefault();
        glm::vec3 GetCubeDefaultPosition();
        void UpdateCubeDefaultPosition(glm::vec3 position);

        // save object data
        void Save(std::string object_data);
        bool NewObject();

        // load object data
        std::string Load();

        void Backup();

        void ChooseColor();

        std::shared_ptr<std::string> GetName();

    private:
        std::string objects_path = "";
        std::string object_path = "";
        std::shared_ptr<std::string> object_name = std::make_shared<std::string>("untitled");

        // the cube that will be placed by default
        Object cube_default;
        Object adjusted_cube_default;

        const char* filter_patterns[1] = {"*.tfobj"};

        void _InitSave();
        
    
};

#endif // SRC_CREATOR_H_