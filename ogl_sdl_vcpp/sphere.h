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


class Point3D
{
public:
    float x, y, z;
    Point3D() : x(0), y(0), z(0)
    {
    }
    Point3D(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    ~Point3D()
    {
    }
};

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
    Sphere() : _center(0,0,0), _r(0), _g(0), _b(0),
        _radius(0),        _rotationAngle(0), _rotationAngularVelocity(0), _axisRotationAngle(0),    _axisTiltAngle(0),
        _orbitalRadius(0), _orbitalAngle(0),  _orbitalAngularVelocity(0),  _orbitalRotationAngle(0), _orbitalTiltAngle(0),
        _parent(nullptr)
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

    void setOrbitalParameters(float orbitalRadius, float orbitalAngle, float orbitalAngularVelocity, float orbitalRotationAngle, float orbitalTiltAngle)
    {
        setOrbitalRadius(orbitalRadius);
        _orbitalAngle = orbitalAngle;
        _orbitalAngularVelocity = orbitalAngularVelocity;
        _orbitalRotationAngle = orbitalRotationAngle;
        _orbitalTiltAngle = orbitalTiltAngle;
    }

    void setParentSphere(Sphere *parent)
    {
        _parent = parent;
    }

    inline Point3D& getCenter()
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
        return _orbitalTiltAngle;
    }


    void generateVertices()
    {
        float numLongitudes = 720;
        float inc = float(2*M_PI) / numLongitudes;

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

                _vertices.push_back(x1);   _vertices.push_back(y1);   _vertices.push_back(z1);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);
                _vertices.push_back(x2);   _vertices.push_back(y2);   _vertices.push_back(z2);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);
                _vertices.push_back(x3);   _vertices.push_back(y3);   _vertices.push_back(z3);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);
                _vertices.push_back(x3);   _vertices.push_back(y3);   _vertices.push_back(z3);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);
                _vertices.push_back(x2);   _vertices.push_back(y2);   _vertices.push_back(z2);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);
                _vertices.push_back(x4);   _vertices.push_back(y4);   _vertices.push_back(z4);   _vertices.push_back(_r);  _vertices.push_back(_g); _vertices.push_back(_b);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }

        //---------------------------------------------------------------------------------
        // Longitudes
        //---------------------------------------------------------------------------------
        std::list<float> alphas = { 0, 30, 60, 90, 120, 150 };

        for (float alpha : alphas)
        {
            alpha = glm::radians(alpha);

            for (float theta = 0; theta < float(2 * M_PI) - inc; theta += inc)
            {
                float x1 = 1.01 * _radius * sin(theta) * cos(alpha);
                float y1 = 1.01 * _radius * sin(theta) * sin(alpha);
                float z1 = 1.01 * _radius * cos(theta);

                float x2 = 1.01 * _radius * sin(theta + inc) * cos(alpha);
                float y2 = 1.01 * _radius * sin(theta + inc) * sin(alpha);
                float z2 = 1.01 * _radius * cos(theta + inc);

                _latLongVertices.push_back(x1);   _latLongVertices.push_back(y1);   _latLongVertices.push_back(z1);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);
                _latLongVertices.push_back(x2);   _latLongVertices.push_back(y2);   _latLongVertices.push_back(z2);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);

                //printf("point generated for alpha = %f, theta = %f\n", alpha, theta);
            }
        }

        //---------------------------------------------------------------------------------
        // Latitudes
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

                _latLongVertices.push_back(x1);   _latLongVertices.push_back(y1);   _latLongVertices.push_back(z1);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);
                _latLongVertices.push_back(x2);   _latLongVertices.push_back(y2);   _latLongVertices.push_back(z2);   _latLongVertices.push_back(_r*0.5);  _latLongVertices.push_back(_g*0.5); _latLongVertices.push_back(_b*0.5);

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

    glm::vec4 getModelTransformedCenter()
    {
        glm::vec4 center(_center.x, _center.y, _center.z, 1.0);
        return getModelTransformMatrix() * center;
    }

    glm::mat4 getModelTransformMatrix()
    {
        glm::mat4 modelTrans(1.0f);

        // translate
        modelTrans = glm::translate(modelTrans,   glm::vec3(getCenter().x, getCenter().y, getCenter().z));

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

    Point3D _center;
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
    float _orbitalRotationAngle;        // previously called 'alphao'
    float _orbitalTiltAngle;            // previously called 'betao'


    std::vector<float> _vertices;
    std::vector<float> _latLongVertices;
    Sphere *_parent;
};


#endif
