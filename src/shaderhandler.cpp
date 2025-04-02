#include "ShaderHandler.h"

//=============================
// DEFAULT CONSTRUCTOR
//=============================

ShaderHandler::ShaderHandler(SDL_GLContext p_context) : context(p_context) {
    // place in SAME order as shader_exts and shader_types
    shaders_sources.push_back(std::make_shared<std::unordered_map<GLenum, std::string>>(vert_shaders_sources));
    shaders_sources.push_back(std::make_shared<std::unordered_map<GLenum, std::string>>(frag_shaders_sources));
    shaders_sources.push_back(std::make_shared<std::unordered_map<GLenum, std::string>>(geom_shaders_sources));
}

//=============================
// GENERAL FUNCTIONS
//=============================

GLuint ShaderHandler::CreateShaderProgram(std::string dir_path) {
    ShaderHandler::_ReadShaderSourceFolder(dir_path);

    GLuint shader_program = glCreateProgram();

    // for each type of shader
    for (const auto& shader_type_sources : shaders_sources) {
        // for each individual shader
        for (const auto& [type,content] : *shader_type_sources) {
            GLuint temp_shader = _CompileShader(type, content);
            // attach shader to shader program
            glAttachShader(shader_program, temp_shader);
            glDeleteShader(temp_shader);
            temp_shader = 0;
        }
    }

    // link shader program to gl context
    glLinkProgram(shader_program);

    // see if successfuly linked, if not return error
    GLint successfully_linked;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &successfully_linked);
    if (!successfully_linked) Util::ThrowError("ERROR LINKING SHADER", "ShaderHandler::CreateShaderProgram");

    shader_programs.push_back(shader_program);

    ShaderHandler::_ClearShaderSources();

    return shader_program;
}

//=============================
// PRIVATE FUNCTIONS
//=============================

void ShaderHandler::_ReadShaderSourceFolder(std::string dir_path) {
    // iterate over shader dir path 
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        // if entry is dir, which it should be
        if (std::filesystem::is_directory(entry.path())) {
            // iterate over dir
            for (const auto& shader : std::filesystem::directory_iterator(entry.path())) {
                // iterate over list of valid extensions
                for (int i = 0; i < shader_exts.size(); i++) {
                    // if shader extension matches valid extensions list
                    if (shader.path().extension().string() == shader_exts[i]) {
                        // open shader source file
                        std::ifstream shader_source_file(shader.path());
                        // if errorw ith the file, continue
                        if (!shader_source_file.is_open()) {
                            Util::ThrowError("INVALID SHADER SOURCE FILE", "ShaderHandler::readShaderSourceFolder");
                            continue;
                        }
                        // gather content from file into string
                        std::string shader_source_content((std::istreambuf_iterator<char>(shader_source_file)), (std::istreambuf_iterator<char>()));
                        // insert shader into proper map, with the key being the shader name, and value being the shader source content
                        (*shaders_sources[i])[shader_types[i]] = shader_source_content;

                        // close the file
                        shader_source_file.close();
                    }
                }
            }
        }
    }
}

GLuint ShaderHandler::_CompileShader(GLenum type, std::string source) {
    // create shader to be compiled
    GLuint shader = glCreateShader(type);
    // convert source to char
    const char* source_to_char = source.c_str();
    // add source to shader
    glShaderSource(shader, 1, &source_to_char, nullptr);
    // compile shader
    glCompileShader(shader);

    // see if successfuly compiled, if not return error
    GLint successfully_compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &successfully_compiled);
    if (!successfully_compiled) Util::ThrowError("ERROR COMPILING SHADER", "ShaderHandler::CompileShader");

    return shader;
}

void ShaderHandler::_ClearShaderSources() {
    vert_shaders_sources = {};
    frag_shaders_sources = {};
    geom_shaders_sources = {};
    shaders_sources = {};
}

//=============================
// MEMORY MANAGEMENT
//=============================

void ShaderHandler::FreeAll() {
    for (const auto& shader_program : shader_programs) {
        glDeleteProgram(shader_program);
    }
}