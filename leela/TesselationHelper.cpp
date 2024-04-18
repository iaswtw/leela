#include <vector>
#include "Utils.h"

//std::vector<float>* constructSphereVertices()
//{
//    std::vector<float>* v = new std::vector<float>();
//    Sphere& s = _sphere;
//
//    float polygonIncrement = _getPolygonIncrement();
//
//    float alpha_inc = float(2 * float(M_PI)) / polygonIncrement;
//    float theta_inc = float(M_PI) / (polygonIncrement / 2);
//
//    int numFloats = int((2 * float(M_PI) / alpha_inc) * (float(M_PI) / theta_inc)) * 10;
//    spdlog::info("");
//    spdlog::info("{}: Main sphere vertex data occupies {} floats", _sphere._name, numFloats);
//
//    v->reserve(numFloats);
//    float alpha;
//    float theta;
//    for (alpha = 0; alpha < float(2 * float(M_PI)); alpha += alpha_inc)
//    {
//        for (theta = 0; theta < float(float(M_PI)); theta += theta_inc)
//        {
//            float theta_2 = theta + theta_inc;
//            float alpha_2 = alpha + alpha_inc;
//
//            auto [x1, y1, z1, N1, texX1, texY1] = calcPointOnSphere(s._radius, alpha, theta);
//            auto [x2, y2, z2, N2, texX2, texY2] = calcPointOnSphere(s._radius, alpha, theta_2);
//            auto [x3, y3, z3, N3, texX3, texY3] = calcPointOnSphere(s._radius, alpha_2, theta);
//            auto [x4, y4, z4, N4, texX4, texY4] = calcPointOnSphere(s._radius, alpha_2, theta_2);
//
//            vector_push_back_12(*v, x1, y1, z1, s._r, s._g, s._b, 1.0f, N1.x, N1.y, N1.z, texX1, texY1);
//            vector_push_back_12(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, texX2, texY2);
//            vector_push_back_12(*v, x3, y3, z3, s._r, s._g, s._b, 1.0f, N3.x, N3.y, N3.z, texX3, texY3);
//            vector_push_back_12(*v, x3, y3, z3, s._r, s._g, s._b, 1.0f, N3.x, N3.y, N3.z, texX3, texY3);
//            vector_push_back_12(*v, x2, y2, z2, s._r, s._g, s._b, 1.0f, N2.x, N2.y, N2.z, texX2, texY2);
//            vector_push_back_12(*v, x4, y4, z4, s._r, s._g, s._b, 1.0f, N4.x, N4.y, N4.z, texX4, texY4);
//        }
//    }
//
//    return v;
//}