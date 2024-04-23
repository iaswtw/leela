#pragma once

#include "SphericalBody.h"
#include "Renderer.h"
#include "GlslProgram.h"
#include <vector>
#include "CityBookmark.h"

class BookmarkRenderer : public Renderer
{
public:
	BookmarkRenderer() {}
	~BookmarkRenderer() {}

	virtual void parentChanged() {
		_bookmark = dynamic_cast<Bookmark*>(_sceneParent);
	}

	bool isSpherePointHidden(glm::vec3 p);
	void _renderBookmarks(GlslProgram& glslProgram);
	void _renderBookmarkSpheres(GlslProgram& glslProgram);

	void init();
	void advance(float stepMultiplier) {}
	virtual void renderMain(GlslProgram& glslProgram);
	virtual void renderFinal(GlslProgram& glslProgram);

public:
	GLuint _bookmarkVao = 0;
	GLuint _bookmarkVbo = 0;
	size_t numBookmarkSphereVertices = 0;

	Bookmark * _bookmark = nullptr;
};