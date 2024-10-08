#include "MonthLabelsRenderer.h"

#include "Leela.h"


void MonthLabelsRenderer::init()
{
    // nothing to send to GPU
}

void MonthLabelsRenderer::parentChanged()
{
    _sphere = dynamic_cast<SphericalBody*>(_sceneParent);
}

// Calculate 12 points outside the orbit suitable for drawing month labels
// labelPositionScale - if 1.0, labels are at sphere's orbital radius from the parent's (e.g. sun's) center.
void MonthLabelsRenderer::calculateMonthPositions(float labelPositionScale)
{
    glm::vec3 axis = _sphere->getAxisTiltOrientationAxis();
    glm::vec3 perp = glm::vec3(axis.y, -axis.x, axis.z);

    float scaler = labelPositionScale * _sphere->_orbitalRadius;
    perp = perp * scaler;

    float theta = float(2 * M_PI / 12);
    for (int i = 0; i < 12; i++)
    {
        monthPositions[i].x = perp.x * cos(theta) - perp.y * sin(theta);
        monthPositions[i].y = perp.x * sin(theta) + perp.y * cos(theta);
        monthPositions[i].z = 0;

        //monthPositions[i] += _center;

        theta += float(2 * M_PI / 12);
    }
}

void MonthLabelsRenderer::_renderLabels(GlslProgram& glslProgram, bool isPre)
{
    if ((!isPre && g_leela->bShowLabelsOnTop) || (isPre && !g_leela->bShowLabelsOnTop))
    {
        // This code is executed in either Pre or Post render stage; not both.
        // If labels are to be shown on top, render them in `post` render stage.

        glm::vec3 projected;
        float z = 0.0f;

        if (g_leela->bShowMonthNames)
        {
            if (g_leela->bMonthLabelsCloserToSphere)
                calculateMonthPositions((_sphere->_orbitalRadius + 1.5f * _sphere->_radius) / _sphere->_orbitalRadius);
            else
                calculateMonthPositions(1.2f);


            //----- TEMP ------
            glm::mat4 projection = glm::ortho(float(g_leela->curViewportX), float(g_leela->curViewportX + g_leela->curViewportWidth), float(g_leela->curViewportY), float(g_leela->curViewportY + g_leela->curViewportHeight));
            glslProgram.setMat4("projection", glm::value_ptr(projection));
            //----- TEMP ------


            //glm::vec2 projected = getScreenCoordinates(earth.getModelTransformedCenter());
            //spdlog::info("projected = {}", glm::to_string(projected));

            std::vector<std::string> monthNames = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

            for (int i = 0; i < 12; i++)
            {
                projected = g_leela->getScreenCoordinates(monthPositions[i]);
                //glm::vec2 projected = getScreenCoordinates(glm::vec3(0.0f, 0.0f, 0.0f));

                //spdlog::info("month Position {}: {}", i, glm::to_string(projected));

                if (projected.z < 1.0f)
                {
                    g_leela->RenderText(
                        glslProgram,
                        RenderTextType_ScreenText,
                        monthNames[i].c_str(),
                        projected.x,
                        projected.y,
                        z,
                        1.0f,
                        glm::vec3(1.0f, 1.0f, 0.0f));
                }
            }
        }
        //RenderText(RenderTextType_ScreenText, "Hello World", 100.0f, 100.0f, z, 1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
    }
}

void MonthLabelsRenderer::render(ViewportType viewportType, RenderStage renderStage, GlslProgram& glslProgram)
{
    if (renderStage == RenderStage::Pre) {
        if (glslProgram.type() == GlslProgramType::Font) {
            if (viewportType == ViewportType::Primary) {
                // Labels that are allowed to hide under other objects should be written before the objects.
                // This is because all labels, regardless of what object they are for, are written at a
                // very shallow depth.
                _renderLabels(glslProgram, true);
            }
        }
    }
    else if (renderStage == RenderStage::Post) {
        if (glslProgram.type() == GlslProgramType::Font) {
            if (viewportType == ViewportType::Primary) {
                _renderLabels(glslProgram, false);
            }
        }
    }
}
