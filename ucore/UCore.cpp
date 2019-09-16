#include "pch.h"
#include <windows.h>

#include <stdio.h>
#include "UCore.h"
#include "Utils.h"
#include <stdio.h>



Universe::Universe()
{
}


Universe::~Universe()
{
}

/*************************************************************************************************


**************************************************************************************************/
void Universe::initSceneObjects()
{
    SetDefaultView();
    generateStars();


    axis.generateVertices(
        1600, 1600, 800,
        glm::vec3(0.2f, 0.2f, 0.5f),        // X axis color
        glm::vec3(0.2f, 0.5f, 0.2f),        // Y axis color
        glm::vec3(0.2f, 0.5f, 0.5f)         // Z axis color
    );
    // Sun
    //---------------------------------------
    //sun.setColor(0.7f, 0.7f, 0.1f);
    sun.setColor(1.0f, 1.0f, 0.6f);
    sun.setRotationParameters(140,          // radius
        0,                                  // initial rotation angle
        0.02f,                              // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(0.0f)                  // axis tilt angle
    );
    sun.setOrbitalParameters(0,             // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                              // revolution velocity
        0,                                  // orbital rotation angle
        0                                   // orbital tilt
    );


    // Earth
    //---------------------------------------
    earth.setColor(0.55f, 0.82f, 1.0f);
    earth.setRotationParameters(80,         // radius
        0,                                  // initial rotation angle
        0.02f,                             // rotation velocity
        glm::radians(270.0f),               // axis rotation angle
        glm::radians(23.5f)                 // axis tilt angle
    );
    earth.setOrbitalParameters(1400,        // radius of orbit
        glm::radians(0.0f),                 // initial orbital angle
        0.001f,                             // revolution velocity
        0.0f,                               // orbital rotation angle
        0                                   // orbital tilt
    );
    earth.setOrbitalPlaneColor(glm::vec3(0.55, 0.82, 1.0));

    // Moon
    //---------------------------------------
    moon.setColor(0.8f, 0.8f, 0.8f);
    moon.setRotationParameters(25,          // radius
        0,                                  // initial rotation angle
        0.005f,                              // rotation velocity
        glm::radians(0.0f),                 // axis rotation angle
        glm::radians(10.0f)                 // axis tilt angle
    );
    moon.setOrbitalParameters(260,          // radius of orbit
        0.0f,                               // initial orbital angle
        0.01f,                             // revolution velocity
        0,                                  // orbital rotation angle
        glm::radians(30.0)                  // orbital tilt
    );
    moon.setOrbitalPlaneColor(glm::vec3(0.8, 0.8, 0.8));

    moon.setParentSphere(&earth);
    earth.setParentSphere(&sun);
}


/*!
****************************************************************************



****************************************************************************/
void Universe::generateStars()
{
    /*  NOTE:
     *  max_z is the thickness of galaxy at a particular radius
     *  it is dynamically calculated for every randomly generated 'r'
     */


    int i;
    double max_dist;
    double max_radius;
    double max_theta;
    double max_z;
    double r, theta, phi;
    double w;
    unsigned char color;  // one component of color

    // generate cubically distributed stars
    i = 0;
    max_dist = 500000.0;
    while (1) {
        if (i == MAXSTARS)
            break;

        star[i].x = max_dist * double(rand()) / RAND_MAX - max_dist / 2;
        star[i].y = max_dist * double(rand()) / RAND_MAX - max_dist / 2;
        star[i].z = max_dist * double(rand()) / RAND_MAX - max_dist / 2;
        color = static_cast<unsigned char>(55) + static_cast<unsigned char>((double(rand()) / RAND_MAX) * 200);
        //star[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        star[i].set_color(color, color, color);

        if (fabs(star[i].x) > 10000 ||
            fabs(star[i].y) > 10000 ||
            fabs(star[i].z) > 10000)
        {
            i++;
            starVertices.push_back(star[i].x);  starVertices.push_back(star[i].y);  starVertices.push_back(star[i].z);   starVertices.push_back(0.8);  starVertices.push_back(0.8);  starVertices.push_back(0.8);  starVertices.push_back(1.0);
        }
    }

    // generate stars for the galaxy
    max_radius = 1000000.0;
    max_theta = 2 * M_PI;
    i = 0;
    while (1) {
        if (i == .7 * MAXGALAXYSTARS)
            break;

        r = max_radius * (double(rand()) / RAND_MAX);
        theta = max_theta * (double(rand()) / RAND_MAX);
        w = r / max_radius;
        if (r < 40000)
            max_z = r;
        else
            max_z = 400000 * (1 / sqrt(1 + 20 * w*w));

        gstar[i].x = r * cos(theta);
        gstar[i].y = r * sin(theta);
        gstar[i].z = max_z * double(rand()) / RAND_MAX - max_z / 2;
        color = static_cast<unsigned char>(55) + static_cast<unsigned char>((double(rand()) / RAND_MAX) * 200);
        //gstar[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        gstar[i].set_color(color, color, color);



        if (fabs(gstar[i].x) > 1000 ||
            fabs(gstar[i].y) > 1000 ||
            fabs(gstar[i].z) > 1000)
            i++;
    }

    while (1) {
        if (i == MAXGALAXYSTARS)
            break;

        r = 200000 * (double(rand()) / RAND_MAX);
        theta = M_PI * (double(rand()) / RAND_MAX);
        phi = 2 * M_PI * (double(rand()) / RAND_MAX);
        gstar[i].x = r * sin(theta) * cos(phi);
        gstar[i].y = r * sin(theta) * sin(phi);
        gstar[i].z = r * cos(theta);
        color = static_cast<unsigned char>((double(rand()) / RAND_MAX) * 255);
        //gstar[i].c = SDL_MapRGB ( space.surface->format, color, color, color );
        gstar[i].set_color(color, color, color);
        i++;

    }

    // shift the Galaxy stars along x axis
    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstar[i].x += max_radius * .66;


    // rotate all stars so that the the plane of the solar system
    // is perpendicular to the plane of the galaxy
    for (i = 0; i < MAXGALAXYSTARS; i++)
        gstar[i] = space.rotate(PNT(0, 0, 0), PNT(100, 0, 0), gstar[i], 80.0);


    gstarVertices.push_back(gstar[i].x);  gstarVertices.push_back(gstar[i].y);  gstarVertices.push_back(gstar[i].z);   gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(0.8);  gstarVertices.push_back(1.0);

}




void Universe::advance(float stepMultiplier)
{
    // Advance parents before children
    sun.advance(stepMultiplier);
    earth.advance(stepMultiplier);
    moon.advance(stepMultiplier);
}





