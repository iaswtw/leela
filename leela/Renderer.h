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
			renderPre(glslProgram);
			break;
		case RenderStage_Main:
			renderMain(glslProgram);
			break;
		case RenderStage_Post:
			renderPost(glslProgram);
			break;
		case RenderStage_Translucent_Main:
			renderTranslucentMain(glslProgram);
			break;
		case RenderStage_Final:
			renderFinal(glslProgram);
		}
	}
	virtual void renderPre(GlslProgram& glslProgram) {}
	virtual void renderMain(GlslProgram& glslProgram) {}
	virtual void renderPost(GlslProgram& glslProgram) {}
	virtual void renderTranslucentMain(GlslProgram& glslProgram) {}
	virtual void renderFinal(GlslProgram& glslProgram) {}
};