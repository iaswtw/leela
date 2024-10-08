#include "SphericalBodyRenderer.h"
#include "Utils.h"
#include "Leela.h"
#include "Elements.h"

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "SphericalBody.h"
#include "Space.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <array>

#include "lodepng.h"
#include "spdlog/spdlog.h"

#include "TessellationHelper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

using TriangleList = std::vector<Triangle>;
using VertexList = std::vector<glm::vec3>;

extern Leela* p_universe;

namespace icosahedron
{
    const float X = .525731112119133606f;
    const float Z = .850650808352039932f;
    const float N = 0.f;

    static const VertexList vertices =
    {
      {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
      {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
      {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
    };

    void populateInitialVertices(VertexList* v)
    {
        v->push_back(glm::vec3(-X, +N, +Z));
        v->push_back(glm::vec3(+X, +N, +Z));
        v->push_back(glm::vec3(-X, +N, -Z));
        v->push_back(glm::vec3(+X, +N, -Z));

        v->push_back(glm::vec3(+N, +Z, +X));
        v->push_back(glm::vec3(+N, +Z, -X));
        v->push_back(glm::vec3(+N, -Z, +X));
        v->push_back(glm::vec3(+N, -Z, -X));

        v->push_back(glm::vec3(+Z, +X, +N));
        v->push_back(glm::vec3(-Z, +X, +N));
        v->push_back(glm::vec3(+Z, -X, +N));
        v->push_back(glm::vec3(-Z, -X, +N));
    }

    static const TriangleList triangles =
    {
      {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
      {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
      {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
      {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
    };

    void populateInitialTriangles(TriangleList* t)
    {
        // Initial 20 faces
        t->push_back(Triangle(0, 4, 1));
        t->push_back(Triangle(0, 9, 4));
        t->push_back(Triangle(9, 5, 4));
        t->push_back(Triangle(4, 5, 8));
        t->push_back(Triangle(4, 8, 1));

        t->push_back(Triangle(8, 10, 1));
        t->push_back(Triangle(8, 3, 10));
        t->push_back(Triangle(5, 3, 8));
        t->push_back(Triangle(5, 2, 3));
        t->push_back(Triangle(2, 7, 3));
        
        t->push_back(Triangle(7, 10, 3));
        t->push_back(Triangle(7, 6, 10));
        t->push_back(Triangle(7, 11, 6));
        t->push_back(Triangle(11, 0, 6));
        t->push_back(Triangle(0, 1, 6));
        
        t->push_back(Triangle(6, 1, 10));
        t->push_back(Triangle(9, 0, 11));
        t->push_back(Triangle(9, 11, 2));
        t->push_back(Triangle(9, 2, 5));
        t->push_back(Triangle(7, 2, 11));

    }

}

using Lookup = std::map<std::pair<unsigned int, unsigned int>, unsigned int>;

unsigned int vertex_for_edge(Lookup& lookup,
    VertexList& vertices, unsigned int first, unsigned int second)
{
    Lookup::key_type key(first, second);
    if (key.first > key.second)
        std::swap(key.first, key.second);

    auto inserted = lookup.insert({ key, vertices.size() });
    if (inserted.second)
    {
        auto& edge0 = vertices[first];
        auto& edge1 = vertices[second];
        auto point = normalize(edge0 + edge1);
        vertices.push_back(point);
    }

    return inserted.first->second;
}

TriangleList * subdivide(VertexList& vertices, TriangleList* triangles)
{
    Lookup lookup;
    TriangleList *result = new TriangleList();

    for (auto&& each : *triangles)
    {
        std::array<unsigned int, 3> mid;
        for (int edge = 0; edge < 3; ++edge)
        {
            mid[edge] = vertex_for_edge(lookup, vertices,
                each.vertex[edge], each.vertex[(edge + 1) % 3]);
        }

        result->push_back({ each.vertex[0], mid[0], mid[2] });
        result->push_back({ each.vertex[1], mid[1], mid[0] });
        result->push_back({ each.vertex[2], mid[2], mid[1] });
        result->push_back({ mid[0], mid[1], mid[2] });
    }

    delete triangles;
    return result;
}


using IndexedMesh = std::pair<VertexList*, TriangleList*>;

IndexedMesh make_icosphere(int subdivisions)
{
    VertexList* vertices = new VertexList();
    TriangleList* triangles = new TriangleList();

    icosahedron::populateInitialVertices(vertices);
    icosahedron::populateInitialTriangles(triangles);

    for (int i = 0; i < subdivisions; ++i)
    {
        triangles = subdivide(*vertices, triangles);
    }

    return{ vertices, triangles };
}


/////////////////////////////////////////////////////////////////////////////////
//// find middle point of 2 vertices
//// NOTE: new vertex must be resized, so the length is equal to the radius
/////////////////////////////////////////////////////////////////////////////////
//void computeHalfVertex(const float v1[3], const float v2[3], float newV[3])
//{
//    newV[0] = v1[0] + v2[0];    // x
//    newV[1] = v1[1] + v2[1];    // y
//    newV[2] = v1[2] + v2[2];    // z
//    float scale = radius / sqrtf(newV[0] * newV[0] + newV[1] * newV[1] + newV[2] * newV[2]);
//    newV[0] *= scale;
//    newV[1] *= scale;
//    newV[2] *= scale;
//}







SphericalBodyRenderer::SphericalBodyRenderer(std::string textureFilename, std::string textureFilename2)
{
    //setNightColorDarkness(NightColorDarkness_Black);
    //setNightColorDarkness(NightColorDarkness_VeryLow);
    _textureFilename = textureFilename;
    _textureFilename2 = textureFilename2;
    //printf("Setting _textureFilename to %s\n", _textureFilename.c_str());
    
    setNightColorDarkness(NightColorDarkness_VeryHigh);

    _componentName = "SphericalBodyRenderer";
}

SphericalBodyRenderer::~SphericalBodyRenderer()
{
}

void SphericalBodyRenderer::init()
{
    constructVerticesAndSendToGpu();
}

void SphericalBodyRenderer::parentChanged()
{
    _sphere = dynamic_cast<SphericalBody*>(_sceneParent);
}

void SphericalBodyRenderer::setAsLightSource()
{
    _bIsLightSource = True;
}

void SphericalBodyRenderer::setNightColorDarkness(NightColorDarkness darkness)
{
    _nightColorDarkness     = darkness;
    _nightColorMultiplier   = nightColorDarknessLevelToFloat(_nightColorDarkness);
}

float SphericalBodyRenderer::nightColorDarknessLevelToFloat(NightColorDarkness level)
{
    float floatValue = 0.0f;
    switch (level)
    {
        case NightColorDarkness_Black:      floatValue = 0.0f;   break;
        case NightColorDarkness_VeryHigh:   floatValue = 0.03f;  break;
        case NightColorDarkness_High:       floatValue = 0.07f;  break;
        case NightColorDarkness_Medium:     floatValue = 0.15f;   break;
        case NightColorDarkness_Low:        floatValue = 0.3f;   break;
        case NightColorDarkness_VeryLow:    floatValue = 0.7f;   break;
        case NightColorDarkness_None:       floatValue = 1.0f;   break;
    }

    floatValue = pow(floatValue, 1.7);

    return floatValue;
}

NightColorDarkness SphericalBodyRenderer::nightColorDarknessStrToLevel(std::string levelStr)
{
    if (levelStr == "Black") {
        return NightColorDarkness_Black;
    }
    else if (levelStr == "Very High") {
        return NightColorDarkness_VeryHigh;
    }
    else if (levelStr == "High") {
        return NightColorDarkness_High;
    }
    else if (levelStr == "Medium") {
        return NightColorDarkness_Medium;
    }
    else if (levelStr == "Low") {
        return NightColorDarkness_Low;
    }
    else if (levelStr == "Very Low") {
        return NightColorDarkness_VeryLow;
    }
    else if (levelStr == "None") {
        return NightColorDarkness_None;
    }
    else {
        return NightColorDarkness_VeryHigh;     // default to Very High for invalid levelStr
    }
}


void SphericalBodyRenderer::setPolygonCountLevel(std::string polygonCountLevel)
{
    if (polygonCountLevel == "low")
        _polygonCountLevel = PolygonCountLevel_Low;
    else if (polygonCountLevel == "medium")
        _polygonCountLevel = PolygonCountLevel_Medium;
    else if (polygonCountLevel == "high")
        _polygonCountLevel = PolygonCountLevel_High;
}

/*
* 
 * Return a tuple containing information about a point on sphere corresponding to the given radius, alpha, theta arguments.
 *  - x, y, & z of the point on sphere
 *  - Normal unit vector at that point on the sphere
 *  - texture coordinates to use for this point from the 2-D rectangular texture.
 */
std::tuple<float, float, float, glm::vec3, float, float> SphericalBodyRenderer::calcPointOnSphere(float radius, float alpha, float theta)
{
    if (alpha < 0)          alpha = 0.0f;
    if (theta < 0)          theta = 0.0f;
    
    if (alpha > 2 * M_PI)   alpha = float(2 * M_PI);
    if (theta > M_PI)       theta = float(M_PI);

    float x = radius * sin(theta) * cos(alpha);
    float y = radius * sin(theta) * sin(alpha);
    float z = radius * cos(theta);
    glm::vec3 N = glm::normalize(glm::vec3(x, y, z) - glm::vec3(0.0f, 0.0f, 0.0f));
    float texX = float(alpha / (2 * M_PI));
    float texY = float(theta / M_PI);

    if (texX > 1.0f)    texX = 1.0f;
    if (texY > 1.0f)    texY = 1.0f;

    return { x, y, z, N, texX, texY };
}

void SphericalBodyRenderer::_constructMainSphereVertices(int numEquatorVertices, GLuint* pVao, GLuint* pVbo, size_t* pNumSphereVertices)
{
    SphericalBody& s = *_sphere;

    std::vector<float>* v = ConstructSphereVertices(s._radius, s._color, numEquatorVertices, true);

    if (*pVbo != 0) {
        glDeleteBuffers(1, pVbo);
    }
    if (*pVao != 0) {
        glDeleteVertexArrays(1, pVao);
    }

    glGenVertexArrays(1, pVao);
    glBindVertexArray(*pVao);
    glGenBuffers(1, pVbo);
    glBindBuffer(GL_ARRAY_BUFFER, *pVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    // x, y & z coordinates of the point
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // r, g, b, a color values of the point
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // x, y & z of unit normal vector to the sphere at the point
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // texture coordinates of the point
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    *pNumSphereVertices = v->size() / PLANET_STRIDE_IN_VBO;
    
    //---------------------------------------------------------

    sendTextureToGpu();

    delete v;
}

void SphericalBodyRenderer::_constructMainIcoSphereVertices()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;

    IndexedMesh indexMesh = make_icosphere(_getIcoSphereSubdivisionLevel());
    //IndexedMesh indexMesh = make_icosphere(1);
    
    // .first is VertexList; which is vector of glm::vec3
    for (unsigned int i = 0; i < indexMesh.first->size(); i++)
    {
        glm::vec3& vertex = (*indexMesh.first)[i];

        // Generate normal for this vertex
        glm::vec3 N = glm::normalize(glm::vec3(vertex.x, vertex.y, vertex.z) - glm::vec3(0.0f, 0.0f, 0.0f));

        float texCoordX;
        float texCoordY;
        float d;

        //------------------------------------------------------------------
        // calculate alpha and beta angle. Convert it to a range between 0 & 1 to be used as texture coordinates.
        // At this point, radius is 1.0f.
        d = sqrt((vertex.x * vertex.x) + (vertex.y * vertex.y));
        float alpha = asin(float(vertex.y) / d);
        if (alpha == -0.0f)
            alpha = 0.0f;
        if (vertex.y >= 0.0f) {
            if (vertex.x >= 0.0f) { // 1st quadrant. no special handling
            }
            else     // 2nd quadrant
                alpha = (float)M_PI - alpha;
            
        }
        else {
            if (alpha >= 0.0f)
                throw std::exception("ERROR: alpha should be negative");

            // y is -ve. alpha will also be -ve
            if (vertex.x < 0.0f)
                // 3rd quadrant
                alpha = float(M_PI) - alpha;
            else
               // 4th quadrant
                alpha += 2 * float(M_PI);
        }
        
        if (alpha < 0)
        {
            std::string msg = "alpha unexpectedly less than 0. Alpha = " + std::to_string(alpha) + 
                              ", x = " + std::to_string(vertex.x) +
                              ", y = " + std::to_string(vertex.y)
                ;
            throw std::exception(msg.c_str());
        }
        texCoordX = float(alpha / (2 * M_PI));
        //printf("tex X: %f", texCoordX);
        //if (texCoordX > 1.0f)
        //    printf("texture X coord is greater than 1.0f");
        //if (texCoordX < 0.0f)
        //    printf("texture X coord is less than 0.0f");
        //if (texCoordX > 1.0f)
        //    texCoordX -= 1.0f;
        //if (texCoordX < 0.0f)
        //    texCoordX += 1.0f;

        //------------------------------------------------------------------
        // beta is measured from from +ve Z axis.
        float beta = asin(vertex.z);                // this is when measured from +ve X axis. goes from +PI/2 to -PI/2
        beta = -beta + float(M_PI / 2.0f);        // invert (-PI/2 to +PI/2) then shift (0 to PI)
        texCoordY = beta / float(M_PI);             // 0 to PI => 0 to 1
        //printf("tex Y: %f", texCoordY);
        //if (texCoordY > 1.0f)
        //    printf("texture Y coord is greater than 1.0f");
        //if (texCoordY < 0.0f)
        //    printf("texture Y coord is less than 0.0f");
        //if (texCoordY > 1.0f)
        //    texCoordY -= 1.0f;
        //if (texCoordY < 0.0f)
        //    texCoordY += 1.0f;

        //printf("%%%% texture coord: x = %f, y = %f\n", texCoordX, texCoordY);
        
        // scale vertices with unit radius to the actual radius of sphere.
        vertex *= s._radius;

        vector_push_back_12(*v, vertex.x, vertex.y, vertex.z, s._r, s._g, s._b, 1.0f, N.x, N.y, N.z, texCoordX, texCoordY);
    }

    spdlog::info("");
    spdlog::info("{}: SphericalBody: Num vertices = {}", _sphere->_name, v->size()/10);
    spdlog::info("{}: SphericalBody: Num elements = {}", _sphere->_name, indexMesh.second->size() / 6);


    //v = pair.first;
    auto e = indexMesh.second;

    glGenVertexArrays(1, &_mainVao);
    glBindVertexArray(_mainVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * e->size() * 3,
        e->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    numMainSphereVertices = v->size() / PLANET_STRIDE_IN_VBO;
    numMainSphereElements = e->size() * 3;

    //--------------------------------------------

    sendTextureToGpu();

    delete indexMesh.first;
    delete v;
    delete e;
}

void SphericalBodyRenderer::constructMainSphereVertices()
{
    int numEquatorVertices = _getNumEquatorVertices();
    _constructMainSphereVertices(numEquatorVertices, &_mainVao, &_mainVbo, &numMainSphereVertices);
}

void SphericalBodyRenderer::constructMainSphereVerticesForMinimap()
{
    int numEquatorVertices = _getNumEquatorVertices(PolygonCountLevel_VeryLow);
    _constructMainSphereVertices(numEquatorVertices, &_minimapMainVao, &_minimapMainVbo, &numMinimapMainSphereVertices);
}



// Construct the circular/elliptical orbit
void SphericalBodyRenderer::constructOrbit()
{
    std::vector<float>* v   = new std::vector<float>();
    SphericalBody& s               = *_sphere;
    glm::vec3 color         = s._color / 2.5f;

    float alpha_inc = float(2 * M_PI) / 500;

    if (USE_TO_SCALE)
        alpha_inc = float(2 * M_PI) / 5000;

    //---------------------------------------------------------------------------------
    // Orbit itself
    //---------------------------------------------------------------------------------
    for (float alpha = 0; alpha < 2 * M_PI; alpha += alpha_inc)
    {
        float x1 = s._orbitalRadius * cos(alpha);
        float y1 = s._orbitalRadius * sin(alpha);
        float z1 = 0;

        float x2 = s._orbitalRadius * cos(alpha + alpha_inc);
        float y2 = s._orbitalRadius * sin(alpha + alpha_inc);
        float z2 = 0;

        vector_push_back_7(*v, x1, y1, z1, color.r, color.g, color.b, 0.8f);
        vector_push_back_7(*v, x2, y2, z2, color.r, color.g, color.b, 0.8f);
    }

    //-----------------------------------
    if (_orbitVbo != 0) {
        glDeleteBuffers(1, &_orbitVbo);
    }
    if (_orbitVao != 0) {
        glDeleteVertexArrays(1, &_orbitVao);
    }

    glGenVertexArrays(1, &_orbitVao);
    glBindVertexArray(_orbitVao);
    glGenBuffers(1, &_orbitVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _orbitVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    // xyz of point
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // rgba of point
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numOrbitVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;
}

void SphericalBodyRenderer::constructOrbitalPlaneGridVertices()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;

    //---------------------------------------------------------------------------------
    // Orbital plane grid.
    //---------------------------------------------------------------------------------
    // generate parallel lines along Y axis in the orbital plane
    //float inc = float(_orbitalRadius) / int(_orbitalRadius / 50.0);

    float maxGridLines = 20;
    float inc = (s._orbitalRadius * 2 * 1.2f) / maxGridLines;
    //inc = std::max(inc, 50.0f);
    //printf("inc = %f\n", inc);

    float x, y;
    float radius = s._orbitalRadius;
    glm::vec3 color = s._color / 3.9f;      // make darker

    x = y = -radius * 1.2f;

    // Create a grid tiny bit above and below the orbital plane
    for (int i = 0; i <= maxGridLines; i++)
    {
        vector_push_back_7(*v, x, -radius * 1.2f, +1, color.r, color.g, color.b, 1.0f);
        vector_push_back_7(*v, x, +radius * 1.2f, +1, color.r, color.g, color.b, 1.0f);

        // If orbital plane is transparent, don't create the bottom grid
        if (!bOrbitalPlaneTransparency) {
            vector_push_back_7(*v, x, -radius * 1.2f, -1, color.r, color.g, color.b, 1.0f);
            vector_push_back_7(*v, x, +radius * 1.2f, -1, color.r, color.g, color.b, 1.0f);
        }

        x += inc;
    }
    for (int i = 0; i <= maxGridLines; i++)
    {
        vector_push_back_7(*v, -radius * 1.2f, y, +1, color.r, color.g, color.b, 1.0f);
        vector_push_back_7(*v, +radius * 1.2f, y, +1, color.r, color.g, color.b, 1.0f);

        // If orbital plane is transparent, don't create the bottom grid
        if (!bOrbitalPlaneTransparency) {
            vector_push_back_7(*v, -radius * 1.2f, y, -1, color.r, color.g, color.b, 1.0f);
            vector_push_back_7(*v, +radius * 1.2f, y, -1, color.r, color.g, color.b, 1.0f);
        }

        y += inc;
    }

    //--------------------------------------

    if (_orbitalPlaneGridVbo != 0) {
        glDeleteBuffers(1, &_orbitalPlaneGridVbo);
    }
    if (_orbitalPlaneGridVao != 0) {
        glDeleteVertexArrays(1, &_orbitalPlaneGridVao);
    }

    glGenVertexArrays(1, &_orbitalPlaneGridVao);
    glBindVertexArray(_orbitalPlaneGridVao);
    glGenBuffers(1, &_orbitalPlaneGridVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _orbitalPlaneGridVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numOrbitalPlaneGridVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;

}


void SphericalBodyRenderer::constructOrbitalPlaneVertices()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;
    float radius = s._orbitalRadius;
    glm::vec3 color = s._color / 5.0f;      // make darker
    float alpha = 1.0f;

    if (bOrbitalPlaneTransparency)
        alpha = 0.5f;

    //---------------------------------------------------------------------------------
    // Orbital plane.  This is centered at origin.
    //---------------------------------------------------------------------------------
    vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r, color.g, color.b, alpha);
    vector_push_back_7(*v, +radius * 1.2f, -radius * 1.2f, 0, color.r, color.g, color.b, alpha);
    vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r, color.g, color.b, alpha);
    vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r, color.g, color.b, alpha);
    vector_push_back_7(*v, -radius * 1.2f, +radius * 1.2f, 0, color.r, color.g, color.b, alpha);
    vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r, color.g, color.b, alpha);

    //---------------------------------

    if (_orbitalPlaneVbo != 0) {
        glDeleteBuffers(1, &_orbitalPlaneVbo);
    }
    if (_orbitalPlaneVao != 0) {
        glDeleteVertexArrays(1, &_orbitalPlaneVao);
    }

    glGenVertexArrays(1, &_orbitalPlaneVao);
    glBindVertexArray(_orbitalPlaneVao);
    glGenBuffers(1, &_orbitalPlaneVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _orbitalPlaneVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numOrbitalPlaneVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;
}


void SphericalBodyRenderer::constructRotationAxis()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;

    float m = 0.4f;      // color multiplier
    float radius = s._radius;
    glm::vec3& color = s._color;

    int polygonIncrement = _getNumEquatorVertices();
    float alpha_inc = float(2 * M_PI) / polygonIncrement;
    
    alpha_inc = alpha_inc * 10;
    
    float axisLengthMultipler = 1.3f;   // multiply to sphere's radius to get the extent of axis above the poles
    float axisCylinderDiameterMultiplier = 0.015f;

    float alpha;
    for (alpha = 0; alpha < float(2 * M_PI); alpha += alpha_inc)
    {
        //---------------------------------------------------
        // Cylindrical rotation axis 
        //---------------------------------------------------
        float x1 = axisCylinderDiameterMultiplier * radius * cos(alpha);
        float y1 = axisCylinderDiameterMultiplier * radius * sin(alpha);
        float z1 = 0.99f  * radius;                 // start from inside the sphere surface; hence radius multiplier is less than 1.

        float x2 = axisCylinderDiameterMultiplier * radius * cos(alpha + alpha_inc);
        float y2 = axisCylinderDiameterMultiplier * radius * sin(alpha + alpha_inc);
        float z2 = 0.99f  * radius;

        float z = radius * axisLengthMultipler;

        glm::vec3 N1 = glm::normalize(glm::vec3(x1, y1, 0.0f) - glm::vec3(0.0f, 0.0f, 0.0f));
        glm::vec3 N2 = glm::normalize(glm::vec3(x2, y2, 0.0f) - glm::vec3(0.0f, 0.0f, 0.0f));

        // two triangles that form a rectangle on north side
        vector_push_back_12(*v, x1, y1, z1, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x1, y1, z,  s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);

        vector_push_back_12(*v, x1, y1, z, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x2, y2, z,  s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);

        // two triangles that form a rectangle on south side
        vector_push_back_12(*v, x1, y1, -z1, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x2, y2, -z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x1, y1, -z, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);

        vector_push_back_12(*v, x1, y1, -z, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x2, y2, -z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        vector_push_back_12(*v, x2, y2, -z, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);


        //---------------------------------------------------
        // Conical rotation axis 
        //---------------------------------------------------

        //float x1 = 0.01f * radius * cos(alpha);
        //float y1 = 0.01f * radius * sin(alpha);
        //float z1 = 0.99 * radius;

        //float x2 = 0.01f * radius * cos(alpha + alpha_inc);
        //float y2 = 0.01f * radius * sin(alpha + alpha_inc);
        //float z2 = 0.99 * radius;

        //float z  = radius * 1.3;

        //glm::vec3 N1 = glm::normalize(glm::vec3(x1, y1, 0.0f) - glm::vec3(0.0f, 0.0f, 0.0f));
        //glm::vec3 N2 = glm::normalize(glm::vec3(x2, y2, 0.0f) - glm::vec3(0.0f, 0.0f, 0.0f));

        //vector_push_back_12(*v, x1,   y1,   z1,  s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
        //vector_push_back_12(*v, x2,   y2,   z2,  s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        //vector_push_back_12(*v, 0.0f, 0.0f, z,   s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);

        //vector_push_back_12(*v, x1,   y1,   -z1, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
        //vector_push_back_12(*v, x2,   y2,   -z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        //vector_push_back_12(*v, 0.0f, 0.0f, -z,  s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
    }

    //----------------------------------------

    if (_rotationAxisVbo != 0) {
        glDeleteBuffers(1, &_rotationAxisVbo);
    }
    if (_rotationAxisVao != 0) {
        glDeleteVertexArrays(1, &_rotationAxisVao);
    }

    glGenVertexArrays(1, &_rotationAxisVao);
    glBindVertexArray(_rotationAxisVao);
    glGenBuffers(1, &_rotationAxisVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _rotationAxisVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    numRotationAxisVertices = v->size() / PLANET_STRIDE_IN_VBO;
    delete v;
}


void SphericalBodyRenderer::constructLongRotationAxis()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;

    float radius = s._radius;
    glm::vec3& color = s._color;
    float r = color.r;
    float g = color.g;
    float b = color.b;

    vector_push_back_12(*v, 0.0f, 0.0f,  radius,      r, g, b, 1.0f, 1.0, 0.0f, 0.0f, 0.0f, 0.0f);
    vector_push_back_12(*v, 0.0f, 0.0f,  radius*1000, r, g, b, 1.0f, 1.0, 0.0f, 0.0f, 0.0f, 0.0f);
    vector_push_back_12(*v, 0.0f, 0.0f, -radius,      r, g, b, 1.0f, 1.0, 0.0f, 0.0f, 0.0f, 0.0f);
    vector_push_back_12(*v, 0.0f, 0.0f, -radius*1000, r, g, b, 1.0f, 1.0, 0.0f, 0.0f, 0.0f, 0.0f);

    //----------------------------------------

    if (_longRotationAxisVbo != 0) {
        glDeleteBuffers(1, &_longRotationAxisVbo);
    }
    if (_longRotationAxisVao != 0) {
        glDeleteVertexArrays(1, &_longRotationAxisVao);
    }

    glGenVertexArrays(1, &_longRotationAxisVao);
    glBindVertexArray(_longRotationAxisVao);
    glGenBuffers(1, &_longRotationAxisVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _longRotationAxisVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    numLongRotationAxisVertices = v->size() / PLANET_STRIDE_IN_VBO;
    delete v;
}


void SphericalBodyRenderer::sendTextureToGpu()
{
    if (_texture == 0)
    {
        //printf("*************** texture setup *******************\n");
        //printf("_textureFilename = %s\n", _textureFilename.c_str());
        if (!_textureFilename.empty())
        {
            glGenTextures(1, &_texture);
            glBindTexture(GL_TEXTURE_2D, _texture);
#ifdef USE_ICOSPHERE
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // The following border color isn't used because 'clamp_to_edge' used above. 
            // Set border color to debug problems at the edge in the future.
            float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);


            std::vector<unsigned char> image1;
            unsigned int width, height;
            std::string textureFilePath = _locateTextureFile(_textureFilename.c_str());
            spdlog::info("Using texture file " + textureFilePath);
            unsigned int error = lodepng::decode(image1, width, height, textureFilePath.c_str());
            if (error)
            {
                printf("error %d: %s\n", error, lodepng_error_text(error));
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1.data());
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

    if (_texture2 == 0)
    {
        if (!_textureFilename2.empty())
        {
            glGenTextures(1, &_texture2);
            glBindTexture(GL_TEXTURE_2D, _texture2);
#ifdef USE_ICOSPHERE
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // The following border color isn't used because 'clamp_to_edge' used above. 
            // Set border color to debug problems at the edge in the future.
            float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);


            std::vector<unsigned char> image1;
            unsigned int width, height;
            std::string textureFilePath = _locateTextureFile(_textureFilename2.c_str());
            spdlog::info("Using texture 2 file " + textureFilePath);
            unsigned int error = lodepng::decode(image1, width, height, textureFilePath.c_str());
            if (error)
            {
                printf("error %d: %s\n", error, lodepng_error_text(error));
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1.data());
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }
}

// Construct vertices for various aspects of the sphere such as the sphere surface itself, orbit, latitude/longitudes,
// orbital plane, etc.
//  - Vertices are sent to opengl immediately after creation.
//  - If the sphere parameters change at runtime, some vertex generation methods have to be invoked again from those places.
void SphericalBodyRenderer::constructVerticesAndSendToGpu()
{

#ifndef USE_ICOSPHERE
    //---------------------------------------------------------------------------------------------------
    // The sphere itself
    int numEquatorVertices;
    
    constructMainSphereVertices();
    constructMainSphereVerticesForMinimap();
#else
    //---------------------------------------------------------------------------------------------------
    // The sphere itself (Icosphere)
    _constructMainIcoSphereVertices();
#endif

    // Orbital itself
    constructOrbit();
    constructOrbitalPlaneVertices();
    constructOrbitalPlaneGridVertices();

    // Rotation axis
    constructRotationAxis();
    constructLongRotationAxis();
}

// locate and return the complete path of the given texture filename in know texture file locations.
std::string SphericalBodyRenderer::_locateTextureFile(const char * fileName)
{
    std::vector<std::string> textureDirs = {
        "../../leela/textures",
        "textures",
    };

    for (std::string textureDir : textureDirs) {

        std::string filePath = textureDir + "/" + fileName;
        std::ifstream textureFile(filePath);
        if (!textureFile.fail()) {
            return filePath;
        }
    }

    std::string msg = "Failed to locate ";
    msg += fileName;
    throw std::exception(msg.c_str());
}

int SphericalBodyRenderer::_getNumEquatorVertices(PolygonCountLevel polygonCountLevel)
{
    PolygonCountLevel level;

    if (polygonCountLevel == PolygonCountLevel_YouChoose)
        level = _polygonCountLevel;
    else
        level = polygonCountLevel;

    switch (level)
    {
        case PolygonCountLevel_VeryLow:
            return 30;
        case PolygonCountLevel_Low:
            return 100;
        case PolygonCountLevel_Medium:
            return 600;
        case PolygonCountLevel_High:
            return 1000;
        default:
            return 500;
    }
}

int SphericalBodyRenderer::_getIcoSphereSubdivisionLevel()
{
    switch (_polygonCountLevel)
    {
    case PolygonCountLevel_Low:
        return 4;
    case PolygonCountLevel_Medium:
        return 6;
    case PolygonCountLevel_High:
        return 9;
    }
}

//############################################################################################################

PlanetRenderer::PlanetRenderer(std::string textureFilename, std::string textureFilename2)
    : SphericalBodyRenderer(textureFilename, textureFilename2)
{
}

PlanetRenderer::~PlanetRenderer()
{

}

void PlanetRenderer::render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram)
{
    if (renderStage == RenderStage::Main) {
        if (glslProgram.type() == GlslProgramType::Planet) {
            if (viewportType == ViewportType::Primary || viewportType == ViewportType::Minimap) {
                doShaderConfig(glslProgram);

                if (bShowBody) {
                    if (viewportType == ViewportType::Primary)
                        renderSphere(glslProgram);
                    else
                        renderMinimapSphere(glslProgram);
                }
                if (g_leela->bShowPlanetAxis) {
                    renderRotationAxis(glslProgram);
                }
            }
        }
        else if (glslProgram.type() == GlslProgramType::Simple) {
            if (viewportType == ViewportType::Primary) {
                renderOrbit(glslProgram);
                if (g_leela->bShowPlanetAxis)
                    renderLongRotationAxis(glslProgram);
            }
            else if (viewportType == ViewportType::Minimap) {
                renderOrbit(glslProgram);
            }
        }
    }
    else if (renderStage == RenderStage::TranslucentMain) {
        if (glslProgram.type() == GlslProgramType::Simple) {
            if (viewportType == ViewportType::Primary) {
                renderOrbitalPlane(glslProgram);
            }
        }
    }
}


void PlanetRenderer::doShaderConfig(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    PNT sphereCenter = PNT(s.getCenter());
    float distSunToThisSphere = (float)PNT(s._sunSphere->getCenter()).distanceTo(sphereCenter);
    float selfUmbraLength = (s.getRadius() * distSunToThisSphere) / (s._sunSphere->getRadius() - s.getRadius());
    float sineOfSelfUmbraConeHalfAngle = asin(s.getRadius() / selfUmbraLength);


    glslProgram.setMat4("model", glm::value_ptr(s.getTransform()));
    glslProgram.setVec3("sphereInfo.centerTransformed", glm::value_ptr(s.getCenter()));
    glslProgram.setFloat("sphereInfo.radius", s.getRadius());
    float multiplierAdjust = 1.0f;
    if (g_leela->bShowLowDarknessAtNight)
    {
        multiplierAdjust = 2.0f;
    }
    //glslProgram.setFloat("nightColorMultiplier", _nightColorMultiplier * multiplierAdjust);
    NightColorDarkness level = nightColorDarknessStrToLevel(g_leela->nightDarknessLevelStr);
    float levelFloatValue = nightColorDarknessLevelToFloat(level);

    glslProgram.setFloat("nightColorMultiplier", levelFloatValue * multiplierAdjust);

    glslProgram.setFloat("sphereInfo.sineOfSelfUmbraConeHalfAngle", sineOfSelfUmbraConeHalfAngle);

    //glEnable(GL_MULTISAMPLE);

    if (s._relatedSphere != nullptr)
    {
        // When drawing earth, other sphere is moon.
        glslProgram.setFloat("otherSphereRadius", s._relatedSphere->getRadius());
        glslProgram.setVec3("otherSphereCenterTransformed", glm::value_ptr(s._relatedSphere->getModelTransformedCenter()));
    }
    else {
        glslProgram.setFloat("otherSphereRadius", 0.0f);
    }

    if (!_textureFilename.empty())
    {
        glslProgram.setBool("useTexture", g_leela->bRealisticSurfaces);

        //printf("texture filename not empty. Texture = %d\n", _texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glslProgram.setInt("texture1", 0);

        if (!_textureFilename2.empty()) {
            glslProgram.setBool("useTexture2", true);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _texture2);
            glslProgram.setInt("texture2", 1);
        }
        else {
            glslProgram.setBool("useTexture2", false);
        }
    }
    else {
        // usage of texture was enabled globally earlier. Disable it.
        glslProgram.setBool("useTexture", false);
    }
}

/*
 * Render the main sphere.
 * 
 * otherSphere      the sphere that could eclipse the sun for this planet/moon. 
 */
void PlanetRenderer::renderSphere(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!_sphere->bIsCenterOfMass) {
        glBindVertexArray(_mainVao);

        if (g_leela->bShowWireframeSurfaces)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw vertices
#ifndef USE_ICOSPHERE
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) numMainSphereVertices);
#else
        glDrawElements(GL_TRIANGLES, numMinimapMainSphereElements, GL_UNSIGNED_INT, 0);
#endif
    }
}


void PlanetRenderer::renderMinimapSphere(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!_sphere->bIsCenterOfMass) {
        glBindVertexArray(_minimapMainVao);

        if (g_leela->bShowWireframeSurfaces)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Draw vertices
#ifndef USE_ICOSPHERE
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numMinimapMainSphereVertices);
#else
        glDrawElements(GL_TRIANGLES, numMinimapMainSphereElements, GL_UNSIGNED_INT, 0);
#endif
    }
}


