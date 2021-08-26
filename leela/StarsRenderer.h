#pragma once

#include "GlslProgram.h"
#include <GL/glew.h>
#include <vector>
#include "Stars.h"

class StarsRenderer
{
public:
    StarsRenderer(Stars& stars);
    ~StarsRenderer();

    void constructVerticesAndSendToGpu();
    void renderCubeStars(GlslProgram& glslProgram);
    void renderGalaxyStars(GlslProgram& glslProgram);

private:
    std::tuple<std::vector<float>*, std::vector<float>*> _constructCubeStars();
    std::tuple<std::vector<float>*, std::vector<float>*> _constructGalaxyStars();


private:
    GLuint cubeStarsSinglePixelVao = 0;
    GLuint cubeStarsDoublePixelVao = 0;
    GLuint galaxyStarsSinglePixelVao = 0;
    GLuint galaxyStarsDoublePixelVao = 0;

    int numCubeStarsSinglePixelVertices = 0;
    int numCubeStarsDoublePixelVertices = 0;
    int numGalaxyStarsSinglePixelVertices = 0;
    int numGalaxyStarsDoublePixelVertices = 0;

    

    Stars& _stars;
};

