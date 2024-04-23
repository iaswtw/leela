#include "LatLonRenderer.h"
#include "Utils.h"
#include "TessellationHelper.h"
#include "Universe.h"

void LatLonRenderer::init()
{
    constructVerticesAndSendToGpu();
}


void LatLonRenderer::advance(float stepMultiplier)
{
	// nothing to do here
}

void LatLonRenderer::parentChanged()
{
	_sphere = dynamic_cast<SphericalBody*>(_sceneParent);
}

void LatLonRenderer::constructVerticesAndSendToGpu()
{
    constructLatitudesAndLongitudeVertices();
    constructSpecialLatitudesAndLongitudeVertices();
}

void LatLonRenderer::constructLatitudesAndLongitudeVertices()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;

    // TODO implement _getPolygonIncrement() in one of several ways:
    //  - in this class
    //  - as a mixin into this class.  The same mixin class can be used in SphericalBodyRenderer.
    //  - locate the main renderer class (will have to add infrastructure in SceneObject for this), and this method on it.
    //float polygonIncrement = s._getPolygonIncrement();
    float polygonIncrement = 100;

    float inc = float(2 * float(M_PI)) / polygonIncrement;
    float c_a = 0.4f;       // color alpha
    float c_a2 = 0.8;       // color alpha for special lat/lon

    //---------------------------------------------------------------------------------
    // Longitudes
    //---------------------------------------------------------------------------------
    for (float alpha_deg = 0.0f; alpha_deg < 360.0f; alpha_deg += 10.0f)
    {
        float alpha = glm::radians(alpha_deg);

        for (float theta = 0; theta < float(2 * float(M_PI)); theta += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = CalcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = CalcPointOnSphere(1.001f * s._radius, alpha, theta + inc);

            float cMult;
            if (alpha == 0.0f)
                cMult = 1.0f;
            else
                cMult = 0.7f;

            vector_push_back_12(*v, x1, y1, z1, s._r * cMult, s._g * cMult, s._b * cMult, c_a, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r * cMult, s._g * cMult, s._b * cMult, c_a, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        }
    }

    //---------------------------------------------------------------------------------
    // Latitudes
    //---------------------------------------------------------------------------------
    for (float thetas = 0; thetas < 180; thetas += 10)
    {
        float theta = glm::radians(thetas);
        float cMult = 0.7f;

        for (float alpha = 0; alpha < float(2 * M_PI); alpha += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = CalcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = CalcPointOnSphere(1.001f * s._radius, alpha + inc, theta);

            vector_push_back_12(*v, x1, y1, z1, s._r * cMult, s._g * cMult, s._b * cMult, c_a, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r * cMult, s._g * cMult, s._b * cMult, c_a, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        }
    }

    //---------------------------------------------

    if (_latAndLongVbo != 0) {
        glDeleteBuffers(1, &_latAndLongVbo);
    }
    if (_latAndLongVao != 0) {
        glDeleteVertexArrays(1, &_latAndLongVao);
    }

    glGenVertexArrays(1, &_latAndLongVao);
    glBindVertexArray(_latAndLongVao);
    glGenBuffers(1, &_latAndLongVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _latAndLongVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    numLatAndLongVertices = v->size() / PLANET_STRIDE_IN_VBO;
    delete v;
}

void LatLonRenderer::constructSpecialLatitudesAndLongitudeVertices()
{
    std::vector<float>* v = new std::vector<float>();
    SphericalBody& s = *_sphere;

    // TODO implement _getPolygonIncrement() in one of several ways:
    //  - in this class
    //  - as a mixin into this class.  The same mixin class can be used in SphericalBodyRenderer.
    //  - locate the main renderer class (will have to add infrastructure in SceneObject for this), and this method on it.
    //float polygonIncrement = s._getPolygonIncrement();
    float polygonIncrement = 100;

    float inc = float(2 * float(M_PI)) / polygonIncrement;
    float c_a = 0.5f;       // color alpha
    float c_a2 = 0.5;       // color alpha for special lat/lon

    //---------------------------------------------------------------------------------
    // Special Longitudes
    //---------------------------------------------------------------------------------
    std::list<float> alpha_degs = { 0 };

    for (float alpha_deg : alpha_degs)
    {
        float alpha = glm::radians(alpha_deg);

        for (float theta = 0; theta < float(2 * float(M_PI)); theta += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = CalcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = CalcPointOnSphere(1.001f * s._radius, alpha, theta + inc);

            float cMult;
            if (alpha == 0.0f)
                cMult = 1.0f;
            else
                cMult = 0.7f;

            vector_push_back_12(*v, x1, y1, z1, s._r * 0.9f, s._g * 0.3f, s._b * 0.3f, c_a, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r * 0.9f, s._g * 0.3f, s._b * 0.3f, c_a, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        }
    }

    //---------------------------------------------------------------------------------
    // Special Latitudes
    //---------------------------------------------------------------------------------
    std::list<float> thetas = { 0, 66.5f, 23.5f, 90, 90 + 23.5f, 90 + 66.5f };          // 0 = +z axis, 90 is equator, 90+66.5 is antarctic circle

    for (float theta : thetas)
    {
        theta = glm::radians(theta);

        for (float alpha = 0; alpha < float(2 * float(M_PI)); alpha += inc)
        {
            auto [x1, y1, z1, N1, texX1, texY1] = CalcPointOnSphere(1.001f * s._radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = CalcPointOnSphere(1.001f * s._radius, alpha + inc, theta);

            vector_push_back_12(*v, x1, y1, z1, s._r * 0.9f, s._g * 0.3f, s._b * 0.3f, c_a2, N1.x, N1.y, N1.z, 0.0f, 0.0f);
            vector_push_back_12(*v, x2, y2, z2, s._r * 0.9f, s._g * 0.3f, s._b * 0.3f, c_a2, N2.x, N2.y, N2.z, 0.0f, 0.0f);
        }
    }

    //---------------------------------------------

    if (_specialLatAndLongVbo != 0) {
        glDeleteBuffers(1, &_specialLatAndLongVbo);
    }
    if (_specialLatAndLongVao != 0) {
        glDeleteVertexArrays(1, &_specialLatAndLongVao);
    }

    glGenVertexArrays(1, &_specialLatAndLongVao);
    glBindVertexArray(_specialLatAndLongVao);
    glGenBuffers(1, &_specialLatAndLongVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _specialLatAndLongVbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * v->size(),
        v->data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, PLANET_STRIDE_IN_VBO * sizeof(float), (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    numSpecialLatAndLongVertices = v->size() / PLANET_STRIDE_IN_VBO;
    delete v;
}



void LatLonRenderer::renderPost(GlslProgram& glslProgram)
{
    if (glslProgram.type() == GlslProgramType_Planet)
        renderLatitudeAndLongitudes(glslProgram);
}


void LatLonRenderer::renderLatitudeAndLongitudes(GlslProgram& glslProgram)
{
    SphericalBody& s = *_sphere;

    if (!s.bIsCenterOfMass)
    {
        if (bShowLatitudesAndLongitudes)
        {

            if (!_sphericalBodyRenderer) {
                Component* c = s.getComponentByName("SphericalBodyRenderer");
                //spdlog::info("LatLonRenderer: c = {}", fmt::ptr(c));

                _sphericalBodyRenderer = dynamic_cast<SphericalBodyRenderer*>(c);
            }

            if (_sphericalBodyRenderer) {
                _sphericalBodyRenderer->doShaderConfig(glslProgram);
            }

            glslProgram.setBool("useTexture", false);

            glslProgram.setMat4("model", glm::value_ptr(s.getTransform()));

            //---------------------------------------------
            // Regular lat/lon
            glBindVertexArray(_latAndLongVao);
            // Draw vertices
            glDrawArrays(GL_LINES, 0, (GLsizei)numLatAndLongVertices);

            //---------------------------------------------
            // Special lat/lon
            glLineWidth(2);

            glBindVertexArray(_specialLatAndLongVao);
            // Draw vertices
            glDrawArrays(GL_LINES, 0, (GLsizei)numSpecialLatAndLongVertices);

            glLineWidth(1);
        }
    }
}