void PlanetRenderer::renderOrbit(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!s.bIsCenterOfMass)
    {
        // Uses same transform as that of orbital plane. But orbital plane may not be drawn depending on flags. Hence
        // set the model matrix again.
        if (bShowOrbit)
        {
            glslProgram.setMat4("model", glm::value_ptr(s.getOrbitalPlaneModelMatrix()));

            glBindVertexArray(_orbitVao);
            glDrawArrays(GL_LINES, 0, (GLsizei) numOrbitVertices);
        }
    }
}


void PlanetRenderer::renderOrbitalPlane(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!s.bIsCenterOfMass)
    {
        // orbital plane has its own model transform different from the sphere itself.
        if (bShowOrbitalPlane)
        {
            glslProgram.setMat4("model", glm::value_ptr(s.getOrbitalPlaneModelMatrix()));

            // Draw orbital plane grid
            glBindVertexArray(_orbitalPlaneGridVao);
            glDrawArrays(GL_LINES, 0, (GLsizei)numOrbitalPlaneGridVertices);

            // Draw vertices
            if (bOrbitalPlaneTransparency) {
                glDepthMask(GL_FALSE);
            }
            glBindVertexArray(_orbitalPlaneVao);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numOrbitalPlaneVertices);
            if (bOrbitalPlaneTransparency) {
                glDepthMask(GL_TRUE);
            }
        }
    }
}


