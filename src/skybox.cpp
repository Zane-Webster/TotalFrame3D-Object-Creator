#include <stb/stb_image.h>

#include "Skybox.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

Skybox::Skybox(std::string skybox_folder_path, GLuint p_shader_program) : shader_program(p_shader_program) {
    Skybox::_LoadPaths(skybox_folder_path);
    Skybox::_CreateVertices();
    Skybox::_LoadCubeMap();
    Skybox::Build();
}

//=============================
// BASIC FUNCTIONS
//=============================

void Skybox::Render(const glm::mat4& view, const glm::mat4& projection) {
    glDepthFunc(GL_LEQUAL);  // Make sure skybox passes depth test

    glUseProgram(shader_program);

    glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view_no_translation));
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(vertex_array);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);  // Reset to default
}

void Skybox::Build() {
    // generate array and buffer
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);

    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

//=============================
// MEMORY MANAGEMENT
//=============================

void Skybox::FreeAll() {
    glDeleteVertexArrays(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteTextures(1, &cubemap_texture);   
}

//=============================
// PRIVATE FUNCTIONS
//=============================
void Skybox::_LoadPaths(std::string skybox_folder_path) {
    // add last slash to go into the folder dir
    if (!skybox_folder_path.empty() && skybox_folder_path.back() != '/' && skybox_folder_path.back() != '\\') skybox_folder_path += '/';
    
    const TF_SKYBOX_PATHS face_filenames = {
        "right.png",   // +X
        "left.png",    // -X
        "top.png",     // +Y
        "bottom.png",  // -Y
        "front.png",   // +Z
        "back.png"     // -Z
    };

    // for each image, add path to face_paths
    for (size_t i = 0; i < face_filenames.size(); ++i) {
        std::string full_path = skybox_folder_path + face_filenames[i];

        // if the full path does not exist, something has gone wrong, return
        if (!std::filesystem::exists(full_path)) {
            Util::ThrowError("FAILED TO LOAD SKYBOX TEXTURE: " + full_path, "Skybox::_LoadPaths");
            return;
        }

        faces_paths[i] = full_path;
    }
}

void Skybox::_LoadCubeMap() {
    // generate texture
    glGenTextures(1, &cubemap_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // don't flip

    // for each path, load the texture
    for (GLuint i = 0; i < faces_paths.size(); i++) {
        unsigned char* data = stbi_load(faces_paths[i].c_str(), &width, &height, &nrChannels, 0);

        // if data is invalid, break and throw error
        if (!data) {
            Util::ThrowError("FAILED TO LOAD TEXTURE: " + faces_paths[i], "Skybox::_LoadCubeMap");
            continue;
        }

        // set color format based on image properties
        GLenum color_format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // load each texture
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, GL_RGB, width, height, 0, color_format, GL_UNSIGNED_BYTE, data
        );

        stbi_image_free(data);
    }

    // set paremeters for cube map
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::_CreateVertices() {
    vertices = {        
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
}