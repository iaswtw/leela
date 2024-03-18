#include "pch.h"
#include "SphereRenderer.h"
#include "Utils.h"


#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include "sphere.h"
#include "Space.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <array>

#include "lodepng.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

using TriangleList = std::vector<Triangle>;
using VertexList = std::vector<glm::vec3>;


//#define USE_ICOSPHERE

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

static inline void vector_push_back_10(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10)
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

static inline void vector_push_back_12(std::vector<float>& v, float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10, float f11, float f12)
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


SphereRenderer::SphereRenderer(Sphere& sphere, std::string textureFilename) :
    _sphere(sphere)
{
    setNightColorDarkness(NightColorDarkness_Black);
    //setNightColorDarkness(NightColorDarkness_VeryLow);
    _textureFilename = textureFilename;
    printf("Setting _textureFilename to %s\n", _textureFilename.c_str());
}

SphereRenderer::~SphereRenderer()
{
}

void SphereRenderer::setAsLightSource()
{
    _bIsLightSource = True;
}

void SphereRenderer::setNightColorDarkness(NightColorDarkness darkness)
{
    _nightColorDarkness = darkness;
    switch (_nightColorDarkness)
    {
    case NightColorDarkness_Black:      _nightColorMultiplier = 0.0f;   break;
    case NightColorDarkness_VeryHigh:   _nightColorMultiplier = 0.05f;  break;
    case NightColorDarkness_High:       _nightColorMultiplier = 0.15f;  break;
    case NightColorDarkness_Medium:     _nightColorMultiplier = 0.2f;   break;
    case NightColorDarkness_Low:        _nightColorMultiplier = 0.5f;   break;
    case NightColorDarkness_VeryLow:    _nightColorMultiplier = 0.8f;   break;
    case NightColorDarkness_None:       _nightColorMultiplier = 1.0f;   break;
    }
}

void SphereRenderer::setPolygonCountLevel(PolygonCountLevel polygonCountLevel)
{
	_polygonCountLevel = polygonCountLevel;
}

/*
 
 */
std::tuple<float, float, float, glm::vec3, float, float> SphereRenderer::calcPointOnSphere(float radius, float alpha, float theta)
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

std::vector<float>* SphereRenderer::_constructMainSphereVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

	float polygonIncrement = _getPolygonIncrement();

	float alpha_inc = float(2 * M_PI) / polygonIncrement;
    float theta_inc = float(M_PI) / (polygonIncrement / 2);

    int numFloats = int((2 * M_PI / alpha_inc) * (M_PI / theta_inc)) * 10;
    printf("numFloats = %d\n", numFloats);

    v->reserve(numFloats);
    float alpha;
    float theta;
    for (alpha = 0; alpha < float(2 * M_PI); alpha += alpha_inc)
    {
        for (theta = 0; theta < float(M_PI); theta += theta_inc)
        {
            float theta_2 = theta + theta_inc;
            float alpha_2 = alpha + alpha_inc;

            auto [x1, y1, z1, N1, texX1, texY1] = calcPointOnSphere(s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = calcPointOnSphere(s._radius, alpha, theta_2);
            auto [x3, y3, z3, N3, texX3, texY3] = calcPointOnSphere(s._radius, alpha_2, theta);
            auto [x4, y4, z4, N4, texX4, texY4] = calcPointOnSphere(s._radius, alpha_2, theta_2);

            vector_push_back_12(*v, x1, y1, z1, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, texX1, texY1);
            vector_push_back_12(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, texX2, texY2);
            vector_push_back_12(*v, x3, y3, z3, s._r, s._g, s._b, 1.0f, N3.x, N3.y, N3.z, texX3, texY3);
            vector_push_back_12(*v, x3, y3, z3, s._r, s._g, s._b, 1.0f, N3.x, N3.y, N3.z, texX3, texY3);
            vector_push_back_12(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, texX2, texY2);
            vector_push_back_12(*v, x4, y4, z4, s._r, s._g, s._b, 1.0f, N4.x, N4.y, N4.z, texX4, texY4);
        }
    }

    return v;
}

