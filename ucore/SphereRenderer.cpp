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


using TriangleList = std::vector<Triangle>;
using VertexList = std::vector<glm::vec3>;


#define USE_ICOSPHERE

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


SphereRenderer::SphereRenderer(Sphere& sphere) :
    _sphere(sphere)
{
    setNightColorDarkness(NightColorDarkness_High);
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


std::vector<float>* SphereRenderer::_constructMainSphereVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

	float polygonIncrement = _getPolygonIncrement();

	float alpha_inc = float(2 * M_PI) / polygonIncrement;
    float theat_inc = float(M_PI) / (polygonIncrement / 2);

    int numFloats = int((2 * M_PI / alpha_inc) * (M_PI / theat_inc)) * 10;
    printf("numFloats = %d\n", numFloats);

    v->reserve(numFloats);

    for (float alpha = 0; alpha < float(2 * M_PI); alpha += alpha_inc)
    {
        for (float theta = 0; theta < float(M_PI); theta += theat_inc)
        {
            float x1 = s._radius * sin(theta) * cos(alpha);
            float y1 = s._radius * sin(theta) * sin(alpha);
            float z1 = s._radius * cos(theta);
            glm::vec3 N1 = glm::normalize(glm::vec3(x1, y1, z1) - glm::vec3(0.0f, 0.0f, 0.0f));

            float x2 = s._radius * sin(theta + theat_inc) * cos(alpha);
            float y2 = s._radius * sin(theta + theat_inc) * sin(alpha);
            float z2 = s._radius * cos(theta + theat_inc);
            glm::vec3 N2 = glm::normalize(glm::vec3(x2, y2, z2) - glm::vec3(0.0f, 0.0f, 0.0f));

            float x3 = s._radius * sin(theta) * cos(alpha + alpha_inc);
            float y3 = s._radius * sin(theta) * sin(alpha + alpha_inc);
            float z3 = s._radius * cos(theta);
            glm::vec3 N3 = glm::normalize(glm::vec3(x3, y3, z3) - glm::vec3(0.0f, 0.0f, 0.0f));

            float x4 = s._radius * sin(theta + theat_inc) * cos(alpha + alpha_inc);
            float y4 = s._radius * sin(theta + theat_inc) * sin(alpha + alpha_inc);
            float z4 = s._radius * cos(theta + theat_inc);
            glm::vec3 N4 = glm::normalize(glm::vec3(x4, y4, z4) - glm::vec3(0.0f, 0.0f, 0.0f));

            vector_push_back_10(*v, x1, y1, z1, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z);
            vector_push_back_10(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z);
            vector_push_back_10(*v, x3, y3, z3, s._r, s._g, s._b, 1.0f, N3.x, N3.y, N3.z);
            vector_push_back_10(*v, x3, y3, z3, s._r, s._g, s._b, 1.0f, N3.x, N3.y, N3.z);
            vector_push_back_10(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z);
            vector_push_back_10(*v, x4, y4, z4, s._r, s._g, s._b, 1.0f, N4.x, N4.y, N4.z);

            //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
        }
    }

    return v;
}

