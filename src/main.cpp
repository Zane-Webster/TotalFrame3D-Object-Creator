/*
TOTALFRAME3D Object Creator

Object Creator based on TotalFrame3D

ZANE WEBSTER
2024-2025
*/

//// GENERAL LIBRARIES
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <string>

#include <filesystem>

//// SDL LIBRARIES
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>

//// GLEW
#include <GL/glew.h>

//// GLM
#include <glm/glm.hpp>

//// TFD
#include "tinyfiledialogs.h"

//// TOTAL-FRAME LIBRARIES
#include "TotalFrame.h"
#include "Util.h"
#include "WindowHandler.h"
#include "AudioHandler.h"
#include "ShaderHandler.h"
#include "CameraHandler.h"
#include "Triangle.h"
#include "Object.h"
#include "ObjectHandler.h"
#include "Creator.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

int main(int argc, char* argv[]) {
    ////////// APP INITILIZATION
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    TTF_Init();

    ////////// APP HANDLERS
    WindowHandler window_handler(1280, 720, {0.025f, 0.05f, 0.10f, 1.0f}, "TotalFrame3D Object Creator", 60.0f);
    AudioHandler audio_handler;
    ShaderHandler shader_handler(window_handler.context);
    CameraHandler camera(glm::vec3(0.0f, 0.0f, 6.0f), window_handler.width, window_handler.height, 0.025f, 0.1f, 70.0f);
    ObjectHandler object_handler(window_handler.aspect_ratio);
    Creator creator(std::filesystem::current_path().string());

    window_handler.PassNamePtr(creator.GetName());

    ////////// APP VARIABLES
    //// GENERAL
    bool app_running = true;
    bool load_state = true;
    SDL_Event event;
    std::string app_state = "game";
    //// INPUT
    float mouse_x, mouse_y; //SDL_GetMouseState(&mouse_x, &mouse_y);
    std::shared_ptr<Object> mouse_object = nullptr;
    std::shared_ptr<double> delta_time = window_handler.DeltaTime();
    TotalFrame::KEYSET keyset = TotalFrame::KEYSET::WASD;
    TF_MOVEMENT_KEYSET movement_keys = TotalFrame::MOVEMENT_KEYS[keyset];

    ////////// LOAD IMAGES

    ////////// LOAD RECTS
    // Load rects here, then add to render list in load_state based on what you need for the app_state

    ////////// LOAD AUDIO
    // Load audio here, then add to load_state based on what you need for the app_state

    ////////// GAME VARIABLES

    ////////// GAME OBJECTS
    GLuint cube_sp = shader_handler.CreateShaderProgram("res/cube_shader");
    
    //object_handler.Create("cube", {0.0f, 0.0f, 0.0f}, TotalFrame::OBJECT_TYPE::BASIC_OBJ, 0.1f, "res/tfobj/cube.tfobj", cube_sp);

    ////////// TEXT

    ////////// MAIN LOOP
    while (app_running) {
        window_handler.UpdateDeltaTime(SDL_GetPerformanceCounter(), SDL_GetPerformanceFrequency());

        //=============================
        // GAME
        //=============================

        if (app_state == "game") {
            // Initial load for the app_state
            if (load_state) {
                load_state = false;

                //// CLEAR LISTS
                audio_handler.ClearAll();

                //// IMAGES

                //// RECTS

                //// AUDIO

            }

            ////////// EVENTS
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_QUIT: 
                        app_running = false;
                        break;

                    ////////
                    //
                    // MOUSE MOVEMENT
                    //
                    ////////
                    
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            creator.ChooseColor();
                        }
                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            SDL_GetMouseState(&mouse_x, &mouse_y);
                            camera.StartMouseMove(mouse_x, mouse_y);
                        }
                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            //
                        }
                        break;
                    case SDL_EVENT_MOUSE_MOTION:
                        //// FACE TESTING
                        SDL_GetMouseState(&mouse_x, &mouse_y);
                        glm::vec3 face_hit_direction;
                        mouse_object = object_handler.GetRayCollidingObjectWithFace(camera.MouseToWorldRay(mouse_x, mouse_y), face_hit_direction);
                        if (mouse_object != nullptr) {
                            Util::Debug(mouse_object->name);
                            Util::Debug(glm::to_string(face_hit_direction));
                        }

                        if (event.motion.state && SDL_BUTTON_MMASK) {
                            SDL_GetMouseState(&mouse_x, &mouse_y);
                            if (camera.UpdateMouseMovement(mouse_x, mouse_y)) window_handler.NeedRender();
                        }
                        break;
                    case SDL_EVENT_MOUSE_BUTTON_UP:
                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            camera.StopMouseMove();
                        }
                        break;

                    case SDL_EVENT_KEY_DOWN:
                        ////////
                        //
                        // FUNCTION KEYS
                        //
                        ////////

                        if (SDL_GetModState() & SDL_KMOD_ALT) {
                            if (event.key.key == SDLK_S) {
                                creator.Save(object_handler.GetData());
                                window_handler.UpdateName();
                            }
                            if (event.key.key == SDLK_O) {
                                std::string loaded_object_path = creator.Load();
                                if (loaded_object_path != "\n") {
                                    object_handler.ClearAndCreate("cube", glm::vec3(0.0f), TotalFrame::OBJECT_TYPE::BASIC_OBJ, 0.1f, loaded_object_path, cube_sp);
                                    window_handler.NeedRender();
                                    window_handler.UpdateName();
                                }
                            }
                        }

                        ////////
                        //
                        // KEYBOARD MOVEMENT
                        //
                        ////////

                        for (auto movement_key : movement_keys) {
                            if (event.key.key == movement_key) {
                                camera.StartMove(event.key.key);
                            }
                        }
                        break;
                    case SDL_EVENT_KEY_UP:
                        for (auto movement_key : movement_keys) {
                            if (event.key.key == movement_key) {
                                camera.StopMove(event.key.key);
                            }
                        }
                        break;
                }
            }

            if (camera.UpdateMovement()) window_handler.NeedRender();

            ////////
            //
            // RENDERING
            //
            ////////

            if (window_handler.StartRender()) {
                window_handler.Clear();
                
                camera.UpdateShaderPrograms(object_handler.GetShaderProgramsUpdates());
                object_handler.UpdateAndRenderAll(camera.GetViewProjectionMatrix(), camera.position);

                window_handler.Update();
                window_handler.EndRender();
            }
        }
    }

    ////////// MEMORY MANAGEMENT
    audio_handler.FreeAll();
    object_handler.FreeAll();

    SDL_Quit();
    Mix_Quit();
    TTF_Quit();

    return 0;
}