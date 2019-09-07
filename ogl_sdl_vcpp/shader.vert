R"glsl(
    #version 150 core

    in vec3 position;
    in vec3 in_color;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;

    uniform vec4  sunCenterTransformed;
    uniform float sunRadius;

    uniform vec4  otherSphereCenterTransformed;
    uniform float otherSphereRadius;

    struct SphereInfo {
        bool isLightSource;
        bool isValid;
        vec3 center;
        float radius;
    };

    uniform SphereInfo sphereInfo;

    out vec4 Color;
        

    vec3 nearestPtL(vec3 modelTransformedPosition)
    {
        float dist = distance(sunCenterTransformed, otherSphereCenterTransformed);
        float den = dist * dist;
        float k = 0;
        if (den > 0.000001)
        {
            k = ((modelTransformedPosition.x - sunCenterTransformed.x) * (otherSphereCenterTransformed.x - sunCenterTransformed.x) +
                    (modelTransformedPosition.y - sunCenterTransformed.y) * (otherSphereCenterTransformed.y - sunCenterTransformed.y) +
                    (modelTransformedPosition.z - sunCenterTransformed.z) * (otherSphereCenterTransformed.z - sunCenterTransformed.z)) / den;
        }
        else
        {
            k = 0.5;
        }

        vec3 N = vec3(sunCenterTransformed.x + (otherSphereCenterTransformed.x - sunCenterTransformed.x) * k,
                        sunCenterTransformed.y + (otherSphereCenterTransformed.y - sunCenterTransformed.y) * k,
                        sunCenterTransformed.z + (otherSphereCenterTransformed.z - sunCenterTransformed.z) * k);
        return N;
    }

    void main()
    {
        if (!sphereInfo.isLightSource)
        {
            if (sphereInfo.isValid)
            {
                vec4 modelTransformedMyCenter   = model * vec4(sphereInfo.center, 1.0);
                float r                         = length(vec2(length(modelTransformedMyCenter),
                                                            sphereInfo.radius));

                vec3 modelTransformedPosition   = vec3((model * vec4(position, 1.0)).xyz);

                if (r < length(modelTransformedPosition))
                {
                    // point is on the night side of this sphere.
                    // todo - this is only true if radius of sun is same as radius of this planet
                    Color = vec4(in_color*0.2, 1.0);
                }
                else
                {
                    bool bInUmbra               = false;
                    bool bInPenumbra            = false;
                    bool bInAntumbra            = false;
                    do
                    {
                        float dist_sun_thisPoint    = distance(vec3(sunCenterTransformed), modelTransformedPosition);
                        float dist_sun_otherSphere  = distance(sunCenterTransformed, otherSphereCenterTransformed);

                        if (dist_sun_thisPoint < dist_sun_otherSphere)
                            break;                                              // point might be inside the 'other' sphere, but cannot be in shadow.

                        //-------------------------------------------------------------
                        // Check if point is in penumbra OR negative umbra OR umbra
                        //-------------------------------------------------------------
                        // todo - check if point is in penumbra
                        //  on the line joining the centers of Sun and otherSphere, find a point between them that is the vertex of the cone of penumbra.
                        float h2 = dist_sun_otherSphere * otherSphereRadius / (otherSphereRadius + sunRadius);
                        if (h2 > otherSphereRadius)
                        {
                            float coneHalfAngle = asin(otherSphereRadius / h2);

                                
                            //float y = tan(coneHalfAngle) * distance(N, h

                        }

                        // todo - check if point is in negative umbra


                        vec3  N                     = nearestPtL(modelTransformedPosition);
                        float dist_N_thisPoint      = distance(N, modelTransformedPosition);
                        float dist_N_sun            = distance(N, vec3(sunCenterTransformed));

                        // calculate length of umbral cone.
                        float umbraLength           = (sunRadius * dist_sun_otherSphere) / (sunRadius - otherSphereRadius);

                        // Find radius of shadow cone at a cross section taken at the nearest point
                        float umbraConeHalfAngle = asin(sunRadius / umbraLength);
                        float y = (umbraLength - dist_N_sun) * tan(umbraConeHalfAngle);
                        
                        if (dist_N_thisPoint < y)
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
                        Color = vec4(in_color, 1.0);
                }
            }
            else
            {
                Color = vec4(in_color, 1.0);
            }
        }
        else
        {
            Color = vec4(in_color, 1.0);
        }
            
        // apply all 3 transformations to the original point
        gl_Position = proj * view * model * vec4(position, 1.0);
    }
)glsl";