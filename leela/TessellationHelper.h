#pragma once

#include <vector>

std::tuple<float, float, float, glm::vec3, float, float>
CalcPointOnSphere(float radius, float alpha, float theta);

std::vector<float>*
ConstructSphereVertices(float radius, glm::vec3 color, int numEquatorVertices, bool bTextureCoord);