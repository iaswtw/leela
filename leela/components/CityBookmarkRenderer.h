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

	void init() {}
	void advance(float stepMultiplier) {}
	virtual void renderPost(GlslProgram& glslProgram);

public:
	CityBookmark * _cityBookmark = nullptr;
};