#pragma once

#include "Component.h"
#include "GlslProgram.h"
#include <spdlog/spdlog.h>
#include "UniverseMinimal.h"

//
// Render certain aspects of a scene object.
//
class Renderer : public Component
{
public:
	Renderer() {}

	virtual void render(GlslProgram& glslProgram, RenderStage stage) final
	{
		switch (stage)
		{
		case RenderStage_Pre:
			// labels allowed to be overwritten by other objects must be rendered here.
			renderPre(glslProgram);
			break;
		case RenderStage_Main:
			renderMain(glslProgram);
			break;
		case RenderStage_Post:
			renderPost(glslProgram);
			break;
		
		case RenderStage_Translucent_Main:
			// Objects with alpha < 1.0 have to be rendered after all objects with alpha = 1.0
			// It still doesn't solve all problems.
			renderTranslucentMain(glslProgram);
			break;
		
		case RenderStage_Final:
			// Labels that must appear on top of everything else must be rendered here.
			renderFinal(glslProgram);
		}
	}
	virtual void renderPre(GlslProgram& glslProgram) {}
	virtual void renderMain(GlslProgram& glslProgram) {}
	virtual void renderPost(GlslProgram& glslProgram) {}
	virtual void renderTranslucentMain(GlslProgram& glslProgram) {}
	virtual void renderFinal(GlslProgram& glslProgram) {}
};