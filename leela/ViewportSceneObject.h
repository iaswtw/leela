#pragma once

#include "SceneObject.h"
#include "UniverseMinimal.h"

class ViewportSceneObject : public SceneObject
{
public:
	ViewportSceneObject(ViewportType);
	~ViewportSceneObject() {}

	void init() {}
	void advance(float stepMultiplier) {}
	void setDimensions(int x, int y, int w, int h);
	void setWidth(int w);

	int _x = 0;
	int _y = 0;
	int _w = 800;
	int _h = 600;
	float _aspectRatio = 1.0;

	bool bChanged = true;
	bool bEnabled = false;
	
	ViewportType _viewportType = ViewportType::Primary;
};