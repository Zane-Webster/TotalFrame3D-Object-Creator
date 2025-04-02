#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <ctime>
#include <SDL3/SDL.h>

#include "TotalFrame.h"

/*
ABOUT:
Contains extremeley important inline functions.
General spot for functions used among many files.
*/

class Util {
    public:
        ////////// RECT CENTERING
        static SDL_Rect CenterRect(SDL_Rect p_rect);
        static SDL_Rect CenterRectOnPoint(SDL_Rect rect, SDL_Point position);

        ////////// MATH
        static int ToggleInt(int x, int max);
        static std::array<float, 2> Normalize(std::array<float, 2> velocity);

        ////////// OUTPUT CONTROL
        static void Debug(std::string message);
        // Pass __func__ as second parameter. Cannot be set as default param.
        static void ThrowError(std::string error, std::string function);
        static void GetOpenGLError();

        ////////// COMPARISON
        static bool ComparePoint(SDL_Point a, SDL_Point b);
};

#endif