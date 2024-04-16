#pragma once

#include <vector>
#include "Renderer.h"
#include "GlslProgram.h"
#include "sphere.h"


class MonthLabelsRenderer : public Renderer
{
public:
	MonthLabelsRenderer(Sphere& sphere) : _sphere(sphere)
	{
	}

    ~MonthLabelsRenderer() {}

	std::vector<GlslProgramType> getNeededGlslProgramTypes() {
		return { GlslProgramType_Font };
	}
    std::vector<GlslProgramType> getNeededTransparentGlslProgramTypes() {
        return {};
    }

	void init()
    {
	}

    void advance(float stepMultiplier) {}

    void calculateMonthPositions(float labelPositionScale);

    void render(GlslProgram& glslProgram);
    void renderTransparent(GlslProgram& glslProgram) {};

private:
	Sphere& _sphere;

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