std::pair<std::vector<float>*, std::vector<Triangle>*>  SphereRenderer::_constructMainIcoSphereVertices()
{
    std::vector<float>* v = new std::vector<float>();
    Sphere& s = _sphere;

    IndexedMesh indexMesh = make_icosphere(_getIcoSphereSubdivisionLevel());
    
    // .first is VertexList; which is vector of glm::vec3
    for (int i = 0; i < indexMesh.first->size(); i++)
    {
        glm::vec3& vertex = (*indexMesh.first)[i];

        // Generate normal for this vertex
        glm::vec3 N = glm::normalize(glm::vec3(vertex.x, vertex.y, vertex.z) - glm::vec3(0.0f, 0.0f, 0.0f));

        // scale vertices with unit radius to the actual radius of sphere.
        vertex *= s._radius;

        vector_push_back_10(*v, vertex.x, vertex.y, vertex.z, s._r, s._g, s._b, 1.0f, N.x, N.y, N.z);
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
    for (float alphas = 0.0f; alphas < 180.0f; alphas += 10.0f)
    {
        float alpha = glm::radians(alphas);

        for (float theta = 0; theta < float(2 * M_PI); theta += inc)
        {
            float x1 = 1.001f * s._radius * sin(theta) * cos(alpha);
            float y1 = 1.001f * s._radius * sin(theta) * sin(alpha);
            float z1 = 1.001f * s._radius * cos(theta);
            glm::vec3 N1 = glm::normalize(glm::vec3(x1, y1, z1) - glm::vec3(0.0f, 0.0f, 0.0f));

            float x2 = 1.001f * s._radius * sin(theta + inc) * cos(alpha);
            float y2 = 1.001f * s._radius * sin(theta + inc) * sin(alpha);
            float z2 = 1.001f * s._radius * cos(theta + inc);
            glm::vec3 N2 = glm::normalize(glm::vec3(x2, y2, z2) - glm::vec3(0.0f, 0.0f, 0.0f));

            float cMult;
            if (alpha == 0.0f)
                cMult = 0.8f;
            else
                cMult = 0.9f;

            vector_push_back_10(*v, x1, y1, z1, s._r*cMult, s._g*cMult, s._b*cMult, 1.0, N1.x, N1.y, N1.z);
            vector_push_back_10(*v, x2, y2, z2, s._r*cMult, s._g*cMult, s._b*cMult, 1.0, N2.x, N2.y, N2.z);

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
            float x1 = 1.001f * s._radius * sin(theta) * cos(alpha);
            float y1 = 1.001f * s._radius * sin(theta) * sin(alpha);
            float z1 = 1.001f * s._radius * cos(theta);
            glm::vec3 N1 = glm::normalize(glm::vec3(x1, y1, z1) - glm::vec3(0.0f, 0.0f, 0.0f));

            float x2 = 1.001f * s._radius * sin(theta) * cos(alpha + inc);
            float y2 = 1.001f * s._radius * sin(theta) * sin(alpha + inc);
            float z2 = 1.001f * s._radius * cos(theta);
            glm::vec3 N2 = glm::normalize(glm::vec3(x2, y2, z2) - glm::vec3(0.0f, 0.0f, 0.0f));

            vector_push_back_10(*v, x1, y1, z1, s._r*0.9f, s._g*0.5f, s._b*0.5f, 1.0f, N1.x, N1.y, N1.z);
            vector_push_back_10(*v, x2, y2, z2, s._r*0.9f, s._g*0.5f, s._b*0.5f, 1.0f, N2.x, N2.y, N2.z);

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

        for (float alpha = 0; alpha < float(2 * M_PI) - inc; alpha += inc)
        {
            float x1 = 1.001f * s._radius * sin(theta) * cos(alpha);
            float y1 = 1.001f * s._radius * sin(theta) * sin(alpha);
            float z1 = 1.001f * s._radius * cos(theta);
            glm::vec3 N1 = glm::normalize(glm::vec3(x1, y1, z1) - glm::vec3(0.0f, 0.0f, 0.0f));

            float x2 = 1.001f * s._radius * sin(theta) * cos(alpha + inc);
            float y2 = 1.001f * s._radius * sin(theta) * sin(alpha + inc);
            float z2 = 1.001f * s._radius * cos(theta);
            glm::vec3 N2 = glm::normalize(glm::vec3(x2, y2, z2) - glm::vec3(0.0f, 0.0f, 0.0f));

            vector_push_back_10(*v, x1, y1, z1, s._r*cMult, s._g*cMult, s._b*cMult, 1.0f, N1.x, N1.y, N1.z);
            vector_push_back_10(*v, x2, y2, z2, s._r*cMult, s._g*cMult, s._b*cMult, 1.0f, N2.x, N2.y, N2.z);

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

    //---------------------------------------------------------------------------------
    // Orbital plane.  This is centered at origin.
    //---------------------------------------------------------------------------------
    vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, 1.0f);
    vector_push_back_7(*v, +radius * 1.2f, -radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, 1.0f);
    vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, 1.0f);
    vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, 1.0f);
    vector_push_back_7(*v, -radius * 1.2f, +radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, 1.0f);
    vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r*m, color.g*m, color.b*m, 1.0f);

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
    printf("inc = %f\n", inc);

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

    numMainSphereVertices = v->size() / PLANET_STRIDE_IN_VBO;
    numMainSphereElements = e->size() * 3;
    
    delete v;
    delete e;
#endif

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
		return 1800.0;
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

PlanetRenderer::PlanetRenderer(Sphere& sphere)
	: SphereRenderer(sphere)
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
	glslProgram.setFloat("nightColorMultiplier", _nightColorMultiplier);
    glslProgram.setFloat("sphereInfo.sineOfSelfUmbraConeHalfAngle", sineOfSelfUmbraConeHalfAngle);

	//glEnable(GL_MULTISAMPLE);

	if (otherSphere != nullptr)
	{
		// When drawing earth, other sphere is moon.
		glslProgram.setFloat("otherSphereRadius", otherSphere->getRadius());
		glslProgram.setVec3("otherSphereCenterTransformed", glm::value_ptr(otherSphere->getModelTransformedCenter()));
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



SunRenderer::SunRenderer(Sphere& sphere)
	: SphereRenderer(sphere)
{
}

SunRenderer::~SunRenderer()
{

}

void SunRenderer::renderSphere(GlslProgram& glslProgram)
{
    glslProgram.setMat4("model", glm::value_ptr(_sphere.getModelMatrix()));

	glBindVertexArray(_mainVao);

	// Draw vertices
#ifndef USE_ICOSPHERE
    glDrawArrays(GL_TRIANGLES, 0, numMainSphereVertices);
#else
    glDrawElements(GL_TRIANGLES, numMainSphereElements, GL_UNSIGNED_INT, 0);
#endif
}