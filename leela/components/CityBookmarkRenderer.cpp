#include "CityBookmarkRenderer.h"
#include "Universe.h"
#include "TessellationHelper.h"

bool CityBookmarkRenderer::isSpherePointHidden(glm::vec3 p)
{
    VECTOR SC = VECTOR(g_space->S, _cityBookmark->_sphericalBody->_center);
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
    float screenProjectedHeight = g_universe->findScreenProjectedHeight(heightOfCharA, _cityBookmark->_sphericalBody->_center);
    //spdlog::info("heightOfCharA = {}", heightOfCharA);
    //spdlog::info("screenProjectedHeight of A = {}", screenProjectedHeight);

    float fontScale = 1.0;
    //if (screenProjectedHeight > 10.0) {
    //    // adjust scale such that screen projected height is 10
    //    fontScale = 10.0 / screenProjectedHeight;
    //}

    glm::vec3 projected;
    glm::vec3 bookmarkPoint;
    float radius = _cityBookmark->_sphericalBody->_radius;

    bookmarkPoint = _cityBookmark->_sphericalBody->getTransformedLatitudeLongitude(_cityBookmark->_lat, _cityBookmark->_lon, 1.0f);
    if (!isSpherePointHidden(bookmarkPoint))
    {
        projected = g_universe->getScreenCoordinates(bookmarkPoint);
        //projected = _sphere.getTransformedLatitudeLongitude(aus_lat, aus_lon, (_sphere._radius + 1) / _sphere._radius);
        if (projected.z < 1.0f)
        {
            g_universe->RenderText(
                //RenderTextType_ObjectText,
                RenderTextType_ScreenText,
                _cityBookmark->_label.c_str(),
                projected.x + heightOfCharA / 2,
                projected.y - heightOfCharA / 2,
                projected.z,
                fontScale,
                glm::vec3(1.0f, 1.0f, 0.0f));
        }
    }
}


void CityBookmarkRenderer::_renderBookmarkSpheres(GlslProgram& glslProgram)
{
    glm::mat4 projection = glm::ortho(0.0f, float(g_universe->curWidth), 0.0f, float(g_universe->curHeight));
    glslProgram.setMat4("projection", glm::value_ptr(projection));

    glm::vec3 projected;
    glm::vec3 bookmarkPoint;
    float radius = _cityBookmark->_sphericalBody->_radius;

    bookmarkPoint = _cityBookmark->_sphericalBody->getTransformedLatitudeLongitude(_cityBookmark->_lat, _cityBookmark->_lon, 1.0f);
    if (!isSpherePointHidden(bookmarkPoint))
    {
        projected = g_universe->getScreenCoordinates(bookmarkPoint);
        //projected = _sphere.getTransformedLatitudeLongitude(aus_lat, aus_lon, (_sphere._radius + 1) / _sphere._radius);
        if (projected.z < 1.0f)
        {
            //-----------------------
            // TODO setting the model matrix may be needed to transform the sphere properly
            //-----------------------
            glm::mat4 model = glm::translate(glm::mat4(1.0), glm::vec3(projected.x, projected.y, projected.z - 10));
            glslProgram.setMat4("model", glm::value_ptr(model));

            glm::vec3 S = glm::vec3(float(g_universe->curWidth / 2), float(g_universe->curHeight / 2), 0.0f);
            glslProgram.setVec3("S", glm::value_ptr(S));

            //spdlog::info("Binding vertex and drawing bookmark spheres");
            glBindVertexArray(_bookmarkVao);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numBookmarkSphereVertices);
        }
    }

}


void CityBookmarkRenderer::init()
{
    vector<float> * v = ConstructSphereVertices(
                            9,                             // constant radius. Sphere will be drawn near the screen
                            glm::vec3(0.5f, 0.1f, 0.1f),
                            20,
                            false);

    int stride_in_vbo = 10;

    //spdlog::info("CityBookmarkRenderer:  sending spheres to GPU");

    if (_bookmarkVbo != 0) {
        glDeleteBuffers(1, &_bookmarkVbo);
    }
    if (_bookmarkVao != 0) {
        glDeleteVertexArrays(1, &_bookmarkVao);
    }

    glGenVertexArrays(1, &_bookmarkVao);
    glBindVertexArray(_bookmarkVao);
    glGenBuffers(1, &_bookmarkVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _bookmarkVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride_in_vbo * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride_in_vbo * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride_in_vbo * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);


    numBookmarkSphereVertices = v->size() / stride_in_vbo;

    delete v;
    
}


void CityBookmarkRenderer::renderMain(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_BookmarkSphere)
    {
        if (g_universe->bShowCityBookmarks)
            _renderBookmarkSpheres(glslProgram);
    }
}

void CityBookmarkRenderer::renderPost(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_Font)
    {
        if (g_universe->bShowCityBookmarks)
            _renderBookmarks(glslProgram);
    }
}