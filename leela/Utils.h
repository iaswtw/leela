#pragma once

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include <vector>
#include <string>

GLint getUniformLocation(GLuint shaderProgram, const std::string& uniformName);
GLint getAttribLocation(GLuint shaderProgram, const std::string& attribName);

inline void vector_push_back_7(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    v.push_back(f1);
    v.push_back(f2);
    v.push_back(f3);
    v.push_back(f4);
    v.push_back(f5);
    v.push_back(f6);
    v.push_back(f7);
}

inline void vector_push_back_10(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10)
{
    v.push_back(f1);
    v.push_back(f2);
    v.push_back(f3);
    v.push_back(f4);
    v.push_back(f5);
    v.push_back(f6);
    v.push_back(f7);
    v.push_back(f8);
    v.push_back(f9);
    v.push_back(f10);
}

inline void vector_push_back_12(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11, float f12)
{
    v.push_back(f1);
    v.push_back(f2);
    v.push_back(f3);
    v.push_back(f4);
    v.push_back(f5);
    v.push_back(f6);
    v.push_back(f7);
    v.push_back(f8);
    v.push_back(f9);
    v.push_back(f10);
    v.push_back(f11);
    v.push_back(f12);
}
