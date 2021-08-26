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
