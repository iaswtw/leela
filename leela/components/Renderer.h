#pragma once

#include "Component.h"
#include "GlslProgram.h"

// All known renderer types.  Add them here as new renderers are implemented.
typedef enum
{
	RendererType_Sphere,
	RendererType_OrbitalPlane,
	RendererType_OrbitalPlaneGrid,
	RendererType_LatitudesLongitudes,
	RendererType_CoordinateAxis,
	RendererType_SphereAxis,

} RendererType;

//
// Render certain aspects of a scene object.
//
class Renderer : public Component
{
	virtual void render(GlslProgram& glslProgram) = 0;
	virtual void renderTransparent(GlslProgram& glslProgram) {}

	RendererType getRendererType()
	{
		return _rendererType;
	}

private:
	RendererType _rendererType;
};