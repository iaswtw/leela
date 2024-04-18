#include "StarsRenderer.h"
#include "Stars.h"
#include <vector>
#include <tuple>
#include <GL/glew.h>
#include "SphericalBody.h"
#include "Universe.h"

static inline void vector_push_back_7(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
    v.push_back(f1);
    v.push_back(f2);
    v.push_back(f3);
    v.push_back(f4);
    v.push_back(f5);
    v.push_back(f6);
    v.push_back(f7);
}

StarsRenderer::StarsRenderer(Stars& stars)
    : _stars(stars)
{

}

StarsRenderer::~StarsRenderer()
{

}

void StarsRenderer::init()
{
    constructVerticesAndSendToGpu();
}

void StarsRenderer::_renderStars(GlslProgram& glslProgram)
{
    if (!g_universe->bGalaxyStars)
    {
        renderCubeStars(glslProgram);
    }
    else
    {
        renderGalaxyStars(glslProgram);
    }
}

void StarsRenderer::renderMain(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_Star)
    {
        _renderStars(glslProgram);
    }
}

std::tuple<std::vector<float>*, std::vector<float>*> StarsRenderer::_constructCubeStars()
{
    std::vector<float>* v1 = new std::vector<float>();      // for single pixel stars
    std::vector<float>* v2 = new std::vector<float>();      // for double pixel stars

    //---------------------------------------------------------------------
    // generate cubically distributed stars
    //---------------------------------------------------------------------
    float i = 0.0f;
    float x, y, z, c, r, g, b;
    float max_dist = 500000.0;
    printf("Starting to generate stars\n");
    while (1) {
        if (i == _stars.maxCubeStars)
            break;

        x = float(max_dist * (double(rand()) / RAND_MAX) - (max_dist / 2));
        y = float(max_dist * (double(rand()) / RAND_MAX) - (max_dist / 2));
        z = float(max_dist * (double(rand()) / RAND_MAX) - (max_dist / 2));
        c = 0.1f + (0.8f * ((float(rand()) / RAND_MAX)));

        r = g = b = c;

        // 1 out of 10 should be reddish, 1 out of 10 should be blueish, and 1 out of 10 should be yellowish.
        int anotherRandom = int(10 * (float(rand()) / RAND_MAX));
        if (anotherRandom == 5)
            r *= 1.5f;
        if (anotherRandom == 6)
            b *= 1.2f;
        if (anotherRandom == 7)
        {
            r *= 1.5f;
            g *= 1.5f;
        }
        
        // Discard starts too close to the solar system
        if (fabs(x) > 50000 || fabs(y) > 50000 || fabs(z) > 50000)
        {
            std::vector<float>* v = (i < _stars.singlePixelStarsRatio * _stars.maxCubeStars) ? v1 : v2;

            v->push_back(x);  v->push_back(y);  v->push_back(z);   v->push_back(r);  v->push_back(g);  v->push_back(b);  v->push_back(1.0);
            i++;
        }
    }

    return { v1, v2 };
}


std::tuple<std::vector<float>*, std::vector<float>*> StarsRenderer::_constructGalaxyStars()
{
    GLuint vbo;
    std::vector<float>* v1 = new std::vector<float>();      // for single pixel stars
    std::vector<float>* v2 = new std::vector<float>();      // for double pixel stars


    ////---------------------------------------------------------------------
    //// generate stars for the galaxy
    ////---------------------------------------------------------------------
    //max_radius = 1000000.0;
    //max_theta = 2 * M_PI;
    //i = 0;
    //while (1) {
    //    if (i == .7 * MAXGALAXYSTARS)
    //        break;

    //    radius = max_radius * (double(rand()) / RAND_MAX);
    //    theta = max_theta * (double(rand()) / RAND_MAX);
    //    w = radius / max_radius;
    //    if (radius < 40000)
    //        max_z = radius;
    //    else
    //        max_z = 400000 * (1 / sqrt(1 + 20 * w * w));

    //    gstar[i].x = radius * cos(theta);
    //    gstar[i].y = radius * sin(theta);
    //    gstar[i].z = max_z * double(rand()) / RAND_MAX - max_z / 2;
    //    color = static_cast<unsigned char>(55) + static_cast<unsigned char>((double(rand()) / RAND_MAX) * 200);
    //    //c = SDL_MapRGB ( space.surface->format, color, color, color );
    //    gstar[i].set_color(color, color, color);

    //    if (fabs(gstar[i].x) > 1000 ||
    //        fabs(gstar[i].y) > 1000 ||
    //        fabs(gstar[i].z) > 1000)
    //        i++;
    //}

    //while (1) {
    //    if (i == MAXGALAXYSTARS)
    //        break;

    //    radius = 200000 * (double(rand()) / RAND_MAX);
    //    theta = M_PI * (double(rand()) / RAND_MAX);
    //    phi = 2 * M_PI * (double(rand()) / RAND_MAX);
    //    gstar[i].x = radius * sin(theta) * cos(phi);
    //    gstar[i].y = radius * sin(theta) * sin(phi);
    //    gstar[i].z = radius * cos(theta);
    //    color = static_cast<unsigned char>((double(rand()) / RAND_MAX) * 255);
    //    //gstar[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
    //    gstar[i].set_color(color, color, color);
    //    i++;

    //}

    //// shift the Galaxy stars along x axis
    //for (i = 0; i < MAXGALAXYSTARS; i++)
    //    gstar[i].x += max_radius * .66;


    //// rotate all stars so that the the plane of the solar system
    //// is perpendicular to the plane of the galaxy
    //for (i = 0; i < MAXGALAXYSTARS; i++)
    //    gstar[i] = space.rotate(PNT(0, 0, 0), PNT(100, 0, 0), gstar[i], 80.0);


    //for (i = 0; i < MAXGALAXYSTARS; i++)
    //    gstarVertices.push_back(gstar[i].x);  gstarVertices.push_back(gstar[i].y);  gstarVertices.push_back(gstar[i].z);   gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(1.0);



    return { v1, v2 };
}


