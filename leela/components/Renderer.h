#pragma once

#include "Component.h"
#include "GlslProgram.h"
#include <spdlog/spdlog.h>

//
// Render certain aspects of a scene object.
//
class Renderer : public Component
{
public:
	Renderer() {}

	virtual void render(GlslProgram& glslProgram) = 0;
	virtual void renderTransparent(GlslProgram& glslProgram) = 0;
	
	virtual std::vector<GlslProgramType> getNeededGlslProgramTypes() = 0;
	virtual std::vector<GlslProgramType> getNeededTransparentGlslProgramTypes() = 0;

};