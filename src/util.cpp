#include "Util.h"

//=============================
// RECT CENTERING
//=============================

SDL_Rect Util::CenterRect(SDL_Rect rect) {
    return {rect.x - rect.w/2, rect.y - rect.h/2, rect.w, rect.h};
}

SDL_Rect Util::CenterRectOnPoint(SDL_Rect rect, SDL_Point position) {
    return {position.x - rect.w/2, position.y - rect.h/2, rect.w, rect.h};
}

//=============================
// MATH
//=============================

int Util::ToggleInt(int x, int max) {
    if (x+1 > max) return 0;
    return x+1;
}

std::array<float, 2> Util::Normalize(std::array<float, 2> velocity) {
    std::array<float, 2> normalized_velocity;
    float hypotenuse = sqrt(velocity[0]*velocity[0]+velocity[1]*velocity[1]);
    normalized_velocity[0] = velocity[0] - (velocity[0] / hypotenuse);
    normalized_velocity[1] = velocity[1] - (velocity[1] / hypotenuse);
    return normalized_velocity;
}

//=============================
// OUTPUT CONTROL
//=============================

void Util::Debug(std::string message) {
    std::time_t time = std::time(0);
    std::tm* localTime = localtime(&time);
    std::cout << "DEBUG || TIME: " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << " || " << message << std::endl;
}

void Util::ThrowError(std::string error, std::string function) {
    std::time_t time = std::time(0);
    std::tm* localTime = localtime(&time);
    std::cout << "ERROR | " << function << " | "<< localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << " || " << error << std::endl;
}

void Util::GetOpenGLError() {
    std::time_t time = std::time(0);
    std::tm* localTime = localtime(&time);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "OPENGL ERROR | " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << " || " << error << std::endl;
    }
}

//=============================
// COMPARISON
//=============================

bool Util::ComparePoint(SDL_Point a, SDL_Point b) {
    return a.x == b.x && a.y == b.y;
}