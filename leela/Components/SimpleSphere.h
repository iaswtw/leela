#pragma once

#include "SceneObject.h"

class SimpleSphere : SceneObject
{
public:
	SimpleSphere(SceneObject& parent) {}
	~SimpleSphere() {}


	virtual void init() {}
	virtual void advance(float stepMultiplier) {}

	virtual glm::mat4 getPositionTransform() {

	}


	float _radius = 1.0f;
	glm::vec3 color = { 0.0f, 0.0f, 0.0f };
};