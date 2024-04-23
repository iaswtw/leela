#pragma once

#include "Renderer.h"

class SphericalBody;
class SphericalBodyRenderer;

class LatLonRenderer : public Renderer
{
public:
	LatLonRenderer() {}
	~LatLonRenderer() {}

	virtual void init();
	virtual void advance(float stepMultiplier);
	virtual void parentChanged();

	void constructVerticesAndSendToGpu();
	void constructLatitudesAndLongitudeVertices();
	void constructSpecialLatitudesAndLongitudeVertices();

	virtual void renderMain(GlslProgram& glslProgram);
	//virtual void renderPost(GlslProgram& glslProgram);
	void renderLatitudeAndLongitudes(GlslProgram& glslProgram);

public:
	bool bShowLatitudesAndLongitudes = true;

	GLuint _latAndLongVao = 0;
	GLuint _latAndLongVbo = 0;
	GLuint _specialLatAndLongVao = 0;
	GLuint _specialLatAndLongVbo = 0;

	size_t numLatAndLongVertices = 0;
	size_t numSpecialLatAndLongVertices = 0;

	SphericalBodyRenderer* _sphericalBodyRenderer = nullptr;
	SphericalBody* _sphere = nullptr;

};