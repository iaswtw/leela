#pragma once

#include "SceneObject.h"

class SimpleSphere : SceneObject
{
public:
	SimpleSphere() {}
	~SimpleSphere() {}

	void setPositionOffsetFromParent(glm::vec3 offset) { positionOffset = offset; }

	virtual void init() {}
	virtual void advance(float stepMultiplier) {}

	virtual glm::mat4 getPositionTransform() {
		glm::mat4 mat(1.0f);

		//-------------------------------------------------------------------
		// Transformation matrix is created in reverse order of intended application to the transformed point.
		//-------------------------------------------------------------------

		// parent is assumed to have been updated.  Translate by an amount equal to the parent's position.
		if (_sceneParent != nullptr)
			mat = _sceneParent->getPositionTransform();

		// TODO transform this simple sphere from t he parent's position.
		mat = glm::translate(mat, positionOffset);

		return mat;
	}

	glm::vec3 positionOffset = {0.0f, 0.0f, 0.0f};			// offset from parent's position

	float _radius = 1.0f;
	glm::vec3 color = { 0.0f, 0.0f, 0.0f };
};