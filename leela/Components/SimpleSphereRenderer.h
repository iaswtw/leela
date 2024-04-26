#pragma once

#include "Renderer.h"
class SphericalBody;

class SimpleSphereRenderer : public Renderer
{
public:
	SimpleSphereRenderer(SphericalBody& sphere) : _sphere(sphere) {}
	~SimpleSphereRenderer() {}

	void init();
	void advance(float stepMultiplier) {}

	virtual void render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram);

	SphericalBody& _sphere;

};