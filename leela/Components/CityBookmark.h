#pragma once

#include "SceneObject.h"
#include "SimpleSphere.h"

class SphericalBody;

class Bookmark : public SceneObject
{
public:
	Bookmark() {}

	void init() {}
	void advance(float stepMultiplier) {}
	virtual void parentChanged();

	void set(std::string label, float lat, float lon)
	{
		_label = label; _lat = lat; _lon = lon;
		_name = _label;		// label is the name of this scene object
	}


	SphericalBody * _sphericalBody = nullptr;
	std::string _label = "";
	float _lat = 0.0f;
	float _lon = 0.0f;
};