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
	std::vector<GlslProgramType> getNeededTransparentGlslProgramTypes() {
		return {};
	}

	bool isSpherePointHidden(glm::vec3 p);

	void _renderBookmarks(GlslProgram& glslProgram);

	void init() {}
	void advance(float stepMultiplier) {}
	virtual void renderPost(GlslProgram& glslProgram);

private:
	Sphere& _sphere;
};