#pragma once

#include "SceneObject.h"

class SimpleSphere : SceneObject
{
public:
	SimpleSphere() {}
	~SimpleSphere() {}


	float _radius = 1.0f;
	glm::vec3 color = { 0.0f, 0.0f, 0.0f };
};