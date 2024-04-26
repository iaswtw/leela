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
    virtual void render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram);

private:
	SphericalBody* _sphere = nullptr;

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