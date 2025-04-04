#ifndef SRC_CAMERAHANDLER_H_
#define SRC_CAMERAHANDLER_H_

#pragma once

#include <iostream>

#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TotalFrame.h"
#include "Util.h"

/*
ABOUT:
Controls the camera, including movement, mouse movement and converting rays to World View.

NOTES:
Ensure you attach ViewProjectionMatrix to ObjectHandler

*/

class CameraHandler {
    public:
        //////// CONSTUCTOR
        CameraHandler(glm::vec3 start_position, Uint16 window_width, Uint16 window_height, float move_speed = 0.01f, float sensitivity = 0.1f, float fov = 45.0f, TotalFrame::KEYSET movement_keyset = TotalFrame::KEYSET::WASD);

        //////// CAMERA ATTRIBUTES
        // camera pos
        glm::vec3 position;

        //////// GENERAL FUNCTIONS
        glm::mat4 GetViewProjectionMatrix();

        //////// SHADER PROGRAM FUNCTIONS
        void UpdateShaderProgram(GLuint shader_program);
        void UpdateShaderPrograms(std::vector<GLuint> shader_programs);

        //////// CAMERA MOVEMENT FUNCTIONS
        void StartMove(SDL_Keycode key);
        bool UpdateMovement();
        void StopMove(SDL_Keycode key);

        //////// CAMERA MOUSE MOVEMENT FUNCTIONS
        void StartMouseMove(float x, float y);
        bool UpdateMouseMovement(float x, float y);
        void StopMouseMove();

        //////// RAY FUNCTIONS
        TotalFrame::Ray MouseToWorldRay(float x, float y);

    private:
        //////// CAMERA ATTRIBUTES
        float aspect_ratio;
        float fov;
        float near_plane = 0.1f;
        float far_plane = 100.0f;

        //////// MOVEMENT ATTRIBUTES
        // move speed of the camera keyboard
        float move_speed;
        TotalFrame::MoveQueue move_queue;

        //////// EXTERNAL ATTRIBUTES
        Uint16 window_width, window_height;

        //////// MOUSE ATTRIBUTES
        bool mouse_moving = false;
        float sensitivity;
        float yaw = -90.0f;
        float pitch = 0.0f;
        float last_x, last_y;

        //////// POSITION ATTRIBUTES
        // the front of where the camera is facing (direction)
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        // which direction in the world is up
        glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
        // which direction of the camera is right
        glm::vec3 right;
        // which direction of the camera is up
        glm::vec3 up;

        //// OPENGL CAMERA FUNCTIONS
        glm::mat4 _GetViewMatrix();
        glm::mat4 _GetProjectionMatrix();

        //////// MOUSE MOVEMENT FUNCTIONS
        void _UpdateCameraDirections();
};

#endif // SRC_CAMERAHANDLER_H_