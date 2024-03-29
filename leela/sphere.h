#ifndef SPHERE_H
#define SPHERE_H

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>


#define VERTEX_STRIDE_IN_VBO        7
#define PLANET_STRIDE_IN_VBO        12

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

    void setName(std::string name)
    {
        _name = name;
    }
    void setOrbitalPlaneColor(glm::vec3 orbitalPlaneColor)
    {
        _orbitalPlaneColor = orbitalPlaneColor;
    }

    void setOrbitalRadius(float orbitalRadius)
    {
        _orbitalRadius = orbitalRadius;
    }

    void restoreOrbitalRadius()
    {
        _orbitalRadius = _orbitalRadius_Backup;
    }

    void restoreAxisTiltAngleFromBackup()
    {
        _axisTiltAngle = _axisTiltAngle_Backup;
        _axisTiltAngle_Deg = glm::degrees(_axisTiltAngle);
    }

    void setRotationParameters(float radius, float rotationAngle, float rotationAngularVelocity, float axisTiltOrientationAngle, float axisTiltAngle)
    {
        _radius = radius;
        _rotationAngle = rotationAngle;
        _rotationAngularVelocity = rotationAngularVelocity;
        _axisTiltOrientationAngle = axisTiltOrientationAngle;
        _axisTiltAngle = axisTiltAngle;
        _axisTiltAngle_Backup = axisTiltAngle;
        _axisTiltAngle_Deg = glm::degrees(_axisTiltAngle);
    }

    void setOrbitalParameters(float orbitalRadius, float orbitalAngle, float orbitalAngularVelocity, float orbitalPlaneRotationAngle, float orbitalPlaneTiltAngle)
    {
        setOrbitalRadius(orbitalRadius);
        _orbitalRadius_Backup = _orbitalRadius;

        _orbitalAngle = orbitalAngle;
        _orbitalAngularVelocity = orbitalAngularVelocity;
        _orbitalPlaneRotationAngle = orbitalPlaneRotationAngle;
        _orbitalPlaneTiltAngle = orbitalPlaneTiltAngle;
        _orbitalPlaneTiltAngle_Deg = glm::degrees(_orbitalPlaneTiltAngle);
    }

    void setParentSphere(Sphere *parent)
    {
        _parent = parent;
    }

    void setOrbitalAngle(float orbitalAngle)
    {
        _orbitalAngle = orbitalAngle;
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

    inline float getAxisTiltAngle()
    {
        return _axisTiltAngle;
    }

    inline float getAxisTiltOrientationAngle()
    {
        return _axisTiltOrientationAngle;
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
     
    void setAxisTiltOrientationAngle(float axisTiltOrientationAngle)
    {
        _axisTiltOrientationAngle = axisTiltOrientationAngle;
    }

    glm::mat4 getOrbitalPlaneModelMatrix()
    {
        glm::mat4 modelTrans(1.0f);

        // translate to the parent's center.
        //printf("getOrbitalPlaneModelMatrix: sphere name = %s, _parent = %p\n", _name.c_str(), _parent);
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

    // returns a vec3 in x-y plane (z=0)
    glm::vec3 getAxisTiltOrientationAxis()
    {
        glm::vec3 axisTiltOrientationAxis = glm::vec3(cos(getAxisTiltOrientationAngle()), sin(getAxisTiltOrientationAngle()), 0.0f);
        return axisTiltOrientationAxis;
    }

    glm::mat4 getModelMatrix()
    {
        glm::mat4 modelTrans(1.0f);

        // translate
        modelTrans = glm::translate(modelTrans,   getCenter());

        // tilt and rotate axis. 
        // todo - do this using a single rotate invocation
        //modelTrans = glm::rotate(modelTrans,   getAxisRotationAngle(),   glm::vec3(0.0f, 0.0f, 1.0f));

        glm::vec3 axisTiltOrientationAxis = getAxisTiltOrientationAxis();

        modelTrans = glm::rotate(modelTrans,   getAxisTiltAngle(),       axisTiltOrientationAxis);
        modelTrans = glm::rotate(modelTrans,   getRotationAngle(),       glm::vec3(0.0f, 0.0f, 1.0f));
        
        return modelTrans;
    }

    void advance(float stepMultiplier)
    {
        if (bRotationMotion)
        {
            _rotationAngle += _rotationAngularVelocity * stepMultiplier;
            _rotationAngle = _normalizeAngle(_rotationAngle);
        }

        if (bRevolutionMotion)
        {
            _orbitalAngle += _orbitalAngularVelocity * stepMultiplier;
            _orbitalAngle = _normalizeAngle(_orbitalAngle);
        }

        if (bPrecessionMotion)
        {
            _axisRotationAngle -= 0.005f * stepMultiplier;
            _axisRotationAngle = _normalizeAngle(_axisRotationAngle);

            _axisTiltOrientationAngle -= 0.01f * stepMultiplier;
            _axisTiltOrientationAngle = _normalizeAngle(_axisTiltOrientationAngle);
        }
        if (bOrbitalPlaneRotation)
        {
            _orbitalPlaneRotationAngle -= 0.005f * stepMultiplier;
            _orbitalPlaneRotationAngle = _normalizeAngle(_orbitalPlaneRotationAngle);
        }
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

    // Calculate 12 points outside the orbit suitable for drawing month labels
    // labelPositionScale - if 1.0, labels are at sphere's orbital radius from the parent's (e.g. sun's) center.
    void calculateMonthPositions(float labelPositionScale)
    {
        glm::vec3 axis = getAxisTiltOrientationAxis();
        glm::vec3 perp = glm::vec3(axis.y, -axis.x, axis.z);

        float scaler = labelPositionScale * _orbitalRadius;
        perp = perp * scaler;

        float theta = float(2 * M_PI / 12);
        for (int i = 0; i < 12; i++)
        {
            monthPositions[i].x = perp.x * cos(theta) - perp.y * sin(theta);
            monthPositions[i].y = perp.x * sin(theta) + perp.y * cos(theta);
            monthPositions[i].z = 0;

            //monthPositions[i] += _center;

            theta += float(2 * M_PI / 12);
        }
    }

    inline float _normalizeAngle(float angle)
    {
        
        //if (angle > 2 * M_PI)
        //    angle -= 2 * M_PI;
        //
        //if (angle < -2 * M_PI)
        //    angle += 2 * M_PI;

        angle = (float) fmod(angle, 2 * M_PI);
        return angle;
    }

    // related sphere is a sphere that is close enough to this sphere so as to possibly cast a shadow
    // on this sphere.
    void setRelatedSphere(Sphere * relatedSphere)
    {
        _relatedSphere = relatedSphere;
    }

    void setSunSphere(Sphere* sunSphere)
    {
        _sunSphere = sunSphere;
    }

public:
    bool bRevolutionMotion = true;
    bool bRotationMotion = true;
    bool bPrecessionMotion = false;
    bool bOrbitalPlaneRotation = false;
    bool bIsCenterOfMass = false;               // if true, this is a virtual sphere. It represents the center of mass of two or more bodies.

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
    float _axisTiltAngle_Backup = 0;        // allows restoring after modifying axis tilt
    float _axisTiltAngle_Deg = 0;           // to show in ImGui for controlling via slider.
    float _axisTiltOrientationAngle = glm::radians(0.0f);

    // Revolution variables
    float _orbitalRadius = 0;
    float _orbitalRadius_Backup = 0;        // allows restoring after modifying orbital radius
    float _orbitalAngle = 0;                // current revolution (orbital) angle (increments based on revolution angular velocity). Previously called 'tho'
    float _orbitalAngularVelocity = 0;      // angular velocity of revolution around parent sphere. Previously called 'wo'.
    float _orbitalPlaneRotationAngle = 0;   // previously called 'alphao'
    float _orbitalPlaneTiltAngle = 0;       // previously called 'betao'
    float _orbitalPlaneTiltAngle_Deg = 0;   // this is initialized when _orbitalPlaneTiltAngle is initially set.  After that, Imgui will show and change the _Deg value
                                            // through the use of a slider. If modified, the radian value will be changed by the code that invokes Imgui slider.

    Sphere* _relatedSphere = nullptr;
    Sphere* _sunSphere = nullptr;

    Sphere *_parent = nullptr;              // e.g. if this is moon, _parent is earth.
                                            // TODO: this will change when we bring in center of mass virtual Sphere object.
    std::string _name = "NoName";

    std::vector<glm::vec3> monthPositions = {
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
    };
};


class AxisRenderer;

class Axis
{
public:
    Axis()
    {
    }

    ~Axis()
    {
    }

    void setSpan(float xHalfSpan, float yHalfSpan, float zHalfSpan)
    {
        this->xHalfSpan = xHalfSpan;
        this->yHalfSpan = yHalfSpan;
        this->zHalfSpan = zHalfSpan;
    }

    void setColors(glm::vec3 xColor, glm::vec3 yColor, glm::vec3 zColor)
    {
        this->xColor = xColor;
        this->yColor = yColor;
        this->zColor = zColor;
    }

private:
    glm::vec3 xColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 yColor = glm::vec3(1.0f, 1.0f, 1.0f);;
    glm::vec3 zColor = glm::vec3(1.0f, 1.0f, 1.0f);;

    float xHalfSpan = 1000.0f;
    float yHalfSpan = 1000.0f;
    float zHalfSpan = 1000.0f;

    friend AxisRenderer;

};


#endif
