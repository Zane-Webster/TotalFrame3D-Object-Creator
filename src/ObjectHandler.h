#ifndef SRC_OBJECTHANDLER_H_
#define SRC_OBJECTHANDLER_H_

#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include <string>

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "TotalFrame.h"
#include "Util.h"
#include "Object.h"
#include "Shape.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/ext.hpp"

/*
ABOUT:
Handles the creation, updating and rendering of Objects.

NOTES:
You can create Objects directly using object_handler.Create() (preferred method).
Ensure you link the CameraHandler's view_projection_matrix to ObjectHandler
Ensure you call UpdateAndRenderAll(), along with CameraHandler::UpdateShaderPrograms(ObjectHandler::GetShaderProgramsUpdates()) for proper results in rendering and handling Objects.
*/

class ObjectHandler {
    public:
        ObjectHandler(float aspect_ratio);
        void FreeAll();

        //////// BASIC
        void UpdateAndRenderAll(glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position);
        void UpdateAndRender(Object object, glm::mat4 camera_view_projection_matrix, glm::vec3 camera_position);

        std::string GetData();
        std::string GetTrueData();

        //////// OBJECT CREATION
        // creates an object
        void Create(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program, std::string object_data_str = "");
        void ClearAndCreate(std::string name, glm::vec3 position, TotalFrame::OBJECT_TYPE type, float size, std::string obj_path, GLuint shader_program);
        // adds a pre-created object
        void Add(Object object);

        void CreateShape(Shape shape);


        //////// OBJECT DESTRUCTION
        void Destory(Object* object);

        //////// SHADER PROGRAMS
        // updates a shader program if it needs updated
        void UpdateSP(Object Object, bool is_visible);
        // returns all shader programs that need updated
        std::vector<GLuint> GetShaderProgramsUpdates();

        //////// CAMERA SCALING
        void UpdateObjectCameraScale(Object object, glm::vec3 camera_position, bool is_visible);

        //////// RAYS
        std::shared_ptr<Object> GetRayCollidingObject(TotalFrame::Ray ray);
        Object GetRayCollidingObjectWithFace(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out);
        Object* GetRayCollidingObjectWithFacePtr(TotalFrame::Ray ray, glm::vec3& face_hit_normal_out);
        std::vector<std::shared_ptr<Object>> GetRayCollidingObjects(TotalFrame::Ray ray);

        //////// RENDERING
        // renders an object if it is in view
        void Render(Object object, bool is_visible);

    private:
        //////// FILE READING FUNCTIONS
        std::string _ReadData(std::string path);
        std::vector<std::string> _SplitByObject(std::string object_data);

        //////// BASIC ATTRIBUTES
        std::vector<Object> objects = {};
        // groups objects by which shader program they use
        std::unordered_map<GLuint, std::vector<Object>> shader_program_groups = {};
        // says which shader_programs need to be updated
        std::unordered_map<GLuint, bool> shader_programs_need_update = {};

        float aspect_ratio = 1.778f;

};

#endif // SRC_OBJECTHANDLER_H_