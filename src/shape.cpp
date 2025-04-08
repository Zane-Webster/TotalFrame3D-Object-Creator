#include "Shape.h"

Shape::Shape() {
    
}

void Shape::SetSymmetryPositions(glm::vec3 position) {
    std::vector<glm::vec3> p_positions = {};

    glm::vec3 temp_position = position;

    p_positions.push_back(temp_position);

    switch (symmetry_type) {
        case TotalFrame::SYMMETRY_TYPE::SYMMETRY_NONE:
            Shape::Clear();
            break;
        case TotalFrame::SYMMETRY_TYPE::ALL_AXIS:
            if (temp_position.x != 0) p_positions.push_back(glm::vec3(-temp_position.x, temp_position.y, temp_position.z));
            if (temp_position.y != 0) p_positions.push_back(glm::vec3(temp_position.x, -temp_position.y, temp_position.z));
            if (temp_position.z != 0) p_positions.push_back(glm::vec3(temp_position.x, temp_position.y, -temp_position.z));
            if (temp_position.x != 0 && temp_position.y != 0) p_positions.push_back(glm::vec3(-temp_position.x, -temp_position.y, temp_position.z));
            if (temp_position.y != 0 && temp_position.z != 0) p_positions.push_back(glm::vec3(temp_position.x, -temp_position.y, -temp_position.z));
            if (temp_position.x != 0 && temp_position.z != 0) p_positions.push_back(glm::vec3(-temp_position.x, temp_position.y, -temp_position.z));
            if (temp_position.x != 0 && temp_position.y != 0 && temp_position.z != 0) p_positions.push_back(glm::vec3(-temp_position.x, -temp_position.y, -temp_position.z));
            break;
        case TotalFrame::SYMMETRY_TYPE::X_AXIS:
            break;
        case TotalFrame::SYMMETRY_TYPE::Y_AXIS:
            break;
        case TotalFrame::SYMMETRY_TYPE::Z_AXIS:
            break;
    }

    positions = p_positions;
}

void Shape::Clear() {
    object = Object();
    type = TotalFrame::SHAPE_TYPE::SHAPE_NONE;
    symmetry_type = TotalFrame::SYMMETRY_TYPE::SYMMETRY_NONE;
    positions = {};
}
