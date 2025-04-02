#include "CameraHandler.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

CameraHandler::CameraHandler(glm::vec3 p_start_position, Uint16 p_window_width, Uint16 p_window_height, float p_move_speed, float p_sensitivity, float p_fov, TotalFrame::KEYSET movement_keyset) 
             : position(p_start_position), fov(p_fov), sensitivity(p_sensitivity), move_speed(p_move_speed) {
    // calculate aspect ratio
    aspect_ratio = float(p_window_width / p_window_height);

    // set up move_queue
    move_queue.movement_keyset = movement_keyset;
    move_queue.UpdateKeyset();

    // default mouse posiition
    last_x = p_window_width / 2.0f;
    last_y = p_window_height / 2.0f;
}

//=============================
// GENERAL FUNCTIONS
//=============================

void CameraHandler::UpdateShaderProgram(GLuint shader_program) {
    // get view and projection matrices
    glm::mat4 view = CameraHandler::_GetViewMatrix();
    glm::mat4 projection = CameraHandler::_GetProjectionMatrix();

    // pass view and projection to shader program
    GLuint view_location = glGetUniformLocation(shader_program, "view");
    GLuint projection_location = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
}

void CameraHandler::UpdateShaderPrograms(std::vector<GLuint> shader_programs) {
    for (auto shader_program : shader_programs) {
        UpdateShaderProgram(shader_program);
    }
}

glm::mat4 CameraHandler::GetViewProjectionMatrix() {
    return _GetProjectionMatrix() * _GetViewMatrix();
}

//=============================
// KEYBOARD MOVEMENT
//=============================

void CameraHandler::StartMove(SDL_Keycode key) {
    move_queue.Add(key);
}

bool CameraHandler::UpdateMovement() {
    if (move_queue.move_dir[0] == 0 && move_queue.move_dir[1] == 0 && move_queue.move_dir[2] == 0) return false;
    // moving left
    if (move_queue.move_dir[0] == -1) position -= glm::normalize(glm::cross(front, world_up)) * move_speed;
    // moving right
    else if (move_queue.move_dir[0] == 1) position += glm::normalize(glm::cross(front, world_up)) * move_speed;
    // moving down
    if (move_queue.move_dir[1] == -1) position -= world_up * move_speed;
    // moving up
    else if (move_queue.move_dir[1] == 1) position += world_up * move_speed;
    // moving back
    if (move_queue.move_dir[2] == -1) position -= move_speed * front;
    // moving forward
    else if (move_queue.move_dir[2] == 1) position += move_speed * front;

    return true;
}


void CameraHandler::StopMove(SDL_Keycode key) {
    move_queue.Remove(key);
}

//=============================
// MOUSE MOVEMENT
//=============================

void CameraHandler::StartMouseMove(float x, float y) {
    if (!mouse_moving) {
        last_x = x;
        last_y = y;
        mouse_moving = true;
    }
}

bool CameraHandler::UpdateMouseMovement(float x, float y) {
    if (!mouse_moving) return false;

    float x_offset = (last_x - x) * sensitivity;
    float y_offset = (last_y - y) * sensitivity;

    yaw += x_offset;
    pitch -= y_offset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;

    CameraHandler::_UpdateCameraDirections();

    last_x = x;
    last_y = y;

    return true;
}

void CameraHandler::StopMouseMove() {
    if (mouse_moving) mouse_moving = false;
}

//=============================
// PRIVATE FUNCTIONS
//=============================

glm::mat4 CameraHandler::_GetViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 CameraHandler::_GetProjectionMatrix() {
    return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
}

void CameraHandler::_UpdateCameraDirections() {
    // calculate front vector (direction) accounting for mouse movement
    glm::vec3 front_temp;
    front_temp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front_temp.y = sin(glm::radians(pitch));
    front_temp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front_temp);

    // calculate right and up vectors based on mouse movement
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}