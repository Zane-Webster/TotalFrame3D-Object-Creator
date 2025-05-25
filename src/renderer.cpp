#include "Renderer.h"

Renderer::Renderer() {
    ;
}

void Renderer::Add(const Skybox& item, int layer) {
    skyboxes.push_back({item, layer});
}

void Renderer::Add(const Object& item, int layer) {
    objects.push_back({item, layer});
}

void Renderer::Add(const Texture& item, int layer) {
    textures.push_back({item, layer});
}

void Renderer::Clear() {
    skyboxes.clear();
    objects.clear();
    textures.clear();
}

std::vector<GLuint> Renderer::GetShaderProgramsUpdates() {
    std::vector<GLuint> shader_programs = {};
    for (auto& object : objects) {
        for (const auto& object_sp : object.first.GetShaderProgramsUpdates()) {
            shader_programs.push_back(object_sp);
        }
    }
    return shader_programs;
}

void Renderer::RenderAll(glm::mat4 camera_view_matrix, glm::mat4 camera_projection_matrix, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights) {
    // clear render queue
    render_queue.clear();

    // calculate view projeciton matrix for objects
    glm::mat4 camera_view_projection_matrix = camera_projection_matrix * camera_view_matrix;

    // add skyboxes to render queue
    for (auto& [skybox, layer] : skyboxes) {
        render_queue.push_back(TotalFrame::RenderItem{
            layer,
            [&skybox, camera_view_matrix, camera_projection_matrix]() {
                skybox.Render(camera_view_matrix, camera_projection_matrix);
            }
        });
    }

    // add objects to render queue
    for (auto& [object, layer] : objects) {
        render_queue.push_back(TotalFrame::RenderItem{
            layer,
            [&object, camera_view_projection_matrix, camera_position, lights]() {
                object.UpdateAndRenderAll(camera_view_projection_matrix, camera_position, lights);
            }
        });
    }

    // add objects to render queue
    for (auto& [texture, layer] : textures) {
        render_queue.push_back(TotalFrame::RenderItem{
            layer,
            [&texture]() {
                texture.Render();
            }
        });
    }

    // sort by layer
    std::sort(render_queue.begin(), render_queue.end(), [](const auto& a, const auto& b) {
        // if -1, then it should be rendered last
        if (a.layer == -1) return false;
        if (b.layer == -1) return true;

        // otherwise, sort in ascending order
        return a.layer < b.layer;
    });

    // render all items
    for (const auto& item : render_queue) {
        item.RenderFunction();
    }
}