#pragma once

#include <string>
#include <vector>

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


#define USE_TO_SCALE true
//#define USE_TO_SCALE false


extern PlanetInfo planetInfo[];
extern PlanetInfo planetInfoToScale[];

extern int numPlanetInfo;
extern int numPlanetInfoToScale;
