#pragma once

#include "sphere.h"

// GLM includes
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "GlslProgram.h"

struct Triangle
{
    unsigned int vertex[3];
    Triangle(unsigned int first, unsigned int second, unsigned int third)
    {
        vertex[0] = first;
        vertex[1] = second;
        vertex[2] = third;
    }
};

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
    void constructVerticesAndSendToGpu();


public:
    bool bShowOrbitalPlane = false;
    bool bShowOrbit = true;
    bool bShowLatitudesAndLongitudes = false;


private:
    std::vector<float>* _constructMainSphereVertices();
    std::pair<std::vector<float>*, std::vector<Triangle>*> _constructMainIcoSphereVertices();
    std::vector<float>* _constructLatitudesAndLongitudeVertices();
    std::vector<float>* _constructOrbit();
    std::vector<float>* _constructOrbitalPlaneVertices();
    std::vector<float>* _constructOrbitalPlaneGridVertices();

	float _getPolygonIncrement();
    int _getIcoSphereSubdivisionLevel();

protected:
    Sphere& _sphere;

    GLuint _mainVao;
    GLuint _orbitalPlaneVao;
    GLuint _orbitalPlaneGridVao;            // grid lines in the orbital plane
    GLuint _orbitVao;
    GLuint _latAndLongVao;

    int numMainSphereVertices = -1;
    int numMainSphereElements = -1;
    int numLatAndLongVertices = -1;
    int numOrbitalPlaneVertices = -1;
    int numOrbitalPlaneGridVertices = -1;
    int numOrbitVertices = -1;

	PolygonCountLevel _polygonCountLevel;
    NightColorDarkness _nightColorDarkness;
    float _nightColorMultiplier;

    bool _bIsLightSource = false;
};


class PlanetRenderer : public SphereRenderer
{
public:
	PlanetRenderer(Sphere& sphere);
	~PlanetRenderer();

	void renderSphere(GlslProgram& glslProgram, Sphere* sun, Sphere* otherSphere = nullptr);
	void renderLatitudeAndLongitudes(GlslProgram& glslProgram);
	void renderOrbitalPlane(GlslProgram& glslProgram);
	void renderOrbit(GlslProgram& glslProgram);

};

class SunRenderer : public SphereRenderer
{
public:
	SunRenderer(Sphere& sphere);
	~SunRenderer();

	void renderSphere(GlslProgram& glslProgram);

};