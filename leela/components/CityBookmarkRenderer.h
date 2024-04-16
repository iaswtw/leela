#pragma once

#include "sphere.h"
#include "Renderer.h"
#include "GlslProgram.h"
#include <vector>
#include "CityBookmarks.h"

class CityBookmarkRenderer : public Renderer
{
public:
	CityBookmarkRenderer(Sphere& sphere, CityBookmarks& cityBookmarks) : _sphere(sphere), _cityBookmarks(cityBookmarks) {}
	~CityBookmarkRenderer() {}

	bool isSpherePointHidden(glm::vec3 p);
	void _renderBookmarks(GlslProgram& glslProgram);

	void init() {}
	void advance(float stepMultiplier) {}
	virtual void renderPost(GlslProgram& glslProgram);

private:
	Sphere& _sphere;
	CityBookmarks& _cityBookmarks;
};