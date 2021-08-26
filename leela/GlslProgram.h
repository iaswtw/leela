#pragma once

#include <GL/glew.h>
#include <string>

class GlslProgram
{
public:
	GlslProgram();
	~GlslProgram();
	void printShaderCompileStatus(GLuint shader);
	void compileShadersFromFile(std::string& vertShaderFilename, std::string& fragShaderFilename);
    void compileShaders(const char* vertShaderText, const char* fragShaderText);
    void link();
    void use();
	void unuse();

	void setBool(const std::string& uniformName, bool value);
	void setInt(const std::string& uniformName, int value);
	void setUint(const std::string& uniformName, unsigned int value);
	void setFloat(const std::string& uniformName, float value);
	void setVec3(const std::string& uniformName, const float* value);
	void setMat4(const std::string& uniformName, const float* value);


private:
    void _readFile(std::string& filePath, std::string& fileContents);
    void _compileShader(const char* shaderText, GLuint& shaderId);

    const char* _vertShaderText = nullptr;
    const char* _fragShaderText = nullptr;

    GLuint vertShaderId		= 0;
    GLuint geomShaderId		= 0;
    GLuint fragShaderId		= 0;
	GLuint shaderProgramId	= 0;
};