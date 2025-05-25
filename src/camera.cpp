#include "Camera.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Camera::Camera(glm::vec3 p_start_position, Uint16 p_window_width, Uint16 p_window_height, float p_move_speed, float p_sensitivity, float p_fov, TotalFrame::KEYSET movement_keyset) 
             : position(p_start_position), window_width(p_window_width), window_height(p_window_height), fov(p_fov), sensitivity(p_sensitivity), move_speed(p_move_speed) {
    // calculate aspect ratio
    aspect_ratio = float(window_width / window_height);

    // set up move_queue
    move_queue.movement_keyset = movement_keyset;
    move_queue.UpdateKeyset();

    // default mouse posiition
    last_x = p_window_width / 2.0f;
    last_y = p_window_height / 2.0f;

    Camera::_UpdateMatrices();

    projection_matrix = glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
    inverse_projection_matrix = glm::inverse(projection_matrix);
}

//=============================
// GENERAL FUNCTIONS
//=============================

glm::mat4 Camera::GetViewMatrix() {
    return view_matrix;
}

glm::mat4 Camera::GetProjectionMatrix() {
    return projection_matrix;
}

//=============================
// SHADER PROGRAM FUNCTIONS
//=============================

void Camera::UpdateShaderProgram(GLuint shader_program) {
    glUseProgram(shader_program);

    // pass view and projection to shader program
    GLuint view_location = glGetUniformLocation(shader_program, "view");
    GLuint projection_location = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection_matrix));
}

void Camera::UpdateShaderPrograms(std::vector<GLuint> shader_programs) {
    for (auto& shader_program : shader_programs) {
        UpdateShaderProgram(shader_program);
    }
}

//=============================
// KEYBOARD MOVEMENT FUNCTIONS
//=============================

void Camera::StartMove(SDL_Keycode key) {
    move_queue.Add(key);
}

bool Camera::UpdateMovement() {
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

    Camera::_UpdateMatrices();

    return true;
}


void Camera::StopMove(SDL_Keycode key) {
    move_queue.Remove(key);
}

//=============================
// MOUSE MOVEMENT FUNCTIONS
//=============================

void Camera::StartMouseMove(float x, float y) {
    if (!mouse_moving) {
        last_x = x;
        last_y = y;
        mouse_moving = true;
    }
}

bool Camera::UpdateMouseMovement(float x, float y) {
    if (!mouse_moving) return false;

    float x_offset = (last_x - x) * sensitivity;
    float y_offset = (last_y - y) * sensitivity;

    yaw += x_offset;
    pitch -= y_offset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    if (yaw > 360.0f) yaw -= 360.0f;
    if (yaw < 0.0f) yaw += 360.0f;

    Camera::_UpdateCameraDirections();

    Camera::_UpdateMatrices();

    last_x = x;
    last_y = y;

    return true;
}

void Camera::StopMouseMove() {
    if (mouse_moving) mouse_moving = false;
}

//=============================
// RAY FUNCTIONS
//=============================

TotalFrame::Ray Camera::MouseToWorldRay(float mouse_x, float mouse_y) {
    // normalized device coordinates (NDC)
    float x = (2.0f * mouse_x) / window_width - 1.0f;
    float y = 1.0f - (2.0f * mouse_y) / window_height;  // flip y because OpenGL's origin is at bottom-left

    // get the ray in clip space
    glm::vec4 clip_space_ray(x, y, -1.0f, 1.0f);

    // transform to eye space, then to world space
    glm::vec4 eye_space_ray = glm::vec4(x, y, -1.0f, 1.0f);
    eye_space_ray = inverse_projection_matrix * eye_space_ray;
    eye_space_ray.z = -1.0f; // keep direction pointing forward
    eye_space_ray.w = 0.0f;

    // transform from eye space to world space
    glm::vec4 world_ray = inverse_view_matrix * eye_space_ray;

    glm::vec3 cam_world_position = glm::vec3(inverse_view_matrix[3]);

    return TotalFrame::Ray(cam_world_position, glm::normalize(glm::vec3(world_ray))); 
}

//=============================
// PRIVATE FUNCTIONS
//=============================

void Camera::_UpdateMatrices() {
    view_matrix = glm::lookAt(position, position + front, up);;
    inverse_view_matrix = glm::inverse(view_matrix);
}

void Camera::_UpdateCameraDirections() {
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