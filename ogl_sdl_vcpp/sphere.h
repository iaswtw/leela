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
    Sphere() : _center(0, 0, 0), _r(0), _g(0), _b(0),
        _radius(0), _rotationAngle(0), _rotationAngularVelocity(0), _axisRotationAngle(0), _axisTiltAngle(0),
        _orbitalRadius(0), _orbitalAngle(0), _orbitalAngularVelocity(0), _orbitalPlaneRotationAngle(0), _orbitalPlaneTiltAngle(0),
        _parent(nullptr), _bOrbitalPlane(false)
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

    void enableOrbitalPlaneGeneration()
    {
        _bOrbitalPlane = true;
    }

    void generateVertices()
    {
        float numLongitudes = 1200;
        float inc = float(2*M_PI) / numLongitudes;
        _vertices.reserve( int((2*M_PI/inc)*(M_PI/inc)) * 7 );

        for (float alpha = 0; alpha < float(2*M_PI)-inc; alpha += inc)
        {
            for (float theta = 0; theta < float(M_PI)-inc; theta += inc)
            {
                float x1 = _radius * sin(theta) * cos(alpha);
                float y1 = _radius * sin(theta) * sin(alpha);
                float z1 = _radius * cos(theta);

                float x2 = _radius * sin(theta+inc) * cos(alpha);
                float y2 = _radius * sin(theta+inc) * sin(alpha);
                float z2 = _radius * cos(theta+inc);

                float x3 = _radius * sin(theta) * cos(alpha+inc);
                float y3 = _radius * sin(theta) * sin(alpha+inc);
                float z3 = _radius * cos(theta);

                float x4 = _radius * sin(theta+inc) * cos(alpha+inc);
                float y4 = _radius * sin(theta+inc) * sin(alpha+inc);
                float z4 = _radius * cos(theta+inc);

                _vertices.push_back(x1);   _vertices.push_back(y1);   _vertices.push_back(z1);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x2);   _vertices.push_back(y2);   _vertices.push_back(z2);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x3);   _vertices.push_back(y3);   _vertices.push_back(z3);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x3);   _vertices.push_back(y3);   _vertices.push_back(z3);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x2);   _vertices.push_back(y2);   _vertices.push_back(z2);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);
                _vertices.push_back(x4);   _vertices.push_back(y4);   _vertices.push_back(z4);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);  _vertices.push_back(1.0);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }

        if (_bOrbitalPlane)
        {
            // add orbital plane to the main vertices for now
            _orbitalPlaneVertices.push_back(-1400);   _orbitalPlaneVertices.push_back(-1400);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(0.3);  _orbitalPlaneVertices.push_back(0.15); _orbitalPlaneVertices.push_back(0.15);    _orbitalPlaneVertices.push_back(0.4);
            _orbitalPlaneVertices.push_back(1400);    _orbitalPlaneVertices.push_back(-1400);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(0.3);  _orbitalPlaneVertices.push_back(0.15); _orbitalPlaneVertices.push_back(0.15);    _orbitalPlaneVertices.push_back(0.4);
            _orbitalPlaneVertices.push_back(1400);    _orbitalPlaneVertices.push_back(1400);    _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(0.3);  _orbitalPlaneVertices.push_back(0.15); _orbitalPlaneVertices.push_back(0.15);    _orbitalPlaneVertices.push_back(0.4);
            _orbitalPlaneVertices.push_back(1400);    _orbitalPlaneVertices.push_back(1400);    _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(0.3);  _orbitalPlaneVertices.push_back(0.15); _orbitalPlaneVertices.push_back(0.15);    _orbitalPlaneVertices.push_back(0.4);
            _orbitalPlaneVertices.push_back(-1400);   _orbitalPlaneVertices.push_back(1400);    _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(0.3);  _orbitalPlaneVertices.push_back(0.15); _orbitalPlaneVertices.push_back(0.15);    _orbitalPlaneVertices.push_back(0.4);
            _orbitalPlaneVertices.push_back(-1400);   _orbitalPlaneVertices.push_back(-1400);   _orbitalPlaneVertices.push_back(0);   _orbitalPlaneVertices.push_back(0.3);  _orbitalPlaneVertices.push_back(0.15); _orbitalPlaneVertices.push_back(0.15);    _orbitalPlaneVertices.push_back(0.4);
        }


        //---------------------------------------------------------------------------------
        // Longitudes
        //---------------------------------------------------------------------------------
        //std::list<float> alphas = { 0, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165 };

        //for (float alpha : alphas)
        for (float alphas = 0.0f; alphas < 180.0f; alphas += 30.0f)
        {
            float alpha = glm::radians(alphas);

            for (float theta = 0; theta < float(2 * M_PI) - inc; theta += inc)
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

            for (float alpha = 0; alpha < float(2 * M_PI) - inc; alpha += inc)
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
        _rotationAngle += _rotationAngularVelocity * stepMultiplier;

        _orbitalAngle += _orbitalAngularVelocity * stepMultiplier;
        _center.x = _orbitalRadius * cos (_orbitalAngle);
        _center.y = _orbitalRadius * sin (_orbitalAngle);
        _center.z = 0;       // todo - use orbital tilt

        // apply orbital tilt
        _center.z = -_center.x * sin(_orbitalPlaneTiltAngle);
        _center.x =  _center.x * cos(_orbitalPlaneTiltAngle);

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


private:
    // angles and angle velocities are in radians

    glm::vec3 _center;
    float _r;
    float _g;
    float _b;

    // Rotation variables
    float _radius;
    float _rotationAngle;               // current rotation angle (increments based on rotation angular velocity). Previously called 'th'.
    float _rotationAngularVelocity;     // angular velocity or rotation around sphere's axis. Previously called 'w'.
    float _axisRotationAngle;           // previously called 'alpha'
    float _axisTiltAngle;               // previously called 'beta'

    // Revolution variables
    float _orbitalRadius;
    float _orbitalAngle;                // current revolution (orbital) angle (increments based on revolution angular velocity). Previously called 'tho'
    float _orbitalAngularVelocity;      // angular velocity of revolution around parent sphere. Previously called 'wo'.
    float _orbitalPlaneRotationAngle;   // previously called 'alphao'
    float _orbitalPlaneTiltAngle;       // previously called 'betao'


    bool _bOrbitalPlane;
    std::vector<float> _vertices;
    std::vector<float> _latLongVertices;
    std::vector<float> _orbitalPlaneVertices;
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
