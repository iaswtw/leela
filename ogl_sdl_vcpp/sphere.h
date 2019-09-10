#ifndef SPHERE_H
#define SPHERE_H

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <list>
#define _USE_MATH_DEFINES
#include <math.h>


#define VERTEX_STRIDE_IN_VBO        7

typedef enum
{
    RotationPaused,
    RevolutionPaused,
    PrecessionPaused
} SphereFlagEnum;

typedef enum
{
    False,
    True,
    Toggle
} SphereFlagOperationEnum;

class Color
{
public:
    float r, g, b, a;
    Color() : r(1), g(1), b(1), a(1)
    {
    }

    ~Color()
    {
    }
};


class Sphere
{
public:
    Sphere()
    {
    }

    ~Sphere()
    {
    }

    void setColor(float r, float g, float b)
    {
        _r = r;
        _g = g;
        _b = b;
    }
    void setOrbitalPlaneColor(glm::vec3 orbitalPlaneColor)
    {
        _orbitalPlaneColor = orbitalPlaneColor;
    }

    void setOrbitalRadius(float orbitalRadius)
    {
        _orbitalRadius = orbitalRadius;
    }

    void setRotationParameters(float radius, float rotationAngle, float rotationAngularVelocity, float axisRotationAngle, float axisTiltAngle)
    {
        _radius = radius;
        _rotationAngle = rotationAngle;
        _rotationAngularVelocity = rotationAngularVelocity;
        _axisRotationAngle = axisRotationAngle;
        _axisTiltAngle = axisTiltAngle;
    }

    void setOrbitalParameters(float orbitalRadius, float orbitalAngle, float orbitalAngularVelocity, float orbitalPlaneRotationAngle, float orbitalPlaneTiltAngle)
    {
        setOrbitalRadius(orbitalRadius);
        _orbitalAngle = orbitalAngle;
        _orbitalAngularVelocity = orbitalAngularVelocity;
        _orbitalPlaneRotationAngle = orbitalPlaneRotationAngle;
        _orbitalPlaneTiltAngle = orbitalPlaneTiltAngle;
    }

    void setParentSphere(Sphere *parent)
    {
        _parent = parent;
    }

    void setOrbitalAngle(float orbitalAngle)
    {
        _orbitalAngle = orbitalAngle;
    }
    void setAxisRotationAngle(float axisRotationAngle)
    {
        _axisRotationAngle = axisRotationAngle;
    }

    inline glm::vec3& getCenter()
    {
        return _center;
    }

    inline float getRadius()
    {
        return _radius;
    }

    inline float getRotationAngle()
    {
        return _rotationAngle;
    }

    inline float getAxisRotationAngle()
    {
        return _axisRotationAngle;
    }

    inline float getAxisTiltAngle()
    {
        return _axisTiltAngle;
    }


    inline float getOrbitalRotationAngle()
    {
        return _axisRotationAngle;
    }

    inline float getOrbitalTiltAngle()
    {
        return _orbitalPlaneTiltAngle;
    }

    inline float getOrbitalPlaneTiltAngle()
    {
        return _orbitalPlaneTiltAngle;
    }

    inline float getOrbitalPlaneRotationAngle()
    {
        return _orbitalPlaneRotationAngle;
    }

