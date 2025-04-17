#include "Creator.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Creator::Creator(std::string p_objects_path, std::string p_exports_path) : objects_path(p_objects_path), exports_path(p_exports_path) {

}

//=============================
// BASIC FUNCTIONS
//=============================

std::shared_ptr<std::string> Creator::GetName() {
    return object_name;
}

//=============================
// CUBE DEFAULT FUNCTIONS
//=============================

void Creator::SetCubeDefault(Cube cube) {
    cube_default = cube;
    adjusted_cube_default = cube;
}

Cube Creator::GetCubeDefault() {
    return adjusted_cube_default;
}

glm::vec3 Creator::GetCubeDefaultPosition() {
    return adjusted_cube_default.GetPosition();
}

void Creator::UpdateCubeDefaultPosition(glm::vec3 position) {
    adjusted_cube_default.SetPosition(position);
}

//=============================
// SHAPE FUNCTIONS
//=============================

Shape Creator::GetShape() {
    shape.SetSymmetryPositions(adjusted_cube_default.GetPosition());
    return shape;
}

void Creator::SetShape(TotalFrame::SHAPE_TYPE type, Cube cube) {
    shape.type = type;
    shape.cube = cube;
    switch (type) {
        case TotalFrame::SHAPE_TYPE::SHAPE_NONE:
            shape.Clear();
            break;
        case TotalFrame::SHAPE_TYPE::SYMMETRY:
            shape.symmetry_type = TotalFrame::SYMMETRY_TYPE::ALL_AXIS;
            shape.SetSymmetryPositions(adjusted_cube_default.GetPosition());
            break;
        case TotalFrame::SHAPE_TYPE::SPHERE:
            break;
        case TotalFrame::SHAPE_TYPE::SQUARE:
            break;
    }
}

void Creator::ToggleSymmetry(Cube cube, TotalFrame::SYMMETRY_TYPE p_symmetry_type) {
    if (shape.type == TotalFrame::SHAPE_TYPE::SYMMETRY) shape.Clear();
    else {
        shape.symmetry_type = p_symmetry_type;
        Creator::SetShape(TotalFrame::SHAPE_TYPE::SYMMETRY, cube);
    }
}

//=============================
// SAVING FUNCTIONS
//=============================

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
    const char * temp_path = tinyfd_saveFileDialog("New TotalFrame Development Object", ".tfobj_dev", 1, filter_patterns, "TotalFrame Development Object File *.tfobj_dev");
    if (temp_path == NULL) {
        return false;
    }

    object_path = temp_path;

    std::filesystem::path temp_fs_path = object_path;
    *object_name = temp_fs_path.filename().string();

    return true;
}

bool Creator::Export(std::string object_data) {
    // if untitled (not yet saved), create a new object. if the user exits, cancel save and return
    if (!Creator::NewExport()) return false;

    std::ofstream out_file(exports_path, std::ios::out | std::ios::trunc);

    if (!out_file) {
        Util::ThrowError("FAILED TO OPEN FILE", "Creator::Export");
        return false;
    }

    out_file << object_data;

    out_file.close();

    return true;
}

bool Creator::NewExport() {
    const char * temp_path = tinyfd_saveFileDialog("New TotalFrame Object", ".tfobj", 1, export_filter_patterns, "TotalFrame Object File *.tfobj");
    if (temp_path == NULL) {
        return false;
    }

    exports_path = temp_path;

    std::filesystem::path temp_fs_path = exports_path;

    return true;
}

//=============================
// LOADING FUNCTIONS
//=============================

std::string Creator::Load() {
    const char* temp_path = tinyfd_openFileDialog("Load TotalFrame Development Object", objects_path.c_str(), 1, filter_patterns, "TotalFrame Development Object File *.tfobj_dev", 0);
    if (temp_path == NULL) {
        return "\n";
    }

    object_path = temp_path;

    std::filesystem::path temp_fs_path = object_path;
    *object_name = temp_fs_path.filename().string();

    return temp_path;
}

//=============================
// COLOR FUNCTIONS
//=============================

void Creator::ChooseColor() {
    unsigned char temp_color[3] = {255, 255, 255};

    const char* picked_color = tinyfd_colorChooser("Pick a Color", NULL, temp_color, temp_color);

    // convert from (0->255) to (0.0f->1.0f)
    for (int i = 0; i < 3; i++) {
        color[i] = float(temp_color[i]) / 255.0f;
    }

    // alpha channel
    color[3] = 1.0f;

    adjusted_cube_default.SetColor(color);
}

void Creator::SetCubeDefaultColor(glm::vec3 p_color) {
    if (p_color != glm::vec3(-1000.0f)) {
        color = glm::vec4(p_color[0], p_color[1], p_color[2], 1.0f);
        cube_default.SetColor(color);
    }
}