/*
 * 
 */
void PlanetRenderer::renderRotationAxis(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!s.bIsCenterOfMass)
    {
        //glslProgram.setMat4("model", glm::value_ptr(_sphere.getModelMatrix()));

        PNT sphereCenter = PNT(s.getCenter());
        float distSunToThisSphere = (float)PNT(s._sunSphere->getCenter()).distanceTo(sphereCenter);
        float selfUmbraLength = (s.getRadius() * distSunToThisSphere) / (s._sunSphere->getRadius() - s.getRadius());
        float sineOfSelfUmbraConeHalfAngle = asin(s.getRadius() / selfUmbraLength);


        glslProgram.setMat4("model", glm::value_ptr(s.getTransform()));
        glslProgram.setVec3("sphereInfo.centerTransformed", glm::value_ptr(s.getCenter()));
        glslProgram.setFloat("sphereInfo.radius", s.getRadius());
        //glslProgram.setFloat("nightColorMultiplier", 0.1);
        glslProgram.setFloat("nightColorMultiplier", _nightColorMultiplier);
        glslProgram.setFloat("sphereInfo.sineOfSelfUmbraConeHalfAngle", sineOfSelfUmbraConeHalfAngle);

        //glEnable(GL_MULTISAMPLE);

        if (s._relatedSphere != nullptr)
        {
            // When drawing earth, other sphere is moon.
            glslProgram.setFloat("otherSphereRadius", s._relatedSphere->getRadius());
            glslProgram.setVec3("otherSphereCenterTransformed", glm::value_ptr(s._relatedSphere->getModelTransformedCenter()));
        }

        glslProgram.setBool("useTexture", false);

        glBindVertexArray(_rotationAxisVao);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei) numRotationAxisVertices);
    }
}

