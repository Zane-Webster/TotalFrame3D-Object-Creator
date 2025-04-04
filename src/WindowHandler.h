#ifndef SRC_WINDOWHANDLER_H_
#define SRC_WINDOWHANDLER_H_

#pragma once

#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL3/SDL.h>
#include <GL/glew.h>

#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

#include "TotalFrame.h"
#include "Util.h"

/*
ABOUT:
Controls the renderer, window information and frame rate.

NOTES:
Creates OpenGL context. Can be accesed by direct reference with window_handler.context.
*/

class WindowHandler {
    public:
        ////////// DEFAULT CONSTRUCTOR
        WindowHandler(Uint16 window_width, Uint16 window_height, SDL_FColor window_color, std::string window_title, float target_fps = 60.0f);
        ~WindowHandler();

        ////////// BASICS
        SDL_Window* window = nullptr;
        Uint16 width, height;
        float target_fps;
        SDL_GLContext context;
        float aspect_ratio = 0.0f;

        ////////// RENDERING
        void Update();
        void Clear();

        ////////// DELTA TIME
        void UpdateDeltaTime(Uint64 current_app_time, Uint64 time_frequency);

        ////////// SETTERS
        void NeedRender();
        bool StartRender();
        void EndRender();
        void SetColor(SDL_FColor color);

        ////////// GETTERS
        std::shared_ptr<double> DeltaTime(); 

    private:

        ////////// BASICS
        std::string title;
        SDL_FColor color;
        bool need_render = true;

        ////////// DELTA TIME
        float frame_duration = 0.0f;
        Uint64 last_app_time = 0;
        Uint64 current_app_time = 0;
        std::shared_ptr<double> delta_time = std::make_shared<double>(0.0);;

};

#endif /* SRC_WINDOWHANDLER_H_ */