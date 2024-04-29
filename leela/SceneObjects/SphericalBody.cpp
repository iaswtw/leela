#include "SphericalBody.h"


void SphericalBody::setRotationParameters(float radius, float rotationAngle, float rotationAngularVelocity, float axisTiltOrientationAngle, float axisTiltAngle)
{
    _radius = radius;
    _radius_Backup = _radius;
    _rotationAngle = rotationAngle;
    _rotationAngularVelocity = rotationAngularVelocity;
    _axisTiltOrientationAngle = axisTiltOrientationAngle;
    _axisTiltAngle = axisTiltAngle;
    _axisTiltAngle_Backup = axisTiltAngle;
    _axisTiltAngle_Deg = glm::degrees(_axisTiltAngle);
}

void SphericalBody::setOrbitalParameters(float orbitalRadius, float orbitalAngle, float orbitalAngularVelocity, float nodalPrecessionInitialAngle, float orbitalPlaneTiltAngle)
{
    setOrbitalRadius(orbitalRadius);
    _orbitalRadius_Backup = _orbitalRadius;

    _orbitalAngle = orbitalAngle;
    _orbitalAngularVelocity = orbitalAngularVelocity;
    _nodalPrecessionAngle = nodalPrecessionInitialAngle;
    _orbitalPlaneTiltAngle = orbitalPlaneTiltAngle;
    _orbitalPlaneTiltAngle_Deg = glm::degrees(_orbitalPlaneTiltAngle);
}




void SphericalBody::advance(float stepMultiplier)
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
            if (_sphericalBodyParent != nullptr)
                increment = _sphericalBodyParent->_orbitalAngularVelocity * 12.3f;       // hardcode for moon
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

            if (_sphericalBodyParent != nullptr)
                increment = _sphericalBodyParent->_orbitalAngularVelocity / 18.6;        // hardcode this for moon
        }
        _nodalPrecessionAngle -= increment * stepMultiplier;
        _nodalPrecessionAngle = _normalizeAngle(_nodalPrecessionAngle);
    }

    calculateCenterPosition();
}

void SphericalBody::parentChanged()
{
    _sphericalBodyParent = dynamic_cast<SphericalBody*>(_sceneParent);
}



// Calculates center position based on the value of current parameters
void SphericalBody::calculateCenterPosition()
{
    glm::mat4 mat = getPositionTransform();
    glm::vec3 raisedCenter;

    _center = mat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);       // transform point at origin to form the transformed sphere's center.
    raisedCenter = mat * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);       // transform unit z to get the normal to the orbital plane.
    _orbitalNormal = raisedCenter - _center;
}

// Get a point defined in untranslated coordinates to be translated by this sphere's model matrix.
glm::vec3 SphericalBody::getTranslatedSpherePoint(glm::vec3 p)
{
    return getPositionTransform() * glm::vec4(p, 1.0);
    //return getTransform() * glm::vec4(p, 1.0);
}

glm::vec3 SphericalBody::getTransformedNorthPole()
{
    return getTransform() * glm::vec4(0.0f, 0.0f, _radius, 1.0);
}

//
// Return world coordinates of a point at given latitude and longitude.
// `radiusScale` tells where the point is along the radius vector of the latitude and longitude.
//    - if 1, point is on the sphere
// 
glm::vec3 SphericalBody::getTransformedLatitudeLongitude(float lat, float lon, float radiusScale)
{
    // convert latitude and longitude to point on sphere.
    float alpha = 180 + lon;                // angle from +ve x axis rotating towards +ve y axis.  range: 0 to 360.
    float theta = 90 - lat;                 // angle from +ve z axis.  range: 0 to 180.

    float alpha_rad = glm::radians(alpha);
    float theta_rad = glm::radians(theta);

    float x = _radius * radiusScale * sin(theta_rad) * cos(alpha_rad);
    float y = _radius * radiusScale * sin(theta_rad) * sin(alpha_rad);
    float z = _radius * radiusScale * cos(theta_rad);

    return getTransform() * glm::vec4(x, y, z, 1.0);
}



// Transforms a point on an upright sphere at origin to the tilted sphere at origin.
//    - use axial tilt, rotation position and precession of the sphere.
glm::mat4 SphericalBody::getOrientationTransform()
{
    glm::mat4 mat(1.0f);

    //-------------------------------------------------------------------
    // Transformation matrix is created in reverse order of intended application to the transformed point.
    //-------------------------------------------------------------------

    // tilt and rotate axis. 
    // todo - do this using a single rotate invocation
    //modelTrans = glm::rotate(modelTrans,   getAxisRotationAngle(),   glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 axisTiltOrientationAxis = getAxisTiltOrientationAxis();

    mat = glm::rotate(mat, _axisTiltAngle, axisTiltOrientationAxis);
    mat = glm::rotate(mat, _rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));

    return mat;
}


// returns a vec3 in x-y plane (z=0)
glm::vec3 SphericalBody::getAxisTiltOrientationAxis()
{
    glm::vec3 axisTiltOrientationAxis = glm::vec3(cos(_axisTiltOrientationAngle), sin(_axisTiltOrientationAngle), 0.0f);
    return axisTiltOrientationAxis;
}

glm::mat4 SphericalBody::getOrbitalPlaneModelMatrix()
{
    glm::mat4 modelTrans(1.0f);

    // translate to the parent's center.
    //printf("getOrbitalPlaneModelMatrix: sphere name = %s, _parent = %p\n", _name.c_str(), _parent);
    if (_sceneParent != nullptr)
    {
        modelTrans = _sceneParent->getPositionTransform();
        modelTrans = glm::rotate(modelTrans, _nodalPrecessionAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        modelTrans = glm::rotate(modelTrans, _orbitalPlaneTiltAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    return modelTrans;
}

glm::vec3 SphericalBody::getModelTransformedCenter()
{
    return _center;
}

// Get the model matrix of the center of this sphere.
// - This includes first getting the parent's center's model matrix.
glm::mat4 SphericalBody::getPositionTransform()
{
    glm::mat4 mat(1.0f);

    //-------------------------------------------------------------------
    // Transformation matrix is created in reverse order of intended application to the transformed point.
    //-------------------------------------------------------------------

    // parent is assumed to have been updated.  Translate by an amount equal to the parent's position.
    if (_sceneParent != nullptr)
        mat = _sceneParent->getPositionTransform();

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