    void generateVertices()
    {
        float numLongitudes = 1000;
        float alpha_inc = float(2*M_PI) / numLongitudes;
        float theat_inc = float(M_PI) / (numLongitudes/2);

        int numFloats = int((2 * M_PI / alpha_inc) * (M_PI / theat_inc)) * 7;
        printf("numFloats = %d\n", numFloats);
        _vertices.reserve(numFloats);

        for (float alpha = 0; alpha < float(2*M_PI); alpha += alpha_inc)
        {
            for (float theta = 0; theta < float(M_PI); theta += theat_inc)
            {
                float x1 = _radius * sin(theta) * cos(alpha);
                float y1 = _radius * sin(theta) * sin(alpha);
                float z1 = _radius * cos(theta);

                float x2 = _radius * sin(theta+ theat_inc) * cos(alpha);
                float y2 = _radius * sin(theta+ theat_inc) * sin(alpha);
                float z2 = _radius * cos(theta+ theat_inc);

                float x3 = _radius * sin(theta) * cos(alpha+ alpha_inc);
                float y3 = _radius * sin(theta) * sin(alpha+ alpha_inc);
                float z3 = _radius * cos(theta);

                float x4 = _radius * sin(theta+ theat_inc) * cos(alpha+ alpha_inc);
                float y4 = _radius * sin(theta+ theat_inc) * sin(alpha+ alpha_inc);
                float z4 = _radius * cos(theta+ theat_inc);

                _vertices.push_back(x1);   _vertices.push_back(y1);   _vertices.push_back(z1);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x2);   _vertices.push_back(y2);   _vertices.push_back(z2);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x3);   _vertices.push_back(y3);   _vertices.push_back(z3);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x3);   _vertices.push_back(y3);   _vertices.push_back(z3);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x2);   _vertices.push_back(y2);   _vertices.push_back(z2);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x4);   _vertices.push_back(y4);   _vertices.push_back(z4);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }

        // Add orbital plane to the main vertices for now.  This is centered at origin.
        _orbitalPlaneVertices.push_back(-_orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(-_orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(_orbitalPlaneColor.r*0.3);  _orbitalPlaneVertices.push_back(_orbitalPlaneColor.g*0.3); _orbitalPlaneVertices.push_back(_orbitalPlaneColor.b*0.3);    _orbitalPlaneVertices.push_back(1.0);
        _orbitalPlaneVertices.push_back( _orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(-_orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(_orbitalPlaneColor.r*0.3);  _orbitalPlaneVertices.push_back(_orbitalPlaneColor.g*0.3); _orbitalPlaneVertices.push_back(_orbitalPlaneColor.b*0.3);    _orbitalPlaneVertices.push_back(1.0);
        _orbitalPlaneVertices.push_back( _orbitalRadius*1.2);   _orbitalPlaneVertices.push_back( _orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(_orbitalPlaneColor.r*0.3);  _orbitalPlaneVertices.push_back(_orbitalPlaneColor.g*0.3); _orbitalPlaneVertices.push_back(_orbitalPlaneColor.b*0.3);    _orbitalPlaneVertices.push_back(1.0);
        _orbitalPlaneVertices.push_back( _orbitalRadius*1.2);   _orbitalPlaneVertices.push_back( _orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(_orbitalPlaneColor.r*0.3);  _orbitalPlaneVertices.push_back(_orbitalPlaneColor.g*0.3); _orbitalPlaneVertices.push_back(_orbitalPlaneColor.b*0.3);    _orbitalPlaneVertices.push_back(1.0);
        _orbitalPlaneVertices.push_back(-_orbitalRadius*1.2);   _orbitalPlaneVertices.push_back( _orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(_orbitalPlaneColor.r*0.3);  _orbitalPlaneVertices.push_back(_orbitalPlaneColor.g*0.3); _orbitalPlaneVertices.push_back(_orbitalPlaneColor.b*0.3);    _orbitalPlaneVertices.push_back(1.0);
        _orbitalPlaneVertices.push_back(-_orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(-_orbitalRadius*1.2);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(_orbitalPlaneColor.r*0.3);  _orbitalPlaneVertices.push_back(_orbitalPlaneColor.g*0.3); _orbitalPlaneVertices.push_back(_orbitalPlaneColor.b*0.3);    _orbitalPlaneVertices.push_back(1.0);


        // generate parallel lines along Y axis in the orbital plane
        float inc = float(_orbitalRadius) / int(_orbitalRadius / 50.0);
        for (float x = -_orbitalRadius * 1.2; x <= _orbitalRadius*1.2; x += inc)
        {
            _orbitalPlaneGridVertices.push_back(x);   _orbitalPlaneGridVertices.push_back(-_orbitalRadius * 1.2);   _orbitalPlaneGridVertices.push_back(1);   _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.r*1.2);  _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.g*1.2); _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.b*1.2);    _orbitalPlaneGridVertices.push_back(1.0);
            _orbitalPlaneGridVertices.push_back(x);   _orbitalPlaneGridVertices.push_back( _orbitalRadius * 1.2);   _orbitalPlaneGridVertices.push_back(1);   _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.r*1.2);  _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.g*1.2); _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.b*1.2);    _orbitalPlaneGridVertices.push_back(1.0);
        }
        for (float y = -_orbitalRadius * 1.2; y <= _orbitalRadius*1.2; y += inc)
        {
            _orbitalPlaneGridVertices.push_back(-_orbitalRadius * 1.2);   _orbitalPlaneGridVertices.push_back(y);   _orbitalPlaneGridVertices.push_back(1);   _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.r*1.2);  _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.g*1.2); _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.b*1.2);    _orbitalPlaneGridVertices.push_back(1.0);
            _orbitalPlaneGridVertices.push_back( _orbitalRadius * 1.2);   _orbitalPlaneGridVertices.push_back(y);   _orbitalPlaneGridVertices.push_back(1);   _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.r*1.2);  _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.g*1.2); _orbitalPlaneGridVertices.push_back(_orbitalPlaneColor.b*1.2);    _orbitalPlaneGridVertices.push_back(1.0);
        }

        // Orbit itself
        for (float alpha = 0; alpha < 2 * M_PI; alpha += alpha_inc)
        {
            float x1 = _orbitalRadius * cos(alpha);
            float y1 = _orbitalRadius * sin(alpha);
            float z1 = 0;

            float x2 = _orbitalRadius * cos(alpha+alpha_inc);
            float y2 = _orbitalRadius * sin(alpha + alpha_inc);
            float z2 = 0;

            _orbitVertices.push_back(x1);  _orbitVertices.push_back(y1);  _orbitVertices.push_back(z1);   _orbitVertices.push_back(_orbitalPlaneColor.r); _orbitVertices.push_back(_orbitalPlaneColor.g); _orbitVertices.push_back(_orbitalPlaneColor.b);  _orbitVertices.push_back(0.8);
            _orbitVertices.push_back(x2);  _orbitVertices.push_back(y2);  _orbitVertices.push_back(z2);   _orbitVertices.push_back(_orbitalPlaneColor.r); _orbitVertices.push_back(_orbitalPlaneColor.g); _orbitVertices.push_back(_orbitalPlaneColor.b);  _orbitVertices.push_back(0.8);
        }


        inc = float(2 * M_PI) / numLongitudes;

        //---------------------------------------------------------------------------------
        // Longitudes
        //---------------------------------------------------------------------------------
        //std::list<float> alphas = { 0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165 };

        //for (float alpha : alphas)
        for (float alphas = 0.0f; alphas < 180.0f; alphas += 30.0f)
        {
            float alpha = glm::radians(alphas);

            for (float theta = 0; theta < float(2 * M_PI); theta += inc)
            {
                float x1 = 1.01 * _radius * sin(theta) * cos(alpha);
                float y1 = 1.01 * _radius * sin(theta) * sin(alpha);
                float z1 = 1.01 * _radius * cos(theta);

                float x2 = 1.01 * _radius * sin(theta + inc) * cos(alpha);
                float y2 = 1.01 * _radius * sin(theta + inc) * sin(alpha);
                float z2 = 1.01 * _radius * cos(theta + inc);

                _latLongVertices.push_back(x1);   _latLongVertices.push_back(y1);   _latLongVertices.push_back(z1);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);  _latLongVertices.push_back(1.0);
                _latLongVertices.push_back(x2);   _latLongVertices.push_back(y2);   _latLongVertices.push_back(z2);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);  _latLongVertices.push_back(1.0);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }

        //---------------------------------------------------------------------------------
        // Special Latitudes
        //---------------------------------------------------------------------------------
        std::list<float> thetas = { 0, 66.5, 23.5, 90, 90+23.5, 90+66.5 };          // 0 = +z axis, 90 is equator, 90+66.5 is antarctic circle

        for (float theta : thetas)
        {
            theta = glm::radians(theta);

            for (float alpha = 0; alpha < float(2 * M_PI); alpha += inc)
            {
                float x1 = 1.005 * _radius * sin(theta) * cos(alpha);
                float y1 = 1.005 * _radius * sin(theta) * sin(alpha);
                float z1 = 1.005 * _radius * cos(theta);

                float x2 = 1.005 * _radius * sin(theta) * cos(alpha + inc);
                float y2 = 1.005 * _radius * sin(theta) * sin(alpha + inc);
                float z2 = 1.005 * _radius * cos(theta);

                _latLongVertices.push_back(x1);   _latLongVertices.push_back(y1);   _latLongVertices.push_back(z1);   _latLongVertices.push_back(_r*0.9);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);  _latLongVertices.push_back(1.0);
                _latLongVertices.push_back(x2);   _latLongVertices.push_back(y2);   _latLongVertices.push_back(z2);   _latLongVertices.push_back(_r*0.9);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);  _latLongVertices.push_back(1.0);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }

        //---------------------------------------------------------------------------------
        // Latitudes
        //---------------------------------------------------------------------------------
        for (float thetas = 0; thetas < 180; thetas += 30)
        {
            float theta = glm::radians(thetas);

            for (float alpha = 0; alpha < float(2 * M_PI) - inc; alpha += inc)
            {
                float x1 = 1.005 * _radius * sin(theta) * cos(alpha);
                float y1 = 1.005 * _radius * sin(theta) * sin(alpha);
                float z1 = 1.005 * _radius * cos(theta);

                float x2 = 1.005 * _radius * sin(theta) * cos(alpha + inc);
                float y2 = 1.005 * _radius * sin(theta) * sin(alpha + inc);
                float z2 = 1.005 * _radius * cos(theta);

                _latLongVertices.push_back(x1);   _latLongVertices.push_back(y1);   _latLongVertices.push_back(z1);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);  _latLongVertices.push_back(1.0);
                _latLongVertices.push_back(x2);   _latLongVertices.push_back(y2);   _latLongVertices.push_back(z2);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);  _latLongVertices.push_back(1.0);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }
        
        printf("sphere vertex generation done\n");
    }

    std::vector<float>& getVertices()
    {
        return _vertices;
    }

    std::vector<float>& getLatitudeAndLongitudeVertices()
    {
        return _latLongVertices;
    }

    std::vector<float>& getOrbitalPlaneVertices()
    {
        return _orbitalPlaneVertices;
    }

    std::vector<float>& getOrbitalPlaneGridVertices()
    {
        return _orbitalPlaneGridVertices;
    }

    std::vector<float>& getOrbitVertices()
    {
        return _orbitVertices;
    }

    glm::mat4 getOrbitalPlaneModelMatrix()
    {
        glm::mat4 modelTrans(1.0f);

        // translate to the parent's center.
        if (_parent != nullptr)
        {
            modelTrans = glm::translate(modelTrans, _parent->getCenter());
            modelTrans = glm::rotate(modelTrans, getOrbitalPlaneRotationAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
            modelTrans = glm::rotate(modelTrans, getOrbitalPlaneTiltAngle(), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        return modelTrans;
    }

    glm::vec3 getModelTransformedCenter()
    {
        //glm::vec4 center(_center.x, _center.y, _center.z, 1.0);
        //return glm::vec3(getModelMatrix() * center);

        return _center;
    }

    glm::mat4 getModelMatrix()
    {
        glm::mat4 modelTrans(1.0f);

        // translate
        modelTrans = glm::translate(modelTrans,   getCenter());

        // tilt and rotate axis. 
        // todo - do this using a single rotate invocation
        modelTrans = glm::rotate(modelTrans,   getAxisRotationAngle(),   glm::vec3(0.0f, 0.0f, 1.0f));
        modelTrans = glm::rotate(modelTrans,   getAxisTiltAngle(),       glm::vec3(0.0f, 1.0f, 0.0f));
        modelTrans = glm::rotate(modelTrans,   getRotationAngle(),       glm::vec3(0.0f, 0.0f, 1.0f));
        
        return modelTrans;
    }

    void advance(float stepMultiplier)
    {
        if (bRotationMotion)
            _rotationAngle += _rotationAngularVelocity * stepMultiplier;

        if (bRevolutionMotion)
            _orbitalAngle += _orbitalAngularVelocity * stepMultiplier;

        calculateCenterPosition();
    }

    // Calculates center position based on the value of current parameters
    void calculateCenterPosition()
    {
        _center.x = _orbitalRadius * cos(_orbitalAngle);
        _center.y = _orbitalRadius * sin(_orbitalAngle);
        _center.z = 0;       // todo - use orbital tilt

        // apply orbital tilt
        _center.z = -_center.x * sin(_orbitalPlaneTiltAngle);
        _center.x = _center.x * cos(_orbitalPlaneTiltAngle);

        // apply orbital rotation
        float x, y;
        x = _center.x;
        y = _center.y;
        _center.x = x * cos(_orbitalPlaneRotationAngle) - y * sin(_orbitalPlaneRotationAngle);
        _center.y = x * sin(_orbitalPlaneRotationAngle) + y * cos(_orbitalPlaneRotationAngle);


        // parent is assumed to have been updated
        if (_parent != nullptr)
        {
            _center.x += _parent->getCenter().x;
            _center.y += _parent->getCenter().y;
            _center.z += _parent->getCenter().z;
        }
    }

    void freeVertices()
    {
        _vertices.clear();
        _latLongVertices.clear();
    }

public:
    bool bRevolutionMotion = true;
    bool bRotationMotion = true;
    bool bPrecessionMotion = false;


private:
    //void changeBoolean(bool *pBool, SphereFlagOperationEnum flagOperation)
    //{
    //    switch (flagOperation)
    //    {
    //    case Toggle:
    //        *pBool = !*pBool;
    //        break;
    //    case True:
    //        *pBool = true;
    //        break;
    //    case False:
    //        *pBool = false;
    //        break;
    //    }
    //}

private:
    // angles and angle velocities are in radians

    glm::vec3 _center = glm::vec3(0.0, 0.0, 0.0);
    float _r = 1;
    float _g = 1;
    float _b = 1;
    glm::vec3 _orbitalPlaneColor = glm::vec3(1.0, 1.0, 1.0);

    // Rotation variables
    float _radius = 0;
    float _rotationAngle = 0;               // current rotation angle (increments based on rotation angular velocity). Previously called 'th'.
    float _rotationAngularVelocity = 0;     // angular velocity or rotation around sphere's axis. Previously called 'w'.
    float _axisRotationAngle = 0;           // previously called 'alpha'
    float _axisTiltAngle = 0;               // previously called 'beta'

    // Revolution variables
    float _orbitalRadius = 0;
    float _orbitalAngle = 0;                // current revolution (orbital) angle (increments based on revolution angular velocity). Previously called 'tho'
    float _orbitalAngularVelocity = 0;      // angular velocity of revolution around parent sphere. Previously called 'wo'.
    float _orbitalPlaneRotationAngle = 0;   // previously called 'alphao'
    float _orbitalPlaneTiltAngle = 0;       // previously called 'betao'



    std::vector<float> _vertices;
    std::vector<float> _latLongVertices;
    std::vector<float> _orbitalPlaneVertices;
    std::vector<float> _orbitalPlaneGridVertices;
    std::vector<float> _orbitVertices;
    Sphere *_parent;
};



class Axis
{
public:
    Axis()
    {
    }

    ~Axis()
    {
    }

    void generateVertices(float xAxisLength, float yAxisLength, float zAxisLength, glm::vec3 xAxisColor, glm::vec3 yAxisColor, glm::vec3 zAxisColor)
    {
        _vertices.push_back(-xAxisLength);  _vertices.push_back(0.0);  _vertices.push_back(0.0);   _vertices.push_back(xAxisColor.r);  _vertices.push_back(xAxisColor.g);  _vertices.push_back(xAxisColor.b); _vertices.push_back(0.8);
        _vertices.push_back(xAxisLength);  _vertices.push_back(0.0);  _vertices.push_back(0.0);   _vertices.push_back(xAxisColor.r);  _vertices.push_back(xAxisColor.g);  _vertices.push_back(xAxisColor.b);  _vertices.push_back(0.8);

        _vertices.push_back(0.0);  _vertices.push_back(-yAxisLength);  _vertices.push_back(0.0);   _vertices.push_back(yAxisColor.r);  _vertices.push_back(yAxisColor.g);  _vertices.push_back(yAxisColor.b); _vertices.push_back(0.8);
        _vertices.push_back(0.0);  _vertices.push_back(yAxisLength);  _vertices.push_back(0.0);   _vertices.push_back(yAxisColor.r);  _vertices.push_back(yAxisColor.g);  _vertices.push_back(yAxisColor.b);  _vertices.push_back(0.8);

        _vertices.push_back(0.0);  _vertices.push_back(0.0);  _vertices.push_back(-zAxisLength);   _vertices.push_back(zAxisColor.r);  _vertices.push_back(zAxisColor.g);  _vertices.push_back(zAxisColor.b); _vertices.push_back(0.8);
        _vertices.push_back(0.0);  _vertices.push_back(0.0);  _vertices.push_back(zAxisLength);   _vertices.push_back(zAxisColor.r);  _vertices.push_back(zAxisColor.g);  _vertices.push_back(zAxisColor.b);  _vertices.push_back(0.8);
    }

    std::vector<float>& getVertices()
    {
        return _vertices;
    }

    void freeVertices()
    {
        _vertices.clear();
    }


private:
    std::vector<float> _vertices;

};


#endif
