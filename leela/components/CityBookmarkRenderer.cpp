#include "CityBookmarkRenderer.h"
#include "Universe.h"

void CityBookmarkRenderer::render(GlslProgram& glslProgram)
{
    float aus_lat = 30.2f;      // Austin latitude/longitude
    float aus_lon = -97.7;

    float ord_lat = 41.8f;      // Chicago latitude/longitude
    float ord_lon = -87.6f;


    glm::mat4 combinedMatrix = g_universe->projectionMatrix * g_universe->viewMatrix;
    glslProgram.setMat4("projection", glm::value_ptr(combinedMatrix));


    // adjust scale based on zoom level to ensure font size is not too large.
    float heightOfCharA = g_universe->getHeightOfCharA();
    float screenProjectedHeight = g_universe->findScreenProjectedHeight(heightOfCharA, _sphere._center);
    //spdlog::info("heightOfCharA = {}", heightOfCharA);
    //spdlog::info("screenProjectedHeight of A = {}", screenProjectedHeight);

    float fontScale = 1.0;
    if (screenProjectedHeight > 10.0) {
        // adjust scale such that screen projected height is 10
        fontScale = 10.0 / screenProjectedHeight;
    }

    glm::vec3 projected;

    //projected = getScreenCoordinates(_sphere.getTransformedLatitudeLongitude(aus_lat, aus_lon));
    projected = _sphere.getTransformedLatitudeLongitude(aus_lat, aus_lon, (_sphere._radius + 1) / _sphere._radius);
    g_universe->RenderText(
        RenderTextType_ObjectText,
        "Austin",
        projected.x,
        projected.y,
        projected.z,
        fontScale,
        glm::vec3(1.0f, 1.0f, 0.0f));

    projected = _sphere.getTransformedLatitudeLongitude(ord_lat, ord_lon, (_sphere._radius + 1) / _sphere._radius);
    g_universe->RenderText(
        RenderTextType_ObjectText,
        "Chicago",
        projected.x,
        projected.y,
        projected.z,
        fontScale,
        glm::vec3(1.0f, 1.0f, 0.0f));

}