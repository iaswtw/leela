#pragma once

#include "SphericalBody.h"
#include "Renderer.h"
#include "GlslProgram.h"
#include <vector>
#include "CityBookmark.h"

class CityBookmarkRenderer : public Renderer
{
public:
	CityBookmarkRenderer() {}
	~CityBookmarkRenderer() {}

	virtual void parentChanged() {
		_cityBookmark = dynamic_cast<CityBookmark*>(_sceneParent);
	}

	bool isSpherePointHidden(glm::vec3 p);
	void _renderBookmarks(GlslProgram& glslProgram);
	void _renderBookmarkSpheres(GlslProgram& glslProgram);

	void init();
	void advance(float stepMultiplier) {}
	virtual void renderMain(GlslProgram& glslProgram);
	virtual void renderPost(GlslProgram& glslProgram);

public:
	GLuint _bookmarkVao = 0;
	GLuint _bookmarkVbo = 0;
	size_t numBookmarkSphereVertices = 0;

	CityBookmark * _cityBookmark = nullptr;
};