#ifndef SRC_TOTALFRAME_H_
#define SRC_TOTALFRAME_H_

#pragma once

#include <iostream>
#include <array>
#include <unordered_map>

#include <SDL3/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Util.h"

/*
ABOUT:
Contains TotalFrame structs
*/

#ifndef TF_MOVEMENT_KEYSET
#define TF_MOVEMENT_KEYSET std::array<SDL_Keycode, 6>
#endif

#ifndef TF_TRIANGLE_VERTICES
#define TF_TRIANGLE_VERTICES std::array<GLfloat, 18>
#endif

class TotalFrame {
    public:
        enum KEYSET {
            WASD,
            ARROWKEYS
        };
        
        enum OBJECT_TYPE {
            BASIC_OBJ, // POSITION AND COLOR
            TEXTURE_OBJ // POSITION AND TEXTURE
        };

        static constexpr std::array<SDL_Keycode, 2> MOVEMENT_KEY_LEFT = {SDLK_A, SDLK_LEFT};
        static constexpr std::array<SDL_Keycode, 2> MOVEMENT_KEY_RIGHT = {SDLK_D, SDLK_RIGHT};
        static constexpr std::array<SDL_Keycode, 2> MOVEMENT_KEY_UP = {SDLK_SPACE, SDLK_SPACE};
        static constexpr std::array<SDL_Keycode, 2> MOVEMENT_KEY_DOWN = {SDLK_LCTRL, SDLK_LCTRL};
        static constexpr std::array<SDL_Keycode, 2> MOVEMENT_KEY_FORWARD = {SDLK_W, SDLK_UP};
        static constexpr std::array<SDL_Keycode, 2> MOVEMENT_KEY_BACK = {SDLK_S, SDLK_DOWN};

        static constexpr TF_MOVEMENT_KEYSET WASD_MOVEMENT_KEYS = {SDLK_W, SDLK_A, SDLK_S, SDLK_D, SDLK_SPACE, SDLK_LCTRL};
        static constexpr TF_MOVEMENT_KEYSET ARROW_MOVEMENT_KEYS = {SDLK_UP, SDLK_LEFT, SDLK_DOWN, SDLK_RIGHT, SDLK_SPACE, SDLK_LCTRL};
        static constexpr std::array<TF_MOVEMENT_KEYSET, 2> MOVEMENT_KEYS = {WASD_MOVEMENT_KEYS, ARROW_MOVEMENT_KEYS};

        static constexpr float TRIANGLE_SIZE = 0.1f;
        static constexpr glm::vec3 READ_POS_FROM_FILE = glm::vec3(-1000.0f);

        struct MoveQueue {
            MoveQueue(KEYSET p_movement_keyset) : movement_keyset(p_movement_keyset) {
                UpdateKeyset();
            }

            KEYSET movement_keyset = WASD;

            // x, y, z
            // x = -1 = "left" :: x = 1 = "right" 
            // y = -1 = "up" :: y = 1 = "down"
            // z = -1 = "back" :: z = 1 = "forward"
            std::array<int, 3> move_dir;

            // translates keys to direction for direction vector. {key value, {move_dir index, move_dir value}}
            std::unordered_map<SDL_Keycode, std::pair<int, int>> key_to_dir;

            // adds movement direction to move_dir
            void Add(SDL_Keycode key) {
                move_dir[key_to_dir[key].first] = key_to_dir[key].second;
            }

            // removes movement direction from move_dir
            void Remove(SDL_Keycode key) {
                if (move_dir[key_to_dir[key].first] == key_to_dir[key].second) move_dir[key_to_dir[key].first] = 0;
            }

            void UpdateKeyset() {
                key_to_dir = {
                    {TotalFrame::MOVEMENT_KEY_LEFT[movement_keyset], {0, -1}}, {TotalFrame::MOVEMENT_KEY_RIGHT[movement_keyset], {0, 1}},  // x
                    {TotalFrame::MOVEMENT_KEY_DOWN[movement_keyset], {1, -1}}, {TotalFrame::MOVEMENT_KEY_UP[movement_keyset], {1, 1}},     // y
                    {TotalFrame::MOVEMENT_KEY_BACK[movement_keyset], {2, -1}}, {TotalFrame::MOVEMENT_KEY_FORWARD[movement_keyset], {2, 1}} // z
                };
            }

            MoveQueue() = default;
        };

        struct Ray {
            Ray(glm::vec3 p_origin, glm::vec3 p_direction) : origin(p_origin), direction(p_direction) {
                ;
            }

            glm::vec3 origin = glm::vec3(0.0f);
            glm::vec3 direction = glm::vec3(0.0f);

            Ray() = default; 
        };
};

#endif // SRC_TOTALFRAME_H_