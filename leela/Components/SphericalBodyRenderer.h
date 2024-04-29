#pragma once

#include "SphericalBody.h"
#include "Renderer.h"

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
    PolygonCountLevel_YouChoose,
    PolygonCountLevel_VeryLow,
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


class SphericalBodyRenderer : public Renderer
{
public:
    SphericalBodyRenderer(std::string textureFilename="", std::string textureFilename2="");
    ~SphericalBodyRenderer();

    virtual void init();
    virtual void parentChanged();
    virtual void advance(float stepMultiplier) {};

    void setAsLightSource();
    void setNightColorDarkness(NightColorDarkness darkness);
	void setPolygonCountLevel(std::string polygonCountLevel);

    void constructVerticesAndSendToGpu();
    void _constructMainSphereVertices(int numEquatorVertices, GLuint* pVao, GLuint* pVbo, size_t* pNumSphereVertices);
    void _constructMainIcoSphereVertices();
    void constructMainSphereVertices();
    void constructMainSphereVerticesForMinimap();
    //void constructIcoSphereVertices();
    //void constructIcoSphereVerticesForMinimap();
    void constructRotationAxis();
    void constructLongRotationAxis();
    void constructOrbit();
    void constructOrbitalPlaneVertices();
    void constructOrbitalPlaneGridVertices();

    void sendTextureToGpu();

    virtual void doShaderConfig(GlslProgram& glslProgram) {}

    std::string _locateTextureFile(const char * filenName);


public:
    bool bShowOrbit = false;
    bool bOrbitalPlaneTransparency = false;
    bool bShowOrbitalPlane = false;
    bool bShowBody = true;                  // if true, draw the planet/moon


public:
    std::tuple<float, float, float, glm::vec3, float, float> calcPointOnSphere(float radius, float alpha, float theta);


	int _getNumEquatorVertices(PolygonCountLevel polygonCountLevel = PolygonCountLevel_YouChoose);
    int _getIcoSphereSubdivisionLevel();

public:
    SphericalBody * _sphere = nullptr;

    
    GLuint _minimapMainVao = 0;
    GLuint _minimapMainVbo = 0;
    GLuint minimapVbo = 0;     // vertex buffer object
    GLuint minimapEbo = 0;     // element buffer object
    
    size_t numMinimapMainSphereVertices = 0;
    size_t minimapNumMainSphereVertices = 0;
    size_t minimapNumMainSphereElements = 0;

    //-------------------------------------------
    // Primary scene VAOs, VBOs and vertex counts

    GLuint _mainVao = 0;
    GLuint _mainVbo = 0;
    GLuint vbo = 0;     // vertex buffer object
    GLuint ebo = 0;     // element buffer object
    GLuint _orbitVao = 0;
    GLuint _orbitVbo = 0;
    GLuint _rotationAxisVao = 0;
    GLuint _rotationAxisVbo = 0;
    GLuint _longRotationAxisVao = 0;
    GLuint _longRotationAxisVbo = 0;
    GLuint _texture = 0;
    GLuint _texture2 = 0;
    GLuint _orbitalPlaneVao = 0;
    GLuint _orbitalPlaneVbo = 0;
    GLuint _orbitalPlaneGridVao = 0;            // grid lines in the orbital plane
    GLuint _orbitalPlaneGridVbo = 0;

    size_t numMainSphereVertices = 0;
    size_t numMainSphereElements = 0;
    size_t numOrbitVertices = 0;
    size_t numRotationAxisVertices = 0;
    size_t numLongRotationAxisVertices = 0;
    size_t numOrbitalPlaneVertices = 0;
    size_t numOrbitalPlaneGridVertices = 0;

    //-------------------------------------------

	PolygonCountLevel _polygonCountLevel = PolygonCountLevel_Low;
    NightColorDarkness _nightColorDarkness = NightColorDarkness_VeryHigh;
    float _nightColorMultiplier;
    bool bLongAxis = false;         // if true, draw ridiculously long axis from north and south pole

    bool _bIsLightSource = false;
    std::string _textureFilename = "";
    std::string _textureFilename2 = "";

    unsigned char* data = nullptr;
};


class PlanetRenderer : public SphericalBodyRenderer
{
public:
	PlanetRenderer(std::string textureFilename = "", std::string textureFilename2 = "");
	~PlanetRenderer();

    virtual void render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram);
    virtual void doShaderConfig(GlslProgram& glslProgram);

	void renderSphere(GlslProgram& glslProgram);
    void renderMinimapSphere(GlslProgram& glslProgram);
	void renderOrbitalPlane(GlslProgram& glslProgram);
	void renderOrbit(GlslProgram& glslProgram);
    void renderRotationAxis(GlslProgram& glslProgram);
    void renderLongRotationAxis(GlslProgram& glslProgram);

};

class SunRenderer : public SphericalBodyRenderer
{
public:
	SunRenderer(std::string textureFilename = "");
	~SunRenderer();

    virtual void doShaderConfig(GlslProgram& glslProgram);

    virtual void render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram);

	void _renderSphere(GlslProgram& glslProgram);
    void renderMinimapSphere(GlslProgram& glslProgram);
};