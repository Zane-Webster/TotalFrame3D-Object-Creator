#include "BlockCursor.h"

BlockCursor::BlockCursor(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program, float aspect_ratio, std::string object_data_str) {
    object.Create(name, position, type, size, obj_path, shader_program, aspect_ratio, object_data_str);
}

void BlockCursor::PlaceOnFace(glm::vec3 p_object_position, glm::vec3 face_pos) {
    // if not looking at an object, reset fully, visibility = false and return
    if (face_pos == glm::vec3(-1000.0f)) {
        current_translation = glm::vec3(0.0f);
        object.ResetTranslation();
        visible = false;
        return;
    }

    // calculate the new translation based on the face looking at
    glm::vec3 new_translation = face_pos * (object.true_size * 2.0f);
    new_translation += p_object_position;

    // if the face is not the same face the user is already looking it, reset current transformation and set the new one
    if (new_translation != current_translation) {
        object.ResetTranslation();
        object.Translate(new_translation);
        current_translation = new_translation;
    }
    
    visible = true;
}

glm::vec3 BlockCursor::NextObjectPosition() {
    return object.GetTTPosition();
}