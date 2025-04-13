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
#include "LightHandler.h"
#include "Triangle.h"
#include "Cube.h"
#include "Object.h"
#include "Creator.h"
#include "BlockCursor.h"
#include "Shape.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

int main(int argc, char* argv[]) {
    ////////// APP INITILIZATION
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    TTF_Init();

    ////////// APP HANDLERS
    WindowHandler window_handler(1920, 1080, {0.025f, 0.05f, 0.10f, 1.0f}, "TotalFrame3D Object Creator", false, 60.0f);
    AudioHandler audio_handler;
    ShaderHandler shader_handler(window_handler.context);
    CameraHandler camera(glm::vec3(0.0f, 1.0f, 3.0f), window_handler.width, window_handler.height, 0.025f, 0.1f, 70.0f);
    Creator creator(std::filesystem::current_path().string() + "/../objects", std::filesystem::current_path().string() + "/../export");
    LightHandler light_handler;

    window_handler.PassNamePtr(creator.GetName());

    ////////// APP VARIABLES
    //// GENERAL
    bool app_running = true;
    bool load_state = true;
    SDL_Event event;
    std::string app_state = "game";
    //// INPUT
    float mouse_x, mouse_y; //SDL_GetMouseState(&mouse_x, &mouse_y);
    Cube mouse_cube;
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
    GLuint cube_sp = shader_handler.CreateShaderProgram("res/shaders/cube");
    GLuint block_cursor_sp = shader_handler.CreateShaderProgram("res/shaders/block_cursor");

    Object object(TotalFrame::OBJECT_TYPE::CUBE_OBJ, window_handler.aspect_ratio);

    creator.SetCubeDefault(Cube("cube", TotalFrame::READ_POS_FROM_FILE, TotalFrame::TRIANGLE_SIZE, "res/tfobj/cube.tfobj_dev", cube_sp, window_handler.aspect_ratio));

    object.Create("starting cube", TotalFrame::READ_POS_FROM_FILE, TotalFrame::TRIANGLE_SIZE, "res/tfobj/starting_cube.tfobj_dev", cube_sp);

    light_handler.Create(glm::vec3(0.5f, 1.2f, 1.0f), glm::vec3(1.0f), 2.0f);

    BlockCursor block_cursor("block cursor", TotalFrame::READ_POS_FROM_FILE, TotalFrame::TRIANGLE_SIZE, "res/tfobj/block_cursor.tfobj_dev", block_cursor_sp, window_handler.aspect_ratio);

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
                    
                    case SDL_EVENT_MOUSE_BUTTON_DOWN:
                        ////////
                        //
                        // CUBE PLACEMENT
                        //
                        ////////

                        if (event.button.button == SDL_BUTTON_LEFT) {
                            //// CHECK IF HITTING CUBE FACE
                            glm::vec3 face_hit_pos;
                            //// GET FIRST CUBE HIT
                            mouse_cube = object.GetRayCollidingCubeWithFace(camera.MouseToWorldRay(mouse_x, mouse_y), face_hit_pos);
                            block_cursor.PlaceOnFace(mouse_cube.GetTTPosition(), face_hit_pos);
                        
                            if (block_cursor.visible) {
                                creator.UpdateCubeDefaultPosition(block_cursor.NextCubePosition());
                                if (creator.GetShape().type == TotalFrame::SHAPE_TYPE::SHAPE_NONE) object.Create(creator.GetCubeDefault().name, creator.GetCubeDefaultPosition(), creator.GetCubeDefault().size[0], "", creator.GetCubeDefault().shader_program, creator.GetCubeDefault().GetTrueData());
                                else object.CreateShape(creator.GetShape());
                                window_handler.NeedRender();
                            }
                        }

                        if (event.button.button == SDL_BUTTON_RIGHT) {
                            //// CHECK IF HITTING CUBE FACE
                            glm::vec3 face_hit_pos;
                            //// GET FIRST CUBE HIT
                            mouse_cube = object.GetRayCollidingCubeWithFace(camera.MouseToWorldRay(mouse_x, mouse_y), face_hit_pos);
                        
                            if (face_hit_pos != glm::vec3(-1000.0f)) {
                                object.Destory(object.GetRayCollidingCubeWithFacePtr(camera.MouseToWorldRay(mouse_x, mouse_y), face_hit_pos));
                                window_handler.NeedRender();
                            }
                        }

                        ////////
                        //
                        // MOUSE MOVEMENT
                        //
                        ////////

                        if (event.button.button == SDL_BUTTON_MIDDLE) {
                            SDL_GetMouseState(&mouse_x, &mouse_y);
                            camera.StartMouseMove(mouse_x, mouse_y);
                        }
                        break;

                    case SDL_EVENT_MOUSE_MOTION:
                        SDL_GetMouseState(&mouse_x, &mouse_y);

                        if (block_cursor.visible) {
                            window_handler.NeedRender();
                        }
                        
                        //// FACE TESTING
                        glm::vec3 face_hit_pos;
                        mouse_cube = object.GetRayCollidingCubeWithFace(camera.MouseToWorldRay(mouse_x, mouse_y), face_hit_pos);
                        block_cursor.PlaceOnFace(mouse_cube.GetTTPosition(), face_hit_pos);

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
                            //// SAVING
                            if (event.key.key == SDLK_S) {
                                creator.Save(object.GetTrueData());
                                window_handler.UpdateName();
                            }

                            //// LOADING
                            if (event.key.key == SDLK_O) {
                                if (*creator.GetName() != "untitled") creator.Save(object.GetTrueData());
                                std::string loaded_object_path = creator.Load();
                                if (loaded_object_path != "\n") {
                                    object.ClearAndCreate("new object", TotalFrame::READ_POS_FROM_FILE, TotalFrame::TRIANGLE_SIZE, loaded_object_path, cube_sp);
                                    window_handler.NeedRender();
                                    window_handler.UpdateName();
                                }
                            }

                            //// NEW OBJECT
                            if (event.key.key == SDLK_N) {
                                if (*creator.GetName() != "untitled") creator.Save(object.GetTrueData());
                                if (creator.NewObject()) {
                                    object.ClearAndCreate("starting cube", TotalFrame::READ_POS_FROM_FILE, TotalFrame::TRIANGLE_SIZE, "res/tfobj/starting_cube.tfobj_dev", cube_sp);
                                    window_handler.NeedRender();
                                    window_handler.UpdateName();
                                }
                            }

                            //// EXPORT
                            if (event.key.key == SDLK_M) {
                                creator.Export(object.GetExportData());
                            }
                            
                            //// ROTATE CAMERA LEFT
                            if (event.key.key == SDLK_Q) {

                            }

                            //// ROTATE CAMERA RIGHT
                            if (event.key.key == SDLK_E) {

                            }

                            //// COLOR PICKER
                            if (event.key.key == SDLK_T) {

                            }

                            //// SYMMETRY TOGGLE
                            if (event.key.key == SDLK_1) {
                                creator.ToggleSymmetry(creator.GetCubeDefault(), TotalFrame::SYMMETRY_TYPE::ALL_AXIS);
                            }
                        }

                        if (event.key.key == SDLK_TAB) {
                            creator.ChooseColor();
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

                camera.UpdateShaderPrograms(object.GetShaderProgramsUpdates());
                object.UpdateAndRenderAll(camera.GetViewProjectionMatrix(), camera.position, light_handler.lights);

                // update block_cursor seperate so it doesn't get saved to tfobj file
                if (block_cursor.visible) {
                    camera.UpdateShaderPrograms({block_cursor_sp});
                    object.UpdateAndRender(block_cursor.cube, camera.GetViewProjectionMatrix(), camera.position, light_handler.lights);
                }

                window_handler.Update();
                window_handler.EndRender();
            }
        }
    }

    ////////// MEMORY MANAGEMENT
    audio_handler.FreeAll();
    object.FreeAll();

    SDL_Quit();
    Mix_Quit();
    TTF_Quit();

    return 0;
}