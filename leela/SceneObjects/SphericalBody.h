#pragma once

// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <SceneObject.h>
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
    Color() : r(1), g(1), b(1), a(1)  {}
    ~Color() {}
};


class SphericalBody : public SceneObject
{
public:
    SphericalBody(std::string name)
        : SceneObject(name)
    {}
    ~SphericalBody() {}

    virtual void init() {}
    void advance(float stepMultiplier);

    virtual void parentChanged();

    void setColor(glm::vec3 color)                          { _r = color.r; _g = color.g; _b = color.b; _color = color; }
    void setColor(float r, float g, float b)                { _r = r; _g = g; _b = b; _color = glm::vec3(r, g, b); }
    void setName(std::string name)                          { _name = name; }
    //void setOrbitalPlaneColor(glm::vec3 orbitalPlaneColor)  { _orbitalPlaneColor = orbitalPlaneColor; }
    void setOrbitalRadius(float orbitalRadius)              { _orbitalRadius = orbitalRadius; }
    void setOrbitalAngle(float orbitalAngle)                { _orbitalAngle = orbitalAngle; }
    inline glm::vec3& getCenter()                           { return _center; }
    inline float getRadius()                                { return _radius; }
    void restoreRadius()                                    { _radius = _radius_Backup;  }
    inline float _normalizeAngle(float angle)               { return (float) fmod(angle, 2 * M_PI);  }

    // related sphere is a sphere that is close enough to this sphere so as to possibly cast a shadow
    // on this sphere.
    void setRelatedSphere(SphericalBody * relatedSphere)           { _relatedSphere = relatedSphere; }
    void setSunSphere(SphericalBody* sunSphere)                    { _sunSphere = sunSphere; }
    void restoreOrbitalRadius()                             { _orbitalRadius = _orbitalRadius_Backup; }
    void restoreAxisTiltAngleFromBackup()                   { _axisTiltAngle = _axisTiltAngle_Backup;  _axisTiltAngle_Deg = glm::degrees(_axisTiltAngle); }

    void setRotationParameters(float radius, float rotationAngle, float rotationAngularVelocity, float axisTiltOrientationAngle, float axisTiltAngle);
    void setOrbitalParameters(float orbitalRadius, float orbitalAngle, float orbitalAngularVelocity, float nodalPrecessionInitialAngle, float orbitalPlaneTiltAngle);

    glm::mat4 getOrbitalPlaneModelMatrix();
    glm::vec3 getModelTransformedCenter();
    glm::mat4 getPositionTransform();
    glm::mat4 getOrientationTransform();
    glm::vec3 getAxisTiltOrientationAxis();
    glm::vec3 getTranslatedSpherePoint(glm::vec3 p);
    glm::vec3 getTransformedNorthPole();
    glm::vec3 getTransformedLatitudeLongitude(float lat, float lon, float radiusScale = 1.0f);
    void calculateCenterPosition();

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
    glm::vec3 _color = { 1.0f, 1.0f, 1.0f };
    //glm::vec3 _orbitalPlaneColor = glm::vec3(1.0, 1.0, 1.0);

    bool bShowCityBookmarks = false;

    // Rotation variables
    float _radius = 0;                      // radius of sphere
    float _radius_Backup = 0;
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

    SphericalBody* _relatedSphere = nullptr;
    SphericalBody* _sunSphere = nullptr;

    SphericalBody* _sphericalBodyParent = nullptr;  // e.g. if this is moon, _parent is earth.
                                                    // TODO: this will change when we bring in center of mass virtual SphericalBody object.
    std::string _name = "NoName";

};
