#include "Creator.h"
Creator::Creator(std::string p_objects_path) : objects_path(p_objects_path) {

}

void Creator::Save(std::string object_data) {
    // if untitled (not yet saved), create a new object. if the user exits, cancel save and return
    if (*object_name == "untitled") if (!Creator::NewObject()) return;

    std::ofstream out_file(object_path, std::ios::out | std::ios::trunc);

    if (!out_file) {
        Util::ThrowError("FAILED TO OPEN FILE", "Creator::Save");
        return;
    }

    out_file << object_data;

    out_file.close();
}

bool Creator::NewObject() {
    const char * temp_path = tinyfd_saveFileDialog("Save TotalFrame Object", ".tfobj", 1, filter_patterns, "TotalFrame Object File *.tfobj");
    if (temp_path == NULL) {
        return false;
    }

    object_path = temp_path;

    std::filesystem::path temp_fs_path = object_path;
    *object_name = temp_fs_path.filename().string();

    return true;
}

std::string Creator::Load() {
    const char* temp_path = tinyfd_openFileDialog("Load TotalFrame Object", objects_path.c_str(), 1, filter_patterns, "TotalFrame Object File *.tfobj", 0);
    if (temp_path == NULL) {
        return "\n";
    }

    object_path = temp_path;

    std::filesystem::path temp_fs_path = object_path;
    *object_name = temp_fs_path.filename().string();

    return temp_path;
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

std::shared_ptr<std::string> Creator::GetName() {
    return object_name;
}