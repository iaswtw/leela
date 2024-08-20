#pragma once

#include "Component.h"
#include "GlslProgram.h"
#include <spdlog/spdlog.h>
#include "UniverseMinimal.h"

//
// Render certain aspects of a scene object.
// 
// A given rendering call is a single combinations of the following 3 things:
//	- rendering stage 
//  - Glsl program type that is currently running when the call is made
//  - scene / Viewport type (primary, minimap, alternate observer, etc.)
//
class Renderer : public Component
{
public:
	Renderer() {}

	virtual void render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram) = 0;

};