std::pair<std::vector<float>*, std::vector<Triangle>*>  SphereRenderer::_constructMainIcoSphereVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

    IndexedMesh indexMesh = make_icosphere(_getIcoSphereSubdivisionLevel());
    //IndexedMesh indexMesh = make_icosphere(1);
    
    // .first is VertexList; which is vector of glm::vec3
    for (int i = 0; i < indexMesh.first->size(); i++)
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
        if (vertex.y >= 0.0f)
        {
            if (vertex.x >= 0.0f)
            { // 1st quadrant. no special handling
            }
            else
            {   // 2nd quadrant
                alpha = M_PI - alpha;
            }
        }
        else
        {
            if (alpha >= 0.0f)
                throw std::exception("ERROR: alpha should be negative");

            // y is -ve. alpha will also be -ve
            if (vertex.x < 0.0f)
            {   // 3rd quadrant
                alpha = M_PI - alpha;
            }
            else
            {   // 4th quadrant
                alpha += 2 * M_PI;
            }
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

        //------------------------------------------------------------------
        // beta is measured from from +ve Z axis.
        float beta = asin(vertex.z);                // this is when measured from +ve X axis. goes from +PI/2 to -PI/2
        beta = -beta + (M_PI / 2.0f);               // invert (-PI/2 to +PI/2) then shift (0 to PI)
        texCoordY = beta / M_PI;                    // 0 to PI => 0 to 1


        //printf("%%%% texture coord: x = %f, y = %f\n", texCoordX, texCoordY);
        
        
        // scale vertices with unit radius to the actual radius of sphere.
        vertex *= s._radius;


        vector_push_back_12(*v, vertex.x, vertex.y, vertex.z, s._r, s._g, s._b, 1.0f, N.x, N.y, N.z, texCoordX, texCoordY);
    }

    printf("=== Sphere: Num vertices = %d\n", v->size()/10);
    printf("=== Sphere: Num elements = %d\n", indexMesh.second->size() / 6);

    delete indexMesh.first;
    return std::pair<std::vector<float>*, std::vector<Triangle>*>(v, indexMesh.second);
}

