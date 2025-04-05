#ifndef SRC_CREATOR_H_
#define SRC_CREATOR_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

//// TFD
#include "tinyfiledialogs.h"

#include <filesystem>
#include <fstream>

#include "TotalFrame.h"
#include "Util.h"
#include "ObjectHandler.h"

class Creator {
    public:
        Creator(std::string objects_path);

        bool saved = false;

        // save object data
        void Save(std::string objects_data);

        // load object data
        std::string Load();

        void Backup();

        void ChooseColor();

    private:
        glm::vec4 color = glm::vec4(1.0f);
        std::string objects_path = "";

        bool first_save = false;

        const char* filter_patterns[1] = {"*.tfobj"};

        void _InitSave(std::string objects_data);
        
    
};

#endif // SRC_CREATOR_H_