#pragma once

#include "Renderer.h"

class ViewportSceneObject;

class ViewportBorderRenderer : public Renderer
{
public:
	ViewportBorderRenderer() {}
	~ViewportBorderRenderer() {}

	void init();
	void constructBorderVertices();
	void advance(float stepMultiplier) {};
	void parentChanged();

	void render(ViewportType viewportType, RenderStage sceneType, GlslProgram& glslProgram);
	void renderBorder(GlslProgram& glslProgram);

	GLuint _borderVao = 0;
	GLuint _borderVbo = 0;

	size_t numBorderVertices = 0;

	ViewportSceneObject* _viewport = nullptr;
};