#include "WindowHandler.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

WindowHandler::WindowHandler(Uint16 p_w, Uint16 p_h, SDL_FColor p_color, std::string p_title, float p_target_fps) 
                            : width(p_w), height(p_h), color(p_color), title(p_title), target_fps(p_target_fps) {

    // INIT OPENGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // CREATE WINDOW
    window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
    
    // OPENGL CONTEXT + GLEW
    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    // Use Vsync
    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) Util::ThrowError("GLEW NOT INITIALIZED PROPERLY", "windowhandler");

    glEnable(GL_DEPTH_TEST);
    glClearColor(color.r, color.g, color.b, color.a);
    glViewport(0, 0, width, height);

    frame_duration = 1000.0f / target_fps;
}

//=============================
// RENDERING
//=============================

void WindowHandler::Update() {
    SDL_GL_SwapWindow(window);
}

void WindowHandler::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//=============================
// DELTA TIME
//=============================

void WindowHandler::UpdateDeltaTime(Uint64 p_current_app_time, Uint64 time_frequency) {
    last_app_time = current_app_time;
    current_app_time = p_current_app_time;

    // CALCULATE DELTA TIME
    double calculated_delta_time = (current_app_time - last_app_time) / (double)time_frequency * 1000.0f;

    // IF IT'S AN OUTLIER, GET RID OF IT
    if (calculated_delta_time <= 0.0) calculated_delta_time = 1.0;

    // SET IT EQUAL TO DELTA TIME PTR
    *delta_time = calculated_delta_time;

    // SLEEP TO MAKE TARGET FPS
    if (*delta_time < floor(frame_duration - *delta_time)) SDL_Delay(floor(frame_duration - *delta_time));
}

//=============================
// MEMORY MANAGEMENT
//=============================

void WindowHandler::FreeAll() {
    SDL_DestroyWindow(window);
    SDL_GL_DestroyContext(context);
}

//=============================
// SETTERS
//=============================

void WindowHandler::NeedRender() {
    need_render = true;
}

bool WindowHandler::StartRender() {
    return need_render;
}

void WindowHandler::EndRender() {
    need_render = false;
}

void WindowHandler::SetColor(SDL_FColor p_color) {
    color = p_color;
}

//=============================
// GETTERS
//=============================

std::shared_ptr<double> WindowHandler::DeltaTime() {
    return delta_time;
}