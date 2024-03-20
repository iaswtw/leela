#pragma once

#include "sphere.h"

class Universe;

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
    SphereRenderer(Universe& parent, Sphere& sphere, std::string textureFilename = "");
    ~SphereRenderer();

    void setAsLightSource();
    void setNightColorDarkness(NightColorDarkness darkness);
	void setPolygonCountLevel(PolygonCountLevel polygonCountLevel);
    std::vector<float>* _constructRotationAxis();
    void constructVerticesAndSendToGpu();


public:
    bool bShowOrbitalPlane = false;
    bool bShowOrbit = true;
    bool bShowLatitudesAndLongitudes = false;


private:
    std::tuple<float, float, float, glm::vec3, float, float> calcPointOnSphere(float radius, float alpha, float theta);

    std::vector<float>* _constructMainSphereVertices();
    std::pair<std::vector<float>*, std::vector<Triangle>*> _constructMainIcoSphereVertices();
    std::vector<float>* _constructLatitudesAndLongitudeVertices();
    std::vector<float>* _constructOrbit();
    std::vector<float>* _constructOrbitalPlaneVertices();
    std::vector<float>* _constructOrbitalPlaneGridVertices();

	float _getPolygonIncrement();
    int _getIcoSphereSubdivisionLevel();

protected:
    Universe& parent;
    Sphere& _sphere;

    GLuint _mainVao;
    GLuint _orbitalPlaneVao;
    GLuint _orbitalPlaneGridVao;            // grid lines in the orbital plane
    GLuint _orbitVao;
    GLuint _latAndLongVao;
    GLuint _rotationAxisVao;
    GLuint _texture;

    int numMainSphereVertices = -1;
    int numMainSphereElements = -1;
    int numLatAndLongVertices = -1;
    int numOrbitalPlaneVertices = -1;
    int numOrbitalPlaneGridVertices = -1;
    int numOrbitVertices = -1;
    int numRotationAxisVertices = -1;

	PolygonCountLevel _polygonCountLevel;
    NightColorDarkness _nightColorDarkness;
    float _nightColorMultiplier;

    bool _bIsLightSource = false;
    std::string _textureFilename;

    unsigned char* data = nullptr;
};


class PlanetRenderer : public SphereRenderer
{
public:
	PlanetRenderer(Universe& parent, Sphere& sphere, std::string textureFilename = "");
	~PlanetRenderer();

	void renderSphere(GlslProgram& glslProgram, Sphere* sun, Sphere* otherSphere = nullptr);
	void renderLatitudeAndLongitudes(GlslProgram& glslProgram);
	void renderOrbitalPlane(GlslProgram& glslProgram);
	void renderOrbit(GlslProgram& glslProgram);
    void renderRotationAxis(GlslProgram& glslProgram, Sphere* sun, Sphere* otherSphere);

};

class SunRenderer : public SphereRenderer
{
public:
	SunRenderer(Universe& parent, Sphere& sphere, std::string textureFilename = "");
	~SunRenderer();

	void renderSphere(GlslProgram& glslProgram);

};