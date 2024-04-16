#pragma once

#include "sphere.h"
#include "Renderer.h"
#include "GlslProgram.h"
#include <vector>

class CityBookmarkRenderer : public Renderer
{
public:
	CityBookmarkRenderer(Sphere& sphere) : _sphere(sphere) {}
	~CityBookmarkRenderer() {}

	std::vector<GlslProgramType> getNeededGlslProgramTypes() {
		return { GlslProgramType_Font };
	}

	void init() {}
	void advance(float stepMultiplier) {}
	void render(GlslProgram& glslProgram);

private:
	Sphere& _sphere;
};