void StarsRenderer::constructVerticesAndSendToGpu()
{
    GLuint vbo;
    std::vector<float>* v = nullptr;

    auto [singlePixelStars, doublePixelStars] = _constructCubeStars();


    //---------------------------------------------------------------------------------------------------
    // Cube stars - 1 pixel
    glGenVertexArrays(1, &cubeStarsSinglePixelVao);
    glBindVertexArray(cubeStarsSinglePixelVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * singlePixelStars->size(),
        singlePixelStars->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numCubeStarsSinglePixelVertices = singlePixelStars->size() / VERTEX_STRIDE_IN_VBO;

    //---------------------------------------------------------------------------------------------------
    // Cube stars - 2 pixel
    glGenVertexArrays(1, &cubeStarsDoublePixelVao);
    glBindVertexArray(cubeStarsDoublePixelVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * doublePixelStars->size(),
        doublePixelStars->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numCubeStarsDoublePixelVertices = doublePixelStars->size() / VERTEX_STRIDE_IN_VBO;

    //---------------------------------------------------------------------------------------------------
    // galaxy stars
    //glGenVertexArrays(1, &oglHandles.gstarsVao);
    //glBindVertexArray(oglHandles.gstarsVao);

    //glGenBuffers(1, &vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(
    //    GL_ARRAY_BUFFER,
    //    sizeof(float) * gstarVertices.size(),
    //    gstarVertices.data(),
    //    GL_STATIC_DRAW);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
    //glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

        
    //numGalaxyStarVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

}

void StarsRenderer::renderCubeStars(GlslProgram& glslProgram)
{
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 1.0f);

    //----------------------------------------------
    // Cubic stars model transformation
    //----------------------------------------------
    glslProgram.setMat4("model", glm::value_ptr(glm::mat4(1.0)));

    glslProgram.setUint("starPointSize", 1);
    glBindVertexArray(cubeStarsSinglePixelVao);
    // Draw vertices
    glDrawArrays(GL_POINTS, 0, numCubeStarsSinglePixelVertices);

    glslProgram.setUint("starPointSize", 2);
    glBindVertexArray(cubeStarsDoublePixelVao);
    glDrawArrays(GL_POINTS, 0, numCubeStarsDoublePixelVertices);


    glDisable(GL_PROGRAM_POINT_SIZE);
}

void StarsRenderer::renderGalaxyStars(GlslProgram& glslProgram)
{
    glEnable(GL_PROGRAM_POINT_SIZE);
    //----------------------------------------------
    // Galaxy stars model transformation
    //----------------------------------------------
    glslProgram.setMat4("model", glm::value_ptr(glm::mat4(1.0)));

    glslProgram.setUint("starPointSize", 1);
    glBindVertexArray(galaxyStarsSinglePixelVao);
    // Draw vertices
    glDrawArrays(GL_POINTS, 0, numGalaxyStarsSinglePixelVertices);

    glBindVertexArray(galaxyStarsDoublePixelVao);
    glslProgram.setUint("starPointSize", 2);
    glDrawArrays(GL_POINTS, 0, numGalaxyStarsDoublePixelVertices);


    glDisable(GL_PROGRAM_POINT_SIZE);

}