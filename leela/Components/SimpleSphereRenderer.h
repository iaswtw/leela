#pragma once

#include "Renderer.h"
class Sphere;

class SimpleSphereRenderer : public Renderer
{
public:
	SimpleSphereRenderer(Sphere& sphere) : _sphere(sphere) {}
	~SimpleSphereRenderer() {}

	void init();
	void advance(float stepMultiplier) {}

	virtual void renderMain(GlslProgram& glslProgram);

	Sphere& _sphere;

};