#pragma once

#include "SceneObject.h"
#include "SimpleSphere.h"


class Bookmark : public SceneObject
{
public:
	Bookmark(SphericalBody * sphericalBody) : _sphericalBody(sphericalBody) {}

	void init() {}
	void advance(float stepMultiplier) {}

	void set(std::string label, float lat, float lon)
	{
		_label = label; _lat = lat; _lon = lon;
	}


	SphericalBody * _sphericalBody = nullptr;
	std::string _label = "";
	float _lat = 0.0f;
	float _lon = 0.0f;

	//bool bShowBookmarks = false;
};