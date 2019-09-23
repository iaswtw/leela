#include "pch.h"

#include "GlslProgram.h"
#include <string>
#include <fstream>
#include <exception>
#include <vector>

GlslProgram::GlslProgram()
{

}

GlslProgram::~GlslProgram()
{

}

/*************************************************************************************************


**************************************************************************************************/
void GlslProgram::printShaderCompileStatus(GLuint shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	printf("Shader compile status = %d\n", status);
	if (status == GL_TRUE)
	{
		printf("Compile successful.\n\n");
	}
	else
	{
		printf("!!! === Compile failed === !!!\n");

		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		printf("Log:\n%s\n", buffer);

		throw std::exception("shader failed to compile.");
	}
}

void GlslProgram::_readFile(std::string& filePath, std::string& fileContents)
{
    std::string line;

    std::ifstream shaderFile(filePath);
    if (shaderFile.fail())
    {
        perror(filePath.c_str());
        printf("Failed to open %s\n", filePath.c_str());
		throw std::exception("Could not open shader file");
    }
    while (std::getline(shaderFile, line))
    {
        fileContents += line + "\n";
    }
}

void GlslProgram::_compileShader(const char* shaderText, GLuint& shaderId)
{
	glShaderSource(shaderId, 1, &shaderText, nullptr);
	glCompileShader(shaderId);

	// Check compile status
	printShaderCompileStatus(shaderId);
}

void GlslProgram::compileShaders(const char* vertShaderText, const char* fragShaderText)
{
	vertShaderId = glCreateShader(GL_VERTEX_SHADER);
	_compileShader(vertShaderText, vertShaderId);

	fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	_compileShader(fragShaderText, fragShaderId);
}

void GlslProgram::compileShadersFromFile(std::string& vertShaderFilename, std::string& fragShaderFilename)
{
    std::string vertFileContents;
	_readFile(vertShaderFilename, vertFileContents);

	std::string fragFileContents;
	_readFile(fragShaderFilename, fragFileContents);

	compileShaders(vertFileContents.c_str(), fragFileContents.c_str());
}

void GlslProgram::link()
{
	shaderProgramId = glCreateProgram();

	glAttachShader(shaderProgramId, vertShaderId);
	glAttachShader(shaderProgramId, fragShaderId);

	glLinkProgram(shaderProgramId);

	GLint isLinked = 0;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, (int*) &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgramId, maxLength, &maxLength, &infoLog[0]);

		printf("Linker error log:\n");
		printf("%s\n", infoLog.data());

		// We don't need the program anymore.
		glDeleteProgram(shaderProgramId);

		// Don't leak shaders either.
		glDeleteShader(vertShaderId);
		glDeleteShader(fragShaderId);

		throw std::exception("shader program failed to link");
	}
	else
	{
		printf("\nShader program link successful\n");
		glDetachShader(shaderProgramId, vertShaderId);
		glDetachShader(shaderProgramId, fragShaderId);

		// Don't leak shaders either.
		glDeleteShader(vertShaderId);
		glDeleteShader(fragShaderId);
	}
}

void GlslProgram::use()
{
	glUseProgram(shaderProgramId);
}

void GlslProgram::unuse()
{
	glUseProgram(0);
}

void GlslProgram::setBool(const std::string& uniformName, bool value)
{
	glUniform1i(
		glGetUniformLocation(shaderProgramId, uniformName.c_str()),
		(int)value
	);
}

void GlslProgram::setInt(const std::string& uniformName, int value)
{
	glUniform1i(
		glGetUniformLocation(shaderProgramId, uniformName.c_str()),
		value
	);
}

void GlslProgram::setUint(const std::string& uniformName, unsigned int value)
{
	glUniform1ui(
		glGetUniformLocation(shaderProgramId, uniformName.c_str()),
		value
	);
}

void GlslProgram::setFloat(const std::string& uniformName, float value)
{
	glUniform1f(
		glGetUniformLocation(shaderProgramId, uniformName.c_str()),
		value
	);
}

void GlslProgram::setVec3(const std::string& uniformName, const float* value)
{
	glUniform3fv(
		glGetUniformLocation(shaderProgramId, uniformName.c_str()),
		1,
		value
	);
}


void GlslProgram::setMat4(const std::string& uniformName, const float* value)
{
	glUniformMatrix4fv(
		glGetUniformLocation(shaderProgramId, uniformName.c_str()),
		1,
		GL_FALSE,
		value
	);
}
