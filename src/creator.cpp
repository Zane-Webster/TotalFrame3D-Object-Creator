#include "Creator.h"
Creator::Creator(std::string p_objects_path) : objects_path(p_objects_path) {
    
}

void Creator::Save(std::string objects_data) {
    if (!first_save) {
        Creator::_InitSave(objects_data);
        first_save = true;
        return;
    }

}

std::string Creator::Load() {
    const char* file_path = tinyfd_openFileDialog("Select a TotalFrame Object", "", 1, filter_patterns, "TotalFrame Object File", 0);
    return file_path;
}

void Creator::Backup() {
    
}

void Creator::ChooseColor() {
    unsigned char temp_color[3] = {255, 255, 255};

    const char* picked_color = tinyfd_colorChooser("Pick a Color", NULL, temp_color, temp_color);

    // convert from (0->255) to (0.0f->1.0f)
    for (int i = 0; i < 3; i++) {
        color[i] = float(temp_color[i]) / 255.0f;
    }

    // alpha channel
    color[3] = 1.0f;
}

void Creator::_InitSave(std::string objects_data) {
    const char* save_path = tinyfd_saveFileDialog("Save TotalFrame Object", "", 1, filter_patterns, "TotalFrame Object File");
}