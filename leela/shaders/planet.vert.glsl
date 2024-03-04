#version 330 core

const float PI = 3.1415926535897932384626433832795;

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float nightColorMultiplier;

uniform vec3  sunCenterTransformed;
uniform float sunRadius;

uniform vec3  otherSphereCenterTransformed;
uniform float otherSphereRadius;
uniform bool  realisticShading;

struct SphereInfo {
    vec3 centerTransformed;
    float radius;
    float sineOfSelfUmbraConeHalfAngle;
};

uniform SphereInfo sphereInfo;

out vec4 Color;
out vec2 TexCoord;
out float darknessFactor;       // multiplier to be applied to the color of vertex based on various factors
                                // such as whether in day/night, where in day, where in shadow, etc.
                                // The actual application of this value is done in fragment shader.
        
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
// See formula derivation at the bottom of this file.
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
    darknessFactor = 1.0;
    float daylightShadingMultiplier = 1.0;

    vec3 modelTransformedPosition   = vec3((model * vec4(position, 1.0)));
    float dist_sun_thisSphere       = distance(sunCenterTransformed, sphereInfo.centerTransformed);
    float dist_sun_thisPoint        = distance(sunCenterTransformed, modelTransformedPosition);
    //float selfUmbraLength           = (sphereInfo.radius * dist_sun_thisSphere) / (sunRadius - sphereInfo.radius);
    //float selfUmbraConeHalfAngle    = asin(sphereInfo.radius / selfUmbraLength);

    vec3 x_normal   = normalize(vec3(model * vec4(normal, 0.0)));
    //vec3 x_position = vec3(model * vec4(position, 1.0));
    float dotProduct = dot(normalize(sunCenterTransformed - sphereInfo.centerTransformed), x_normal);
    //float compareValue = sin(selfUmbraConeHalfAngle);
    
    do {
        // Find if point is in night
        if (realisticShading)
        {
            //if (dotProduct < -compareValue)
            if (dotProduct < -sphereInfo.sineOfSelfUmbraConeHalfAngle)
            {
                // vertex is in night
                darknessFactor = nightColorMultiplier;
                break;
            }
        }
        else
        {
            // calculate distance beyond which this point will be in the night side.  Regardless of the size of the sun,
            // our calculations assume night side to be equal in size to the day side.
            float longest_dist_of_point_in_day = length(vec2(dist_sun_thisSphere, sphereInfo.radius));
        
            if (longest_dist_of_point_in_day < dist_sun_thisPoint)
            {
                // point is on the night side of this sphere.
                // todo - this is only true if radius of sun is same as radius of this planet
                darknessFactor = nightColorMultiplier;
                break;
            }
        }
        
        // Point is in day.  Find if it is in shadow
        {
            //float daylightShadingMultiplier = sqrt(min(1.0, dotProduct+compareValue));
            if (realisticShading)
            {
                daylightShadingMultiplier = sqrt(min(1.0, dotProduct + sphereInfo.sineOfSelfUmbraConeHalfAngle));
                daylightShadingMultiplier = max(daylightShadingMultiplier, nightColorMultiplier);       // don't let color become any darker than night time color.
                darknessFactor = daylightShadingMultiplier;
            }
      
            float dist_sun_otherSphere      = distance(sunCenterTransformed, vec3(otherSphereCenterTransformed));
            
            {
                //---------------------------------------------------------------------------------
                // In this code block, check if point is in penumbra, antumbra, umbra or none.
                // Provide color to the point accordingly.
                //---------------------------------------------------------------------------------
                bool bInUmbra               = false;
                bool bInPenumbra            = false;
                bool bInAntumbra            = false;
                float edgeCloseness = 0.0;
                do
                {
                    if (dist_sun_thisPoint < dist_sun_otherSphere)
                        break;                                              // point might be inside the 'other' sphere, but cannot be in shadow.
        
                    // Calculate some important points and distances about umbra and penumbra.
                    vec3  N                     = nearestPtL(modelTransformedPosition);
                    float dist_N_thisPoint      = distance(N, modelTransformedPosition);
                    float dist_N_sun            = distance(N, sunCenterTransformed);
                    float dist_N_otherSphere    = distance(N, otherSphereCenterTransformed);


                    //===================================================================
                    // Calculate umbra, penumbra and antumbra radius at crosssection
                    //===================================================================

                    //----------------------------------------------
                    // this is the length from the start of imginary cone, between the sun and the 'other' sphere, to otherSphere center.
                    float penumbraLength        = (otherSphereRadius * dist_sun_otherSphere) / (sunRadius + otherSphereRadius);
                    float penumbraConeHalfAngle = asin(otherSphereRadius / penumbraLength);
        
                    // y1 = radius of crosssection of penumbra at N
                    float y1 = (penumbraLength + dist_N_otherSphere) * tan(penumbraConeHalfAngle);

                    //----------------------------------------------
                    // calculate length of umbral cone.
                    float umbraLength           = (otherSphereRadius * dist_sun_otherSphere) / (sunRadius - otherSphereRadius);
                    // Find radius of shadow cone at a cross section taken at the nearest point
                    float umbraConeHalfAngle = asin(otherSphereRadius / umbraLength);
                    // y2 = radius of crosssection of umbra at N
                    float y2 = (umbraLength - dist_N_otherSphere) * tan(umbraConeHalfAngle);


        
                    //===================================================================
                    // Perform checks to determine which shadow type this point is in.
                    //===================================================================

                    //---------------------------------------------------------
                    // Penumbra specific calculation and check
                    if (dist_N_thisPoint < y1)
                    {
                        bInPenumbra = true;
                        edgeCloseness = (dist_N_thisPoint - y2) / (y1 - y2);
                    }
                    
                    // Even if the point was determined to be in penumbra, it might actually be in umbra. So continue to check further.
       
                    //---------------------------------------------------------
                    // Umbra specific calculation and check
                    if (dist_N_thisPoint < y2)
                    {
                        bInUmbra = true;
                    }
        
                    //---------------------------------------------------------
                    // Antumbra specific calculation and check
                    // antumbra half angle cone is same as that umbra.  We will reuse umbraConeHalfAngle.
                    if (umbraLength < dist_N_otherSphere)
                    {
                        // there is a chance the point is in antumbra.
                        float y3 = (dist_N_otherSphere - umbraLength) * tan(umbraConeHalfAngle);
                        if (dist_N_thisPoint < y3)
                        {
                            bInAntumbra = true;
                        }
                    }
                    
                    // TODO - after adding the below condition and the code in it, frame rate has gone down to 50 on home computer integrated graphics.
                    if (bInUmbra || bInPenumbra || bInAntumbra) {
                        // make sure other sphere is above horizon. if not, show as if in full day light.
                        vec3 x_position = vec3(model * vec4(position, 1.0));
                        float dist_position_otherSphereCenter = distance(x_position, otherSphereCenterTransformed);
                        float dotProduct_position_otherSphereCenter = dot(normalize(x_position - otherSphereCenterTransformed), x_normal);
                        float angleAtPositionDueToOtherSphereRadius = 2 * asin(otherSphereRadius/2 / dist_position_otherSphereCenter);
                        if (dotProduct_position_otherSphereCenter > sin(angleAtPositionDueToOtherSphereRadius)) {
                            // TODO As per my calculations on paper, the above condition should have been:
                            //          dotProduct_position_otherSphereCenter < -sin(angleAtPositionDueToOtherSphereRadius
                            //      But if I use it, it doesn't work.  Investigate.  The if condition right now works.                        
                            bInUmbra = false;
                            bInPenumbra = false;
                            bInAntumbra = false;
                        }
                    }                
        
                } while (false);
        
                //----------------------------------------------------------------
                // Apply coloring based on the type of shadow the point is in.
                float umbraDarknessFactor = 0.1;
                if (bInUmbra) {
                    darknessFactor *= umbraDarknessFactor;
                }
                else if (bInAntumbra) {
                    float antumbraDarknessFactor = 0.4;
                    if (realisticShading) {
                        // todo - perform shading based on distance from internal penumbra edge.
                        darknessFactor *= antumbraDarknessFactor;
                    }
                    else {
                        darknessFactor *= antumbraDarknessFactor;
                    }
                }
                else if (bInPenumbra) {
                    // simulate reduction in darkness using a shifted (by +1) cosine curve between 0 & 180 degrees
                    float penumbraDarknessFactor = 0;                    
                    if (realisticShading) {
                        edgeCloseness = sqrt(edgeCloseness);
                        penumbraDarknessFactor = umbraDarknessFactor + 
                                                 ((1.0 - umbraDarknessFactor) * ((- cos(edgeCloseness * PI) / 2.0) + 0.5));
                    }
                    else {
                        penumbraDarknessFactor = umbraDarknessFactor + 0.5;
                    }
                    
                    darknessFactor *= penumbraDarknessFactor;
                }
                else {
                }
            }
        }
    } while (false);
    
    // apply all 3 transformations to the original point
    gl_Position = proj * view * model * vec4(position, 1.0);
    Color = in_color;
    TexCoord = texCoord;
}


