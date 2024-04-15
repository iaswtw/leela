#pragma once

#include <Component.h>
#include <Sphere.h>
#include <Utils.h>

class OrbitalPlaneComponent : public Component
{
public:
	OrbitalPlaneComponent()
		: Component(ComponentType_OrbitalPlane)
	{
	}

    void init()
    {
        constructOrbitalPlaneVertices();
        constructOrbitalPlaneGridVertices();
    }


    void constructOrbitalPlaneGridVertices()
    {
        std::vector<float>* v = new std::vector<float>();
        Sphere& s = *_sphere;

        //---------------------------------------------------------------------------------
        // Orbital plane grid.
        //---------------------------------------------------------------------------------
        // generate parallel lines along Y axis in the orbital plane
        //float inc = float(_orbitalRadius) / int(_orbitalRadius / 50.0);

        float maxGridLines = 120;
        float inc = (s._orbitalRadius * 2 * 1.2f) / maxGridLines;
        //inc = std::max(inc, 50.0f);
        //printf("inc = %f\n", inc);

        float x, y;
        float m = 0.4f;      // color multiplier
        float radius = s._orbitalRadius;
        glm::vec3& color = s._orbitalPlaneColor;

        x = y = -radius * 1.2f;

        // Create a grid tiny bit above and below the orbital plane
        for (int i = 0; i <= maxGridLines; i++)
        {
            vector_push_back_7(*v, x, -radius * 1.2f, +1, color.r * m, color.g * m, color.b * m, 1.0f);
            vector_push_back_7(*v, x, +radius * 1.2f, +1, color.r * m, color.g * m, color.b * m, 1.0f);

            // If orbital plane is transparent, don't create the bottom grid
            if (!bOrbitalPlaneTransparency) {
                vector_push_back_7(*v, x, -radius * 1.2f, -1, color.r * m, color.g * m, color.b * m, 1.0f);
                vector_push_back_7(*v, x, +radius * 1.2f, -1, color.r * m, color.g * m, color.b * m, 1.0f);
            }

            x += inc;
        }
        for (int i = 0; i <= maxGridLines; i++)
        {
            vector_push_back_7(*v, -radius * 1.2f, y, +1, color.r * m, color.g * m, color.b * m, 1.0f);
            vector_push_back_7(*v, +radius * 1.2f, y, +1, color.r * m, color.g * m, color.b * m, 1.0f);

            // If orbital plane is transparent, don't create the bottom grid
            if (!bOrbitalPlaneTransparency) {
                vector_push_back_7(*v, -radius * 1.2f, y, -1, color.r * m, color.g * m, color.b * m, 1.0f);
                vector_push_back_7(*v, +radius * 1.2f, y, -1, color.r * m, color.g * m, color.b * m, 1.0f);
            }

            y += inc;
        }

        //--------------------------------------

        if (_orbitalPlaneGridVbo != 0) {
            glDeleteBuffers(1, &_orbitalPlaneGridVbo);
        }
        if (_orbitalPlaneGridVao != 0) {
            glDeleteVertexArrays(1, &_orbitalPlaneGridVao);
        }

        glGenVertexArrays(1, &_orbitalPlaneGridVao);
        glBindVertexArray(_orbitalPlaneGridVao);
        glGenBuffers(1, &_orbitalPlaneGridVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _orbitalPlaneGridVbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float) * v->size(),
            v->data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        numOrbitalPlaneGridVertices = v->size() / VERTEX_STRIDE_IN_VBO;
        delete v;

    }


    void constructOrbitalPlaneVertices()
    {
        std::vector<float>* v = new std::vector<float>();
        Sphere& s = *_sphere;
        float m = 0.3f;                          // color multiplier
        float radius = s._orbitalRadius;
        glm::vec3& color = s._orbitalPlaneColor;
        float alpha = 1.0f;

        if (bOrbitalPlaneTransparency)
            alpha = 0.5f;

        //---------------------------------------------------------------------------------
        // Orbital plane.  This is centered at origin.
        //---------------------------------------------------------------------------------
        vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r * m, color.g * m, color.b * m, alpha);
        vector_push_back_7(*v, +radius * 1.2f, -radius * 1.2f, 0, color.r * m, color.g * m, color.b * m, alpha);
        vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r * m, color.g * m, color.b * m, alpha);
        vector_push_back_7(*v, +radius * 1.2f, +radius * 1.2f, 0, color.r * m, color.g * m, color.b * m, alpha);
        vector_push_back_7(*v, -radius * 1.2f, +radius * 1.2f, 0, color.r * m, color.g * m, color.b * m, alpha);
        vector_push_back_7(*v, -radius * 1.2f, -radius * 1.2f, 0, color.r * m, color.g * m, color.b * m, alpha);

        //---------------------------------

        if (_orbitalPlaneVbo != 0) {
            glDeleteBuffers(1, &_orbitalPlaneVbo);
        }
        if (_orbitalPlaneVao != 0) {
            glDeleteVertexArrays(1, &_orbitalPlaneVao);
        }

        glGenVertexArrays(1, &_orbitalPlaneVao);
        glBindVertexArray(_orbitalPlaneVao);
        glGenBuffers(1, &_orbitalPlaneVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _orbitalPlaneVbo);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(float) * v->size(),
            v->data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VERTEX_STRIDE_IN_VBO * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        numOrbitalPlaneVertices = v->size() / VERTEX_STRIDE_IN_VBO;
        delete v;
    }

    void advance(float stepMultiplier)
    {

    }

    void render(GlslProgram& glslProgram)
    {
        Sphere& s = *_sphere;

        if (!s.bIsCenterOfMass)
        {
            // orbital plane has its own model transform different from the sphere itself.
            if (bShowOrbitalPlane)
            {
                glslProgram.setMat4("model", glm::value_ptr(s.getOrbitalPlaneModelMatrix()));

                // Draw orbital plane grid
                glBindVertexArray(_orbitalPlaneGridVao);
                glDrawArrays(GL_LINES, 0, (GLsizei)numOrbitalPlaneGridVertices);

                // Draw vertices
                if (bOrbitalPlaneTransparency) {
                    glDepthMask(GL_FALSE);
                }
                glBindVertexArray(_orbitalPlaneVao);
                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)numOrbitalPlaneVertices);
                if (bOrbitalPlaneTransparency) {
                    glDepthMask(GL_TRUE);
                }
            }
        }
    }


public:

    GLuint _orbitalPlaneVao = 0;
    GLuint _orbitalPlaneVbo = 0;
    GLuint _orbitalPlaneGridVao = 0;            // grid lines in the orbital plane
    GLuint _orbitalPlaneGridVbo = 0;

    size_t numOrbitalPlaneVertices = 0;
    size_t numOrbitalPlaneGridVertices = 0;

    bool bOrbitalPlaneTransparency = false;
    bool bShowOrbitalPlane = false;


    Sphere * _sphere = nullptr;
};