void PlanetRenderer::renderLongRotationAxis(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!s.bIsCenterOfMass)
    {
        if (bLongAxis)
        {
            glslProgram.setMat4("model", glm::value_ptr(s.getTransform()));
            glBindVertexArray(_longRotationAxisVao);
            glDrawArrays(GL_LINES, 0, (GLsizei)numLongRotationAxisVertices);
        }
    }
}


//############################################################################################################



SunRenderer::SunRenderer( std::string textureFilename)
    : SphericalBodyRenderer(textureFilename)
{
}

SunRenderer::~SunRenderer()
{

}

void SunRenderer::doShaderConfig(GlslProgram& glslProgram)
{
    if (!_textureFilename.empty())
    {
        //printf("texture filename not empty. Texture = %d\n", _texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texture);
    }
    else
    {
        // usage of texture was enabled globally earlier. Disable it.
        glslProgram.setBool("useTexture", false);
    }
}

void SunRenderer::render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram)
{
    if (renderStage == RenderStage::Main) {
        if (glslProgram.type() == GlslProgramType::Sun) {
            if (viewportType == ViewportType::Primary || viewportType == ViewportType::Minimap) {
                doShaderConfig(glslProgram);

                if (bShowBody)
                    if (viewportType == ViewportType::Primary)
                        _renderSphere(glslProgram);
                    else
                        renderMinimapSphere(glslProgram);
            }
        }
    }
}

void SunRenderer::_renderSphere(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    glslProgram.setMat4("model", glm::value_ptr(s.getTransform()));

    glBindVertexArray(_mainVao);

    // Draw vertices
#ifndef USE_ICOSPHERE
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) numMainSphereVertices);
#else
    glDrawElements(GL_TRIANGLES, numMainSphereElements, GL_UNSIGNED_INT, 0);
#endif
}

void SunRenderer::renderMinimapSphere(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    glslProgram.setMat4("model", glm::value_ptr(s.getTransform()));

    glBindVertexArray(_minimapMainVao);

    // Draw vertices
#ifndef USE_ICOSPHERE
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numMinimapMainSphereVertices);
#else
    glDrawElements(GL_TRIANGLES, numMinimapMainSphereElements, GL_UNSIGNED_INT, 0);
#endif
}