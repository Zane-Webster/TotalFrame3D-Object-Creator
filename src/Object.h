#ifndef SRC_OBJECT_H_
#define SRC_OBJECT_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <map>

#include <string>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Cube.h"
#include "Shape.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

/*
ABOUT:
Handles the creation, updating and rendering of Cubes

NOTES:
You can create cubes directly using object.Create() (preferred method).
Ensure you link the CameraHandler's view_projection_matrix to cube
Ensure you call UpdateAndRenderAll(), along with CameraHandler::UpdateShaderPrograms(GetShaderProgramsUpdates()) for proper results in rendering and handling Objects.
*/

class Object {
    public:
        Object(TotalFrame::OBJECT_TYPE type, float aspect_ratio);
        void FreeAll();

        //////// BASIC ATTRIBUTES
        glm::vec3 position = glm::vec3(0.0f);
        TotalFrame::OBJECT_TYPE type = TotalFrame::OBJECT_TYPE::CUBE_OBJ;

        //////// BASIC
        void UpdateAndRenderAll(glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights);
        void UpdateAndRender(Cube cube, glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights);

        std::string GetData();
        std::string GetTrueData();

        //////// EXPORTATION
        std::string GetExportData();

        //////// CUBE CREATION
        // creates a cube
        void Create(std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program, std::string object_data_str = "");
        void CreateLight(std::shared_ptr<TotalFrame::Light> light, std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program, std::string object_data_str = "");
        void ClearAndCreate(std::string name, glm::vec3 position, float size, std::string obj_path, GLuint shader_program);
        // adds a pre-created cube
        void Add(Cube cube);

        void CreateShape(Shape shape);

        //////// CUBE DESTRUCTION
        void Destory(Cube* cube);

        //////// TRANSLATION
        void Rotate(glm::vec3 rotation, glm::vec3 camera_position);
        void Translate(glm::vec3 translation);

        //////// SHADER PROGRAMS
        // updates a shader program if it needs updated
        void UpdateSP(Cube cube, bool is_visible);
        // returns all shader programs that need updated
        std::vector<GLuint> GetShaderProgramsUpdates();

        //////// CAMERA SCALING
        void UpdateCubeCameraScale(Cube cube, glm::vec3 camera_position, bool is_visible);

        //////// RAYS
        std::shared_ptr<Cube> GetRayCollidingCube(TotalFrame::Ray ray);
        Cube GetRayCollidingCubeWithFace(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out);
        Cube* GetRayCollidingCubeWithFacePtr(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out);
        std::vector<std::shared_ptr<Cube>> GetRayCollidingCubes(TotalFrame::Ray ray);

        //////// LIGHTING
        void AttachLight(std::shared_ptr<TotalFrame::Light> light);

        //////// RENDERING
        // renders an cube if it is in view
        void Render(Cube cube, glm::vec3 camera_position, std::vector<std::shared_ptr<TotalFrame::Light>> lights, bool is_visible);

    private:
        //////// FILE READING FUNCTIONS
        std::string _ReadData(std::string path);
        std::vector<std::string> _SplitByCube(std::string cube_data);

        //////// BASIC ATTRIBUTES
        std::vector<Cube> cubes = {};
        // groups cubes by which shader program they use
        std::unordered_map<GLuint, std::vector<Cube>> shader_program_groups = {};
        // says which shader_programs need to be updated
        std::unordered_map<GLuint, bool> shader_programs_need_update = {};

        //////// LIGHTING
        std::shared_ptr<TotalFrame::Light> light = nullptr;

        float aspect_ratio = 1.778f;

};

#endif // SRC_OBJECT_H_