std::vector<float>* SphereRenderer::_constructLatitudesAndLongitudeVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

	float polygonIncrement = _getPolygonIncrement();
    float inc = float(2 * M_PI) / polygonIncrement;

    //---------------------------------------------------------------------------------
    // Longitudes
    //---------------------------------------------------------------------------------
    //std::list<float> alphas = { 0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165 };

    //for (float alpha : alphas)
    for (float alpha_deg = 0.0f; alpha_deg < 360.0f; alpha_deg += 10.0f)
    {
        float alpha = glm::radians(alpha_deg);

        for (float theta = 0; theta < float(2 * M_PI); theta += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = calcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = calcPointOnSphere(1.001f * s._radius, alpha, theta + inc);

            float cMult;
            if (alpha == 0.0f)
                cMult = 0.8f;
            else
                cMult = 0.9f;

            vector_push_back_12(*v, x1, y1, z1, s._r*cMult, s._g*cMult, s._b*cMult, 1.0, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r*cMult, s._g*cMult, s._b*cMult, 1.0, N2.x, N2.y, N2.z, 0.0f, 0.0f);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    //---------------------------------------------------------------------------------
    // Special Latitudes
    //---------------------------------------------------------------------------------
    std::list<float> thetas = { 0, 66.5f, 23.5f, 90, 90 + 23.5f, 90 + 66.5f };          // 0 = +z axis, 90 is equator, 90+66.5 is antarctic circle

    for (float theta : thetas)
    {
        theta = glm::radians(theta);

        for (float alpha = 0; alpha < float(2 * M_PI); alpha += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = calcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = calcPointOnSphere(1.001f * s._radius, alpha + inc, theta);

            vector_push_back_12(*v, x1, y1, z1, s._r*0.9f, s._g*0.5f, s._b*0.5f, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r*0.9f, s._g*0.5f, s._b*0.5f, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    //---------------------------------------------------------------------------------
    // Latitudes
    //---------------------------------------------------------------------------------
    for (float thetas = 0; thetas < 180; thetas += 10)
    {
        float theta = glm::radians(thetas);
        float cMult = 0.9f;

        for (float alpha = 0; alpha < float(2 * M_PI); alpha += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = calcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = calcPointOnSphere(1.001f * s._radius, alpha + inc, theta);

            vector_push_back_12(*v, x1, y1, z1, s._r*cMult, s._g*cMult, s._b*cMult, 1.0f, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r*cMult, s._g*cMult, s._b*cMult, 1.0f, N2.x, N2.y, N2.z, 0.0f, 0.0f);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    return v;
}


// Construct the circular/elliptical orbit
std::vector<float>* SphereRenderer::_constructOrbit()
{
    std::vector<float>* v   = new std::vector<float>();
    Sphere& s               = _sphere;
    glm::vec3& color        = s._orbitalPlaneColor;
    float m                 = 0.7f;

    float alpha_inc = float(2 * M_PI) / 500;

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

        vector_push_back_7(*v, x1, y1, z1, color.r*m, color.g*m, color.b*m, 0.8f);
        vector_push_back_7(*v, x2, y2, z2, color.r*m, color.g*m, color.b*m, 0.8f);
    }

    return v;
}

std::vector<float>* SphereRenderer::_constructOrbitalPlaneVertices()
{
    std::vector<float>* v   = new std::vector<float>();
    Sphere& s               = _sphere;
    float m                 = 0.3f;                          // color multiplier
    float radius            = s._orbitalRadius;
    glm::vec3& color        = s._orbitalPlaneColor;
    float alpha             = 1.0f;

    //---------------------------------------------------------------------------------
    // Orbital plane.  This is centered at origin.
    //---------------------------------------------------------------------------------
    vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, alpha);
    vector_push_back_7(*v, +radius * 1.2f, -radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, alpha);
    vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, alpha);
    vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, alpha);
    vector_push_back_7(*v, -radius * 1.2f, +radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, alpha);
    vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, alpha);

    return v;
}
std::vector<float>* SphereRenderer::_constructOrbitalPlaneGridVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

    //---------------------------------------------------------------------------------
    // Orbital plane grid.
    //---------------------------------------------------------------------------------
    // generate parallel lines along Y axis in the orbital plane
    //float inc = float(_orbitalRadius) / int(_orbitalRadius / 50.0);

    float maxGridLines      = 20;
    float inc               = (s._orbitalRadius * 2 * 1.2f) / maxGridLines;
    //inc = std::max(inc, 50.0f);
    //printf("inc = %f\n", inc);

    float x, y;
    float m                 = 0.4f;      // color multiplier
    float radius            = s._orbitalRadius;
    glm::vec3& color        = s._orbitalPlaneColor;

    x = y = -radius * 1.2f;

    for (int i = 0; i <= maxGridLines; i++)
    {
        vector_push_back_7(*v, x, -radius * 1.2f, +1, color.r*m, color.g*m, color.b*m, 1.0f);
        vector_push_back_7(*v, x, +radius * 1.2f, +1, color.r*m, color.g*m, color.b*m, 1.0f);
        vector_push_back_7(*v, x, -radius * 1.2f, -1, color.r*m, color.g*m, color.b*m, 1.0f);
        vector_push_back_7(*v, x, +radius * 1.2f, -1, color.r*m, color.g*m, color.b*m, 1.0f);

        x += inc;
    }
    for (int i = 0; i <= maxGridLines; i++)
    {
        vector_push_back_7(*v, -radius * 1.2f, y, +1, color.r*m, color.g*m, color.b*m, 1.0f);
        vector_push_back_7(*v, +radius * 1.2f, y, +1, color.r*m, color.g*m, color.b*m, 1.0f);
        vector_push_back_7(*v, -radius * 1.2f, y, -1, color.r*m, color.g*m, color.b*m, 1.0f);
        vector_push_back_7(*v, +radius * 1.2f, y, -1, color.r*m, color.g*m, color.b*m, 1.0f);

        y += inc;
    }

    return v;
}


void SphereRenderer::constructVerticesAndSendToGpu()
{
    GLuint vbo;     // vertex buffer object
    GLuint ebo;     // element buffer object
    std::vector<float> *v = nullptr;


#ifndef USE_ICOSPHERE
    //---------------------------------------------------------------------------------------------------
    // The sphere itself
    v = _constructMainSphereVertices();

    glGenVertexArrays(1, &_mainVao);
    glBindVertexArray(_mainVao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

    numMainSphereVertices = v->size() / PLANET_STRIDE_IN_VBO;
    delete v;

#else
    //---------------------------------------------------------------------------------------------------
    // The sphere itself (Icosphere)
    auto pair = _constructMainIcoSphereVertices();

    v = pair.first;
    auto e = pair.second;

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
    
    delete v;
    delete e;
#endif

    printf("*************** texture setup *******************\n");
    printf("_textureFilename = %s\n", _textureFilename.c_str());
    if (!_textureFilename.empty())
    {
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // The following border color isn't used because 'clamp_to_edge' used above. 
        // Set border color to debug problems at the edge in the future.
        float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

        std::vector<unsigned char> image1;
        unsigned int width, height;
        unsigned int error = lodepng::decode(image1, width, height, _textureFilename.c_str());
        if (error)
        {
            printf("error %d: %s\n", error, lodepng_error_text(error));
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1.data());
        glGenerateMipmap(GL_TEXTURE_2D);


        //int width, height, nrChannels;
        //
        //data = stbi_load(_textureFilename.c_str(), &width, &height, &nrChannels, 0);
        //if (data)
        //{
        //    printf("\n====== Image: width = %d, height = %d, nrChannels = %d\n", width, height, nrChannels);
        //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //    glGenerateMipmap(GL_TEXTURE_2D);
        //    glBindTexture(GL_TEXTURE_2D, 0);
        //    stbi_image_free(data);
        //}
        //else
        //{
        //    printf("ERROR: Unable to load texture image\n");
        //    throw std::exception("Unable to load texture image");
        //}
    }

    //---------------------------------------------------------------------------------------------------
    // latitude and longitude
    v = _constructLatitudesAndLongitudeVertices();

    glGenVertexArrays(1, &_latAndLongVao);
    glBindVertexArray(_latAndLongVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

    numLatAndLongVertices = v->size() / PLANET_STRIDE_IN_VBO;
    delete v;

    //---------------------------------------------------------------------------------------------------
    // Orbital plane
    v = _constructOrbitalPlaneVertices();

    glGenVertexArrays(1, &_orbitalPlaneVao);
    glBindVertexArray(_orbitalPlaneVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

    //---------------------------------------------------------------------------------------------------
    // orbital plane grid lines
    v = _constructOrbitalPlaneGridVertices();

    glGenVertexArrays(1, &_orbitalPlaneGridVao);
    glBindVertexArray(_orbitalPlaneGridVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

    //---------------------------------------------------------------------------------------------------
    // Orbital itself
    v = _constructOrbit();

    glGenVertexArrays(1, &_orbitVao);
    glBindVertexArray(_orbitVao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    numOrbitVertices = v->size() / VERTEX_STRIDE_IN_VBO;
    delete v;
}


float SphereRenderer::_getPolygonIncrement()
{
	switch (_polygonCountLevel)
	{
	case PolygonCountLevel_Low:
		return 100.0;
	case PolygonCountLevel_Medium:
		return 500.0;
	case PolygonCountLevel_High:
		return 1000.0;
	}
}

int SphereRenderer::_getIcoSphereSubdivisionLevel()
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

PlanetRenderer::PlanetRenderer(Sphere& sphere, std::string textureFilename)
	: SphereRenderer(sphere, textureFilename)
{
}

PlanetRenderer::~PlanetRenderer()
{

}

void PlanetRenderer::renderSphere(GlslProgram& glslProgram, Sphere* sun, Sphere* otherSphere)
{
	//----------------------------------------------------------------------------------------------------
	// Main sphere
	//----------------------------------------------------------------------------------------------------
    PNT sphereCenter = PNT(_sphere.getCenter());
    float distSunToThisSphere = PNT(sun->getCenter()).distanceTo(sphereCenter);
    float selfUmbraLength = (_sphere.getRadius() * distSunToThisSphere) / (sun->getRadius() - _sphere.getRadius());
    float sineOfSelfUmbraConeHalfAngle = asin(_sphere.getRadius() / selfUmbraLength);


	glslProgram.setMat4("model", glm::value_ptr(_sphere.getModelMatrix()));
	glslProgram.setVec3("sphereInfo.centerTransformed", glm::value_ptr(_sphere.getCenter()));
	glslProgram.setFloat("sphereInfo.radius", _sphere.getRadius());
	//glslProgram.setFloat("nightColorMultiplier", 0.1);
    glslProgram.setFloat("nightColorMultiplier", _nightColorMultiplier);
    glslProgram.setFloat("sphereInfo.sineOfSelfUmbraConeHalfAngle", sineOfSelfUmbraConeHalfAngle);

	//glEnable(GL_MULTISAMPLE);

	if (otherSphere != nullptr)
	{
		// When drawing earth, other sphere is moon.
		glslProgram.setFloat("otherSphereRadius", otherSphere->getRadius());
		glslProgram.setVec3("otherSphereCenterTransformed", glm::value_ptr(otherSphere->getModelTransformedCenter()));
	}

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
	glBindVertexArray(_mainVao);

	// Draw vertices
#ifndef USE_ICOSPHERE
	glDrawArrays(GL_TRIANGLES, 0, numMainSphereVertices);
#else
    glDrawElements(GL_TRIANGLES, numMainSphereElements, GL_UNSIGNED_INT, 0);
#endif

}

void PlanetRenderer::renderLatitudeAndLongitudes(GlslProgram& glslProgram)
{
	//----------------------------------------------------------------------------------------------------
	// Latitude and longitude
	//----------------------------------------------------------------------------------------------------
	if (bShowLatitudesAndLongitudes)
	{
        glslProgram.setMat4("model", glm::value_ptr(_sphere.getModelMatrix()));
        glBindVertexArray(_latAndLongVao);

        glslProgram.setBool("useTexture", false);

		// Draw vertices
		glDrawArrays(GL_LINES, 0, numLatAndLongVertices);
	}
}

void PlanetRenderer::renderOrbitalPlane(GlslProgram& glslProgram)
{
	//----------------------------------------------------------------------------------------------------
	// Orbital plane
	//----------------------------------------------------------------------------------------------------
	// orbital plane has its own model transform different from the sphere itself.
	if (bShowOrbitalPlane)
	{
		glslProgram.setMat4("model", glm::value_ptr(_sphere.getOrbitalPlaneModelMatrix()));

		// Draw orbital plane grid
		glBindVertexArray(_orbitalPlaneGridVao);
		glDrawArrays(GL_LINES, 0, numOrbitalPlaneGridVertices);

		// Draw vertices
        //glDepthMask(GL_FALSE);
		glBindVertexArray(_orbitalPlaneVao);
		glDrawArrays(GL_TRIANGLES, 0, numOrbitalPlaneVertices);
        //glDepthMask(GL_TRUE);
	}
}

void PlanetRenderer::renderOrbit(GlslProgram& glslProgram)
{
	//----------------------------------------------------------------------------------------------------
	// Orbital itself
	//----------------------------------------------------------------------------------------------------
	// Uses same transform as that of orbital plane. But orbital plane may not be drawn depending on flags. Hence
	// set the model matrix again.
	if (bShowOrbit)
	{
		glslProgram.setMat4("model", glm::value_ptr(_sphere.getOrbitalPlaneModelMatrix()));

		glBindVertexArray(_orbitVao);
		glDrawArrays(GL_LINES, 0, numOrbitVertices);
	}
}



//############################################################################################################



SunRenderer::SunRenderer(Sphere& sphere, std::string textureFilename)
	: SphereRenderer(sphere, textureFilename)
{
}

SunRenderer::~SunRenderer()
{

}

void SunRenderer::renderSphere(GlslProgram& glslProgram)
{
    glslProgram.setMat4("model", glm::value_ptr(_sphere.getModelMatrix()));

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
    glBindVertexArray(_mainVao);

	// Draw vertices
#ifndef USE_ICOSPHERE
    glDrawArrays(GL_TRIANGLES, 0, numMainSphereVertices);
#else
    glDrawElements(GL_TRIANGLES, numMainSphereElements, GL_UNSIGNED_INT, 0);
#endif
}