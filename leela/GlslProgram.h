#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>



enum class GlslProgramType
{
	None,
	Font,
	Planet,
	Star,
	Sun,
	Simple,
	SimpleOrtho,
	BookmarkSphere,

};


class GlslProgram
{
public:
	GlslProgram(GlslProgramType type = GlslProgramType::None);
	~GlslProgram();
	void printShaderCompileStatus(GLuint shader);
	void compileShadersFromFile(const char * vertShaderFilenames, const char * fragShaderFilename);
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

	GlslProgramType type() { return _type;  }

private:
    void _readFile(const char * fileName, std::string& fileContents);
    void _compileShader(const char* shaderText, GLuint& shaderId);

    const char* _vertShaderText = nullptr;
    const char* _fragShaderText = nullptr;

    GLuint vertShaderId		= 0;
    GLuint geomShaderId		= 0;
    GLuint fragShaderId		= 0;
	GLuint shaderProgramId	= 0;

	GlslProgramType _type;
};

