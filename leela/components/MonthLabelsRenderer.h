#pragma once

#include <vector>
#include "Renderer.h"
#include "GlslProgram.h"
#include "SphericalBody.h"
#include "UniverseMinimal.h"


class MonthLabelsRenderer : public Renderer
{
public:
	MonthLabelsRenderer() {}
    ~MonthLabelsRenderer() {}

    void init();

    virtual void parentChanged();

    void advance(float stepMultiplier) {}

    void calculateMonthPositions(float labelPositionScale);

    void _renderLabels(GlslProgram& glslProgram, bool isPre);

    virtual void renderPre(GlslProgram& glslProgram);
    virtual void renderPost(GlslProgram& glslProgram);

private:
	SphericalBody* _sphere;

    std::vector<glm::vec3> monthPositions = {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
    };

};