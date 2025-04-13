#include "LightHandler.h"

LightHandler::LightHandler() {
    ;
}

void LightHandler::Create(glm::vec3 position, glm::vec3 color, GLfloat intensity) {
    lights.push_back(std::make_shared<TotalFrame::Light>(position, color, intensity));
}

std::shared_ptr<TotalFrame::Light> LightHandler::CreateAndReturn(glm::vec3 position, glm::vec3 color, GLfloat intensity) {
    lights.push_back(std::make_shared<TotalFrame::Light>(position, color, intensity));
    return lights.back();
}