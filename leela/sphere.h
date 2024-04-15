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

#include <spdlog/spdlog.h>

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

    void setOrbitalParameters(float orbitalRadius, float orbitalAngle, float orbitalAngularVelocity, float nodalPrecessionInitialAngle, float orbitalPlaneTiltAngle)
    {
        setOrbitalRadius(orbitalRadius);
        _orbitalRadius_Backup = _orbitalRadius;

        _orbitalAngle = orbitalAngle;
        _orbitalAngularVelocity = orbitalAngularVelocity;
        _nodalPrecessionAngle = nodalPrecessionInitialAngle;
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
        return _nodalPrecessionAngle;
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
        return _center;
    }

    // Get the model matrix of the center of this sphere.
    // - This includes first getting the parent's center's model matrix.
    glm::mat4 getCenterModelMatrix()
    {
        glm::mat4 mat(1.0f);

        //-------------------------------------------------------------------
        // Transformation matrix is created in reverse order of intended application to the transformed point.
        //-------------------------------------------------------------------

        // parent is assumed to have been updated.  Translate by an amount equal to the parent's position.
        if (_parent != nullptr)
            mat = _parent->getCenterModelMatrix();

        // apply nodal precession
        mat = glm::rotate(
            mat,
            _nodalPrecessionAngle,
            glm::vec3(0.0f, 0.0f, 1.0f)         // nodal precession application along z axis
        );

        // apply orbital tilt
        mat = glm::rotate(
            mat,
            _orbitalPlaneTiltAngle,
            glm::vec3(0.0f, 1.0f, 0.0f)         // orbital tilt application is along y axis
        );

        // Create point on the orbit by translating a point at 0,0,0 to the orbit.
        mat = glm::translate(
            mat,
            glm::vec3(
                _orbitalRadius * cos(_orbitalAngle),
                _orbitalRadius * sin(_orbitalAngle),
                0.0f
            )
        );

        return mat;
    }

    // returns a vec3 in x-y plane (z=0)
    glm::vec3 getAxisTiltOrientationAxis()
    {
        glm::vec3 axisTiltOrientationAxis = glm::vec3(cos(getAxisTiltOrientationAngle()), sin(getAxisTiltOrientationAngle()), 0.0f);
        return axisTiltOrientationAxis;
    }

    // Transforms a point on an upright sphere at origin to the tilted sphere in its orbit.
    //    - usis axial tilt, rotation position and precession of the sphere.
    //    - uses orbital radius, tilt and nodal precession. 
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
            float increment = _rotationAngularVelocity;
            if (bSyncWithRevolution)
                increment = _orbitalAngularVelocity * 356.25f;

            _rotationAngle += increment * stepMultiplier;
            _rotationAngle = _normalizeAngle(_rotationAngle);
        }

        if (bRevolutionMotion)
        {
            float increment = _orbitalAngularVelocity;
            if (bOrbitalRevolutionSyncToParent) {
                increment = _parent->_orbitalAngularVelocity * 12.3f;
            }
           
            _orbitalAngle += increment * stepMultiplier;
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
            float increment = 0.005f;
            if (bNodalPrecessionSpeedSyncToParentsRevolution) {         // e.g. Moon's nodal precession with earth's revolution period with a factor of 18.6
                //spdlog::info("applying orbital plane rotation sync'd to parent's revolution");
                //spdlog::info("_parent = {}", fmt::ptr(_parent));
                if (_parent) {
                    increment = _parent->_orbitalAngularVelocity / 18.6;        // hardcode this for moon
                }
            }
            _nodalPrecessionAngle -= increment * stepMultiplier;
            _nodalPrecessionAngle = _normalizeAngle(_nodalPrecessionAngle);
        }
    }

    // Calculates center position based on the value of current parameters
    void calculateCenterPosition()
    {
        glm::mat4 mat = getCenterModelMatrix();
        glm::vec3 raisedCenter;

        _center         = mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);       // transform point at origin to form the transformed sphere's center.
        raisedCenter    = mat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);       // transform unit z to get the normal to the orbital plane.
        _orbitalNormal  = raisedCenter - _center;
    }

    // Get a point defined in untranslated coordinates to be translated by this sphere's model matrix.
    glm::vec3 getTranslatedSpherePoint(glm::vec3 p)
    {
        return getCenterModelMatrix() * glm::vec4(p, 1.0);
        //return getModelMatrix() * getCenterModelMatrix() * glm::vec4(p, 1.0);
    }

    glm::vec3 getTransformedNorthPole()
    {
        return getModelMatrix() * glm::vec4(0.0f, 0.0f, _radius, 1.0);
    }

    glm::vec3 getTransformedLatitudeLongitude(float lat, float lon, float radiusScale = 1.0f)
    {
        // convert latitude and longitude to point on sphere.
        float alpha = 180 + lon;                // angle from +ve x axis rotating towards +ve y axis.  range: 0 to 360.
        float theta = 90 - lat;                 // angle from +ve z axis.  range: 0 to 180.

        float alpha_rad = glm::radians(alpha);
        float theta_rad = glm::radians(theta);

        float x = _radius * radiusScale * sin(theta_rad) * cos(alpha_rad);
        float y = _radius * radiusScale * sin(theta_rad) * sin(alpha_rad);
        float z = _radius * radiusScale * cos(theta_rad);

        return getModelMatrix() * glm::vec4(x, y, z, 1.0);
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
    glm::vec3 _orbitalNormal = glm::vec3(0.0f, 0.0f, 1.0f);     // perpendicular to the orbital plane
    float _r = 1;
    float _g = 1;
    float _b = 1;
    glm::vec3 _orbitalPlaneColor = glm::vec3(1.0, 1.0, 1.0);


    // Rotation variables
    float _radius = 0;                      // radius of sphere
    float _rotationAngle = 0;               // current rotation angle (increments based on rotation angular velocity).
    float _rotationAngularVelocity = 0;     // angular velocity or rotation around sphere's axis.
    bool   bSyncWithRevolution = false;      // sync with this sphere's revolution
    float _axisRotationAngle = 0;           // 
    float _axisTiltAngle = 0;               // 
    float _axisTiltAngle_Backup = 0;        // allows restoring after modifying axis tilt
    float _axisTiltAngle_Deg = 0;           // to show in ImGui for controlling via slider.
    float _axisTiltOrientationAngle = glm::radians(0.0f);

    // Revolution variables
    float _orbitalRadius = 0;
    float _orbitalRadius_Backup = 0;        // allows restoring after modifying orbital radius
    float _orbitalAngle = 0;                // current revolution (orbital) angle (increments based on revolution angular velocity).
    float _orbitalAngularVelocity = 0;      // angular velocity of revolution around parent sphere.
    bool   bOrbitalRevolutionSyncToParent = false;
    float _nodalPrecessionAngle = 0;   // 
    bool   bNodalPrecessionSpeedSyncToParentsRevolution = false;
    float _orbitalPlaneTiltAngle = 0;       // 
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
