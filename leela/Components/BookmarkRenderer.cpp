#include "BookmarkRenderer.h"
#include "Universe.h"
#include "TessellationHelper.h"

bool BookmarkRenderer::isSpherePointHidden(glm::vec3 p)
{
    VECTOR SC = VECTOR(g_space->S, _bookmark->_sphericalBody->_center);
    VECTOR Sp = VECTOR(g_space->S, p);

    // length-squared of tangent using pythagorean theorem.
    float tangentLengthSquared = (SC.length() * SC.length())  -  (double(_bookmark->_sphericalBody->_radius) * double(_bookmark->_sphericalBody->_radius));
    if (tangentLengthSquared < Sp.length() * Sp.length())
        return true;

    return false;
}

void BookmarkRenderer::_renderBookmarks(GlslProgram& glslProgram)
{
    //glm::mat4 combinedMatrix = g_universe->projectionMatrix * g_universe->viewMatrix;
    //glslProgram.setMat4("projection", glm::value_ptr(combinedMatrix));

    glm::mat4 projection = glm::ortho(float(g_universe->curViewportX),
                                      float(g_universe->curViewportX + g_universe->curViewportWidth),
                                      float(g_universe->curViewportY),
                                      float(g_universe->curViewportY + g_universe->curViewportHeight),
                                      0.0f,
                                      100.0f);
    glslProgram.setMat4("projection", glm::value_ptr(projection));
    

    // adjust scale based on zoom level to ensure font size is not too large.
    float heightOfCharA = g_universe->getHeightOfCharA();
    float screenProjectedHeight = g_universe->findScreenProjectedHeight(heightOfCharA, _bookmark->_sphericalBody->_center);
    //spdlog::info("heightOfCharA = {}", heightOfCharA);
    //spdlog::info("screenProjectedHeight of A = {}", screenProjectedHeight);

    float fontScale = 1.0;
    //if (screenProjectedHeight > 10.0) {
    //    // adjust scale such that screen projected height is 10
    //    fontScale = 10.0 / screenProjectedHeight;
    //}

    glm::vec3 projected;
    glm::vec3 bookmarkPoint;
    float radius = _bookmark->_sphericalBody->_radius;

    bookmarkPoint = _bookmark->_sphericalBody->getTransformedLatitudeLongitude(_bookmark->_lat, _bookmark->_lon, 1.0f);
    if (!isSpherePointHidden(bookmarkPoint))
    {
        projected = g_universe->getScreenCoordinates(bookmarkPoint);
        //projected = _sphere.getTransformedLatitudeLongitude(aus_lat, aus_lon, (_sphere._radius + 1) / _sphere._radius);
        if (projected.z < 1.0f)
        {
            g_universe->RenderText(
                glslProgram,
                //RenderTextType_ObjectText,
                RenderTextType_ScreenText,
                _bookmark->_label.c_str(),
                projected.x + heightOfCharA / 2,
                projected.y - heightOfCharA / 2,
                projected.z,
                fontScale,
                glm::vec3(1.0f, 1.0f, 0.0f));
        }
    }
}


void BookmarkRenderer::_renderBookmarkSpheres(GlslProgram& glslProgram)
{
    glm::mat4 projection = glm::ortho(float(g_universe->curViewportX),
                                      float(g_universe->curViewportX + g_universe->curViewportWidth),
                                      float(g_universe->curViewportY),
                                      float(g_universe->curViewportY + g_universe->curViewportHeight),
                                      0.0f,
                                      100.0f);
    glslProgram.setMat4("projection", glm::value_ptr(projection));

    glm::vec3 projected;
    glm::vec3 bookmarkPoint;


    bookmarkPoint = _bookmark->_sphericalBody->getTransformedLatitudeLongitude(_bookmark->_lat, _bookmark->_lon, 1.0f);

    if (!isSpherePointHidden(bookmarkPoint))
    {
        projected = g_universe->getScreenCoordinates(bookmarkPoint);
        //spdlog::info("projected.z = {}", projected.z);

        GLboolean curDepthMaskEnable;

        glGetBooleanv(GL_DEPTH_WRITEMASK, &curDepthMaskEnable);       // backup current depth mask before disabling it
        glDepthMask(GL_FALSE);            // disable writing to depth buffer.  This will allow other objects (spheres, etc) to
        // overwrite the label when they are drawn later.

        if (projected.z < 1.0f)
        {
            glslProgram.setVec3("offset", glm::value_ptr(projected));

            //spdlog::info("Binding vertex and drawing bookmark spheres");
            glBindVertexArray(_bookmarkVao);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numBookmarkSphereVertices);
        }

        glDepthMask(curDepthMaskEnable);    // restore depth mask

    }

}


void BookmarkRenderer::init()
{
    vector<float> * v = ConstructSphereVertices(
                            5,                             // constant radius. Sphere will be drawn near the screen
                            glm::vec3(1.0f, 0.0f, 0.0f),
                            120,
                            false
    );

    int stride_in_vbo = 10;

    //spdlog::info("BookmarkRenderer:  sending spheres to GPU");

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


void BookmarkRenderer::renderMain(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_BookmarkSphere)
    {
        if (_bookmark->_sphericalBody->bShowCityBookmarks)
            _renderBookmarkSpheres(glslProgram);
    }
}

void BookmarkRenderer::renderFinal(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_Font)
    {
        if (_bookmark->_sphericalBody->bShowCityBookmarks)
            _renderBookmarks(glslProgram);
    }
}