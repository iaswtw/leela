#version 150 core

in vec3 position;
in vec3 in_color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3  sunCenterTransformed;
uniform float sunRadius;

uniform vec3  otherSphereCenterTransformed;
uniform float otherSphereRadius;

struct SphereInfo {
    bool isLightSource;
    bool isValid;
    vec3 centerTransformed;
//    float radius;
};

uniform SphereInfo sphereInfo;

out vec4 Color;
        
//--------------------------------------------------------------------------------------------------------------------------
// Terminology:
//--------------------------------------------------------------------------------------------------------------------------
//   thisPoint - the point being rendered
//   thisSphere - the sphere on which thisPoint might exist. Why 'might'? Because this point might belong to axis or
//                orbital plane, in which case, it is not part of any sphere.
//   otherSphere - the center of a sphere that can cause thisPoint to be in its shadow.  Only one such sphere is supported at the moment.
//                 E.g. otherSphere will be set to the center of moon when the thisPoint is a point on the earth.


//-------------------------------------------------------------------------------------------------
// Calculate perpendicular from the given point in argument (modelTransformedPosition, i.e. thisPoint
// transformed using its model matrix) point on the line joining the sun's center and the
// otherSphere's center.
//-------------------------------------------------------------------------------------------------
vec3 nearestPtL(vec3 modelTransformedPosition)
{
    float dist = distance(sunCenterTransformed, otherSphereCenterTransformed);
    float den = dist * dist;
    float k = 0.0;
    if (den > 0.000001)
    {
        k = ((modelTransformedPosition.x - otherSphereCenterTransformed.x) * (sunCenterTransformed.x - otherSphereCenterTransformed.x) +
             (modelTransformedPosition.y - otherSphereCenterTransformed.y) * (sunCenterTransformed.y - otherSphereCenterTransformed.y) +
             (modelTransformedPosition.z - otherSphereCenterTransformed.z) * (sunCenterTransformed.z - otherSphereCenterTransformed.z)) / den;
    }
    else
    {
        k = 0.5;
    }

    vec3 N = vec3(otherSphereCenterTransformed.x + (sunCenterTransformed.x - otherSphereCenterTransformed.x) * k,
                  otherSphereCenterTransformed.y + (sunCenterTransformed.y - otherSphereCenterTransformed.y) * k,
                  otherSphereCenterTransformed.z + (sunCenterTransformed.z - otherSphereCenterTransformed.z) * k);
    return N;
}

void main()
{
    if (!sphereInfo.isLightSource)
    {
        if (sphereInfo.isValid)
        {
            vec3 modelTransformedPosition   = vec3((model * vec4(position, 1.0)));
            float dist_sun_thisPoint    = distance(sunCenterTransformed, modelTransformedPosition);
            float dist_sun_thisSphere   = distance(sunCenterTransformed, sphereInfo.centerTransformed);
            float dist_sun_otherSphere  = distance(sunCenterTransformed, vec3(otherSphereCenterTransformed));

            if (dist_sun_thisSphere < dist_sun_thisPoint)
            {
                // point is on the night side of this sphere.
                // todo - this is only true if radius of sun is same as radius of this planet
                Color = vec4(in_color*0.2, 1.0);
            }
            else
            {
                //---------------------------------------------------------------------------------
                // In this code block, check if point is in penumbra, antumbra, umbra or none.
                // Provide color to the point accordingly.
                //---------------------------------------------------------------------------------
                bool bInUmbra               = false;
                bool bInPenumbra            = false;
                bool bInAntumbra            = false;
                do
                {
                    if (dist_sun_thisPoint < dist_sun_otherSphere)
                        break;                                              // point might be inside the 'other' sphere, but cannot be in shadow.

                    // Calculate some important points and distances about umbra and penumbra.
                    vec3  N                     = nearestPtL(modelTransformedPosition);
                    float dist_N_thisPoint      = distance(N, modelTransformedPosition);
                    float dist_N_sun            = distance(N, sunCenterTransformed);
                    float dist_N_otherSphere    = distance(N, otherSphereCenterTransformed);

                    //---------------------------------------------------------
                    // Penumbra specific calculation and check
                    //---------------------------------------------------------
                    // this is the length from the start of imginary cone, between the sun and the 'other' sphere, to otherSphere center.
                    float penumbraLength        = (sunRadius * dist_sun_otherSphere) / (sunRadius - otherSphereRadius);
                    float penumbraConeHalfAngle = asin(otherSphereRadius / penumbraLength);

                    // y2 = radius of crosssection of penumbra at N
                    float y1 = (penumbraLength + dist_N_otherSphere) * tan(penumbraConeHalfAngle);
                    if (dist_N_thisPoint < y1)
                        bInPenumbra = true;
                    
                    // Even if the point was determined to be in penumbra, it might actually be in umbra. So continue to check further.

                    // todo - check if point is in antumbra


                    //---------------------------------------------------------
                    // Umbra specific calculation and check
                    //---------------------------------------------------------
                    // calculate length of umbral cone.
                    float umbraLength           = (otherSphereRadius * dist_sun_otherSphere) / (sunRadius - otherSphereRadius);

                    // Find radius of shadow cone at a cross section taken at the nearest point
                    float umbraConeHalfAngle = asin(otherSphereRadius / umbraLength);
                    
                    // y2 = radius of crosssection of umbra at N
                    float y2 = (umbraLength - dist_N_otherSphere) * tan(umbraConeHalfAngle);
                        
                    if (dist_N_thisPoint < y2)
                        bInUmbra = true;

                } while (false);

                //----------------------------------------------------------------
                // Apply coloring based on the type of shadow the point is in.
                if (bInUmbra)
                    Color = vec4(in_color * 0.2, 1.0);
                else if (bInAntumbra)
                    Color = vec4(in_color * 0.3, 1.0);
                else if (bInPenumbra)
                    Color = vec4(in_color * 0.4, 1.0);
                else
                    //Color = vec(1.0, 0.0, 0.0, 1.0);
                    Color = vec4(in_color, 1.0);
                        
            }
        }
        else    // if (sphereInfo.isValid)
        {
            Color = vec4(in_color, 1.0);
            //Color = vec4(0.0, 1.0, 0.0, 1.0);
        }
    }
    else
    {
        Color = vec4(in_color, 1.0);
    }
            
    // apply all 3 transformations to the original point
    gl_Position = proj * view * model * vec4(position, 1.0);
}
