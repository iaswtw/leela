#pragma once

#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>


struct PlanetInfo
{
    std::string name;
    std::string parentName;
    std::vector<std::string> relatedObjectNames;

    float radius;
    float rotationAngle;
    float rotationAngularVelocity;
    float axisTiltOrientationAngle;
    float axisTiltAngle;

    float orbitalRadius;
    float orbitalAngle;
    float orbitalAngularVelocity;
    float nodalPrecessionInitialAngle;
    float orbitalPlaneTiltAngle;

    glm::vec3 color;

    std::string polygonCountLevel;
    std::string textureFilename;
};

PlanetInfo planetInfo[] = {
    // name         parent      Related     radius  rotation    rotation   axis tilt    axis tilt   orbital orbital orbital     nodal       orbital     color                   polygon     texture filename
    //              name        object              angle       angular    orientation  angle       radius  angle   angular     precession  plane tilt                          count   
    //                          names                           velocity   angle                                    velocity    init angle  angle
    {  "Sun",       "",         {},         160,    0,          0.002f,    0.0f,        0.0f,       0,      0.0f,   0.01f,      0,          0,          {1.0f, 1.0f, 0.6f},     "low",      "sunmap.png"                    },
    {  "Mercury",   "Sun",      {},         10,     0,          0.0004f,   0.0f,        2.0f,       700,    90.0f,  0.004f,     0.0f,       7.0f,       {0.8f, 0.8f, 0.8f},     "medium",   "2k_mercury.png"                },
    {  "Earth",     "Sun",      {"Moon"},   80,     0,          0.02f,     0.0f,        23.5f,      3000,   30.0f,  0.001f,     0.0f,       0,          {0.55f, 0.82f, 1.0f},   "medium",   "earth-erde-mit-wolken-2k.png"  },
    {  "Moon",      "Earth",    {"Earth"},  22,     0,          0.008888f, 0.0f,        10.0f,      400,    0.0f,   0.0088888f, 0,          30.0f,      {0.8f, 0.8f, 0.8f},     "medium",   "moonmap1k.png"                 },
    //{  "Earth2",    "Sun",      {},         80,     0,          0.02f,     0.0f,        23.5f,      3000,   60.0f,  0.001f,     0.0f,       0,          {0.55f, 0.82f, 1.0f},   "medium",   "earth-erde-mit-wolken-2k.png"  },
    //{  "Moon2",      "Earth2",   {"Earth2"}, 22,     0,          0.008888f, 0.0f,        10.0f,      400,    0.0f,   0.0088888f, 0,          30.0f,      {0.8f, 0.8f, 0.8f},     "medium",   "moonmap1k.png"                 },
    {  "Mars",      "Sun",      {},         60,     0,          0.02f,     270.0f,      25.0f,      7000,   0,      0.0003f,    0.0f,       1.85f,      {0.85f, 0.5f, 0.5f},    "low",      "mars2k.png"                    },
    {  "Jupiter",   "Sun",      {},         140,    0,          0.02f,     270.0f,      3.1f,       11000,  280.0f, 0.00012f,    0.0f,       1.31f,      {0.85f, 0.7f, 0.6f},    "low",      "jupiter2k.png"                 },
    {  "Saturn",    "Sun",      {},         110,    0,          0.02f,     270.0f,      26.7f,      15000,  220.0f, 0.00006f,    0.0f,       2.49f,      {0.7f, 0.7f, 0.4f},     "low",      "saturn2k.png"                  },
    {  "Uranus",    "Sun",      {},         90,     0,          0.02f,     270.0f,      97.7f,      20000,  18.0f,  0.00003f,    0.0f,       0.77f,      {0.7f, 0.7f, 0.85f},    "low",      "uranus2k.png"                  },
    {  "Neptune",   "Sun",      {},         80,     0,          0.02f,     270.0f,      28.0f,      25000,  150.0f, 0.000021f,   0.0f,       1.77f,      {0.4f, 0.4f, 0.9f},     "low",      "neptune2k.png"                 }
};

