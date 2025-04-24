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


using TF_MOVEMENT_KEYSET = std::array<SDL_Keycode, 6>;
using TF_TRIANGLE_VERTICES = std::array<GLfloat, 18>;
using TF_TRIANGLE_VERTICES_WITH_NORMAL = std::array<GLfloat, 27>;

class TotalFrame {
    public:
        enum KEYSET {
            WASD,
            ARROWKEYS
        };
        
        enum OBJECT_TYPE {
            CUBE_OBJ,
            TILE_OBJ,
            TEXTURE_OBJ
        };

        enum SHAPE_TYPE {
            SHAPE_NONE,
            SYMMETRY,
            SPHERE,
            SQUARE
        };

        enum SYMMETRY_TYPE {
            SYMMETRY_NONE,
            ALL_AXIS,
            X_AXIS,
            Y_AXIS,
            Z_AXIS
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

        static constexpr float READ_SIZE_FROM_FILE = -1000.0f;
        static constexpr glm::vec3 READ_POS_FROM_FILE = glm::vec3(-1000.0f);

        struct Material {
            Material(glm::vec3 p_ambient, glm::vec3 p_diffuse, glm::vec3 p_specular, GLfloat p_shininess) : ambient(p_ambient), diffuse(p_diffuse), specular(p_specular), shininess(p_shininess) {
                ;
            }

            glm::vec3 ambient = {};
            glm::vec3 diffuse = {};
            glm::vec3 specular = {};
            GLfloat shininess = 0.0f;

            Material() = default;
        };

        struct Light {
            Light(glm::vec3 p_position, glm::vec3 p_color, GLfloat p_intensity) : position(p_position), color(p_color), intensity(p_intensity) {
                ;
            }

            glm::vec3 position = glm::vec3(0.0f);
            glm::vec3 color = glm::vec3(0.0f);
            GLfloat intensity = 1.0f;

            Light() = default;
        };

        struct MoveQueue {
            MoveQueue(TotalFrame::KEYSET p_movement_keyset) : movement_keyset(p_movement_keyset) {
                UpdateKeyset();
            }

            TotalFrame::KEYSET movement_keyset = WASD;

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