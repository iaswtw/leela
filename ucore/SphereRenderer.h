#pragma once

#include "sphere.h"

// GLM includes
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "OglHandles.h"


enum PolygonCountLevel
{
	PolygonCountLevel_Low,
	PolygonCountLevel_Medium,
	PolygonCountLevel_High
} ;

enum NightColorDarkness
{
    NightColorDarkness_Black,
    NightColorDarkness_VeryHigh,        // close to black
    NightColorDarkness_High,
    NightColorDarkness_Medium,
    NightColorDarkness_Low,             
    NightColorDarkness_VeryLow,         // close to day color
    NightColorDarkness_None,            // night vertex color same as day color
};

class SphereRenderer
{
public:
    SphereRenderer(Sphere& sphere);
    ~SphereRenderer();

    void setAsLightSource();
    void setNightColorDarkness(NightColorDarkness darkness);
	void setPolygonCountLevel(PolygonCountLevel polygonCountLevel);
    void constructVerticesAndSendToGpu(OglHandles oglHandles);
    void render(OglHandles oglHandles, Sphere* otherSphere = nullptr);

public:
    bool bShowOrbitalPlane = false;
    bool bShowOrbit = true;
    bool bShowLatitudesAndLongitudes = false;


private:
    std::vector<float>* _constructMainSphereVertices();
    std::vector<float>* _constructLatitudesAndLongitudeVertices();
    std::vector<float>* _constructOrbit();
    std::vector<float>* _constructOrbitalPlaneVertices();
    std::vector<float>* _constructOrbitalPlaneGridVertices();

	float _getPolygonIncrement();

private:
    Sphere& _sphere;

    GLuint _mainVao;
    GLuint _orbitalPlaneVao;
    GLuint _orbitalPlaneGridVao;            // grid lines in the orbital plane
    GLuint _orbitVao;
    GLuint _latAndLongVao;

    int numMainSphereVertices = -1;
    int numLatAndLongVertices = -1;
    int numOrbitalPlaneVertices = -1;
    int numOrbitalPlaneGridVertices = -1;
    int numOrbitVertices = -1;

	PolygonCountLevel _polygonCountLevel;
    NightColorDarkness _nightColorDarkness;
    float _nightColorMultiplier;

    bool _bIsLightSource = false;
};

