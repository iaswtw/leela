#include "CityBookmarkRenderer.h"
#include "Universe.h"


bool CityBookmarkRenderer::isSpherePointHidden(glm::vec3 p)
{
    VECTOR SC = VECTOR(g_space->S, _sphere._center);
    //PNT U = PNT(_sphere._center).translated(_sphere._radius, g_space->DR).toVec3();     // point on sphere with roughly the most separation from observer's point of view.

    //VECTOR SU = VECTOR(g_space->S, U);
    VECTOR Sp = VECTOR(g_space->S, p);

    //float max_angle = SU.angleFrom(SC, SU);
    //float angle = SU.angleFrom(SC, SU);

    //if (angle < max_angle)
    //{
    //}

    //spdlog::info("Sp.length() = {}", Sp.length());
    //spdlog::info("SC.length() = {}", SC.length());
    if (Sp.length() > SC.length())
        return true;
    
    return false;
}

void CityBookmarkRenderer::_renderBookmarks(GlslProgram& glslProgram)
{
    //glm::mat4 combinedMatrix = g_universe->projectionMatrix * g_universe->viewMatrix;
    //glslProgram.setMat4("projection", glm::value_ptr(combinedMatrix));

    glm::mat4 projection = glm::ortho(0.0f, float(g_universe->curWidth), 0.0f, float(g_universe->curHeight));
    glslProgram.setMat4("projection", glm::value_ptr(projection));
    

    // adjust scale based on zoom level to ensure font size is not too large.
    float heightOfCharA = g_universe->getHeightOfCharA();
    float screenProjectedHeight = g_universe->findScreenProjectedHeight(heightOfCharA, _sphere._center);
    //spdlog::info("heightOfCharA = {}", heightOfCharA);
    //spdlog::info("screenProjectedHeight of A = {}", screenProjectedHeight);

    float fontScale = 1.0;
    //if (screenProjectedHeight > 10.0) {
    //    // adjust scale such that screen projected height is 10
    //    fontScale = 10.0 / screenProjectedHeight;
    //}

    glm::vec3 projected;
    glm::vec3 bookmarkPoint;

    for (BookmarkInfo bi : _cityBookmarks.bookmarks)
    {

        bookmarkPoint = _sphere.getTransformedLatitudeLongitude(bi.lat, bi.lon, (_sphere._radius + 1) / _sphere._radius);
        if (!isSpherePointHidden(bookmarkPoint))
        {
            projected = g_universe->getScreenCoordinates(bookmarkPoint);
            //projected = _sphere.getTransformedLatitudeLongitude(aus_lat, aus_lon, (_sphere._radius + 1) / _sphere._radius);
            if (projected.z < 1.0f)
            {
                g_universe->RenderText(
                    //RenderTextType_ObjectText,
                    RenderTextType_ScreenText,
                    bi.label.c_str(),
                    projected.x,
                    projected.y,
                    projected.z,
                    fontScale,
                    glm::vec3(1.0f, 1.0f, 0.0f));
            }
        }
    }
}

void CityBookmarkRenderer::renderPost(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_Font)
    {
        _renderBookmarks(glslProgram);
    }
}