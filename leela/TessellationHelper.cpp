#include <vector>
#include "Utils.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "TessellationHelper.h"
#include "spdlog/spdlog.h"

/*
*
 * Return a tuple containing information about a point on sphere corresponding to the given radius, alpha, theta arguments.
 *  - x, y, & z of the point on sphere
 *  - Normal unit vector at that point on the sphere
 *  - texture coordinates to use for this point from the 2-D rectangular texture.
 */
std::tuple<float, float, float, glm::vec3, float, float> CalcPointOnSphere(float radius, float alpha, float theta)
{
    if (alpha < 0)          alpha = 0.0f;
    if (theta < 0)          theta = 0.0f;

    if (alpha > 2 * M_PI)   alpha = float(2 * M_PI);
    if (theta > M_PI)       theta = float(M_PI);

    float x = radius * sin(theta) * cos(alpha);
    float y = radius * sin(theta) * sin(alpha);
    float z = radius * cos(theta);

    glm::vec3 N = glm::normalize(glm::vec3(x, y, z) - glm::vec3(0.0f, 0.0f, 0.0f));

    float texX = float(alpha / (2 * M_PI));
    float texY = float(theta / M_PI);

    if (texX > 1.0f)    texX = 1.0f;
    if (texY > 1.0f)    texY = 1.0f;

    return { x, y, z, N, texX, texY };
}

//
//  numEquatorVertices      Number of vertices on the circumference. determines the increment.
//
std::vector<float>* ConstructSphereVertices(float radius, glm::vec3 color, int numEquatorVertices, bool bTextureCoord)
{
    std::vector<float>* v = new std::vector<float>();

    float alpha_inc = float(2 * float(M_PI)) / numEquatorVertices;
    float theta_inc = float(M_PI) / (numEquatorVertices / 2);

    int numFloats = int((2 * float(M_PI) / alpha_inc) * (float(M_PI) / theta_inc)) * 10;

    v->reserve(numFloats);
    float alpha;
    float theta;
    for (alpha = 0; alpha < float(2 * float(M_PI)); alpha += alpha_inc)
    {
        for (theta = 0; theta < float(M_PI); theta += theta_inc)
        {
            float theta_2 = theta + theta_inc;
            float alpha_2 = alpha + alpha_inc;

            auto [x1, y1, z1, N1, texX1, texY1] = CalcPointOnSphere(radius, alpha, theta);
            auto [x2, y2, z2, N2, texX2, texY2] = CalcPointOnSphere(radius, alpha, theta_2);
            auto [x3, y3, z3, N3, texX3, texY3] = CalcPointOnSphere(radius, alpha_2, theta);
            auto [x4, y4, z4, N4, texX4, texY4] = CalcPointOnSphere(radius, alpha_2, theta_2);

            if (bTextureCoord) {
                vector_push_back_12(*v, x1, y1, z1, color.r, color.g, color.b, 1.0f, N1.x, N1.y, N1.z, texX1, texY1);
                vector_push_back_12(*v, x2, y2, z2, color.r, color.g, color.b, 1.0f, N2.x, N2.y, N2.z, texX2, texY2);
                vector_push_back_12(*v, x3, y3, z3, color.r, color.g, color.b, 1.0f, N3.x, N3.y, N3.z, texX3, texY3);
                vector_push_back_12(*v, x3, y3, z3, color.r, color.g, color.b, 1.0f, N3.x, N3.y, N3.z, texX3, texY3);
                vector_push_back_12(*v, x2, y2, z2, color.r, color.g, color.b, 1.0f, N2.x, N2.y, N2.z, texX2, texY2);
                vector_push_back_12(*v, x4, y4, z4, color.r, color.g, color.b, 1.0f, N4.x, N4.y, N4.z, texX4, texY4);
            }
            else {
                vector_push_back_10(*v, x1, y1, z1, color.r, color.g, color.b, 1.0f, N1.x, N1.y, N1.z);
                vector_push_back_10(*v, x2, y2, z2, color.r, color.g, color.b, 1.0f, N2.x, N2.y, N2.z);
                vector_push_back_10(*v, x3, y3, z3, color.r, color.g, color.b, 1.0f, N3.x, N3.y, N3.z);
                vector_push_back_10(*v, x3, y3, z3, color.r, color.g, color.b, 1.0f, N3.x, N3.y, N3.z);
                vector_push_back_10(*v, x2, y2, z2, color.r, color.g, color.b, 1.0f, N2.x, N2.y, N2.z);
                vector_push_back_10(*v, x4, y4, z4, color.r, color.g, color.b, 1.0f, N4.x, N4.y, N4.z);
            }
        }
    }

    return v;
}