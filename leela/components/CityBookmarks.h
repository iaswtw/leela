#pragma once

#include "SceneObject.h"

typedef struct 
{
	std::string label;		// city name
	float lat;
	float lon;
} BookmarkInfo;



class CityBookmarks : public SceneObject
{
public:
	CityBookmarks() {}

	void init() {}
	void advance(float stepMultiplier) {}

	std::vector<BookmarkInfo> bookmarks = {
		{ "Austin",			30.2f, -97.7},
		{ "Chicago",		41.8f, -87.6f}
	};

};