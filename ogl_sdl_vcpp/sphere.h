#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
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
        _orbitalRadius(0), _orbitalAngle(0),  _orbitalAngularVelocity(0),  _orbitalRotationAngle(0), _orbitalTiltAngle(0)
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

    void setOrbitalParameters(float orbitalRadius, float orbitalAngularVelocity, float orbitalRotationAngle, float orbitalTiltAngle)
    {
        setOrbitalRadius(orbitalRadius);
        _orbitalAngularVelocity = orbitalAngularVelocity;
        _orbitalRotationAngle = orbitalRotationAngle;
        _orbitalTiltAngle = orbitalTiltAngle;
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
        float numLongitudes = 72;
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

        printf("sphere vertex generation done\n");
    }

    std::vector<float>& getVertices()
    {
        return _vertices;
    }

    void advance(float stepMultiplier)
    {
        _rotationAngle += _rotationAngularVelocity * stepMultiplier;

        _orbitalAngle += _orbitalAngularVelocity * stepMultiplier;
        _center.x = _orbitalRadius * cos (_orbitalAngle);
        _center.y = _orbitalRadius * sin (_orbitalAngle);
        _center.z = 0;

        // todo - use orbital rotation & axis rotation parameters to increment angles
    }

    void freeVertices()
    {
        _vertices.clear();
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
};


#endif
