#include "pch.h"
#include "Utils.h"
#include <SDL.h>


/*************************************************************************************************


**************************************************************************************************/
GLint getUniformLocation(GLuint shaderProgram, const std::string& uniformName)
{
    GLint id = glGetUniformLocation(shaderProgram, uniformName.c_str());
    if (id < 0)
    {
        printf("Error getting location of uniform variable '%s'", uniformName.c_str());
        SDL_Quit();
        exit(1);
    }
    else
    {
        printf("%s = %d\n", uniformName.c_str(), id);
    }

    return id;
}

/*************************************************************************************************


**************************************************************************************************/
GLint getAttribLocation(GLuint shaderProgram, const std::string& attribName)
{
    GLint id = glGetAttribLocation(shaderProgram, attribName.c_str());
    if (id < 0)
    {
        printf("Error getting location of attrib variable '%s'", attribName.c_str());
        SDL_Quit();
        exit(1);
    }
    else
    {
        printf("%s = %d\n", attribName.c_str(), id);
    }

    return id;
}