/*

Take derivative of the length of segment joining the given point P, and any point on AB.
Equate the derivative to 0 to get the minima.
k is the parameter in the parametric equation of line joining A & B points.

d sqrt(f(k))           1
------------ =   ----------- x f'(k)  =  0
     dk          2 sqrt(f(k))

Therefore,  f'(k) = 0

f(k) = (a_x + k_x - p_x)^2  +  (a_y + k_y - p_y)^2  +  (a_z + k_z - p_z)^2 

                             d(k_x)                          d(k_y)                            d(k_z)
f'(k) = 2 (a_x + k_x - p_x) -------   +  2 (a_y + k_y - p_y) -------  +   2 (a_z + k_z - p_z) -------       = 0
                              dk                              dk                                dk

where:  k_x = k(b_x - a_x)
        k_y = k(b_y - a_y)
        k_z = k(b_z - a_z)


Dividing both sizes by 2.  Substituting k_x, k_y and k_z, and taking their derivative:

    (a_x + k_x - p_x) (b_x - a_x)   +   (a_y + k_y - p_y) (b_y - a_y)  +  (a_z + k_z - p_z) (b_z - a_x)     = 0 

    Expanding k_x, k_y and k_z, and multiplying the brackets:

    k(b_x - a_x)^2 + (a_x - p_x)(b_x - a_x)   +   k(b_y - a_y)^2 + (a_y - p_y)(b_y - a_y)   +   k(b_z - a_z)^2 + (a_z - p_z)(b_z - a_z)   = 0

    Collecting K terms to gether.

    k[(b_x - a_x)^2 + (b_y - a_y)^2 + (b_z - a_z)^2]   +   (a_x - p_x)(b_x - a_x) + (a_y - p_y)(b_y - a_y) + (a_z - p_z)(b_z - a_z)   = 0

    Move constants to the right side. They become negative.  Change a_x - p_x to p_x - a_x to eliminate the negative sign.

    k[(b_x - a_x)^2 + (b_y - a_y)^2 + (b_z - a_z)^2]   =  (p_x - a_x)(b_x - a_x) + (p_y - a_y)(b_y - a_y) + (p_z - a_z)(b_z - a_z) 

               (p_x - a_x)(b_x - a_x) + (p_y - a_y)(b_y - a_y) + (p_z - a_z)(b_z - a_z)
    k =       ----------------------------------------------------------------------------
                         (b_x - a_x)^2 + (b_y - a_y)^2 + (b_z - a_z)^2


  Denominator is the square of the distance between A & B.


                 (p_x - a_x)(b_x - a_x) + (p_y - a_y)(b_y - a_y) + (p_z - a_z)(b_z - a_z)
    k =       ----------------------------------------------------------------------------
                                          dist(AB) ^ 2

*/
