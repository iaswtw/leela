
#include "Elements.h"

PlanetInfo planetInfo[] = {
    // name         parent      Related     radius  rotation    rotation   axis tilt    axis tilt   orbital orbital orbital     nodal       orbital     color                   polygon     texture filename
    //              name        object              angle       angular    orientation  angle       radius  angle   angular     precession  plane tilt                          count   
    //                          names                           velocity   angle                                    velocity    init angle  angle
    {  "Sun",       "",         {},         160,    0,          0.002f,    0.0f,        0.0f,       0,      0.0f,   0.01f,      0,          0,          {1.0f, 1.0f, 0.6f},     "low",      "sunmap.png"                    },
    {  "Mercury",   "Sun",      {},         10,     0,          0.0004f,   0.0f,        2.0f,       700,    90.0f,  0.004f,     0.0f,       7.0f,       {0.8f, 0.8f, 0.8f},     "medium",   "2k_mercury.png"                },
    {  "Earth",     "Sun",      {"Moon"},   80,     0,          0.02f,     0.0f,        23.5f,      3000,   30.0f,  0.001f,     0.0f,       0,          {0.55f, 0.82f, 1.0f},   "medium",   "earth-erde-mit-wolken-2k.png"  },
    {  "Moon",      "Earth",    {"Earth"},  22,     0,          0.008888f, 0.0f,        10.0f,      400,    0.0f,   0.0088888f, 0,          30.0f,      {0.8f, 0.8f, 0.8f},     "medium",   "moonmap1k.png"                 },
    //{  "Earth2",    "Sun",      {},         80,     0,          0.02f,     0.0f,        23.5f,      3000,   60.0f,  0.001f,     0.0f,       0,          {0.55f, 0.82f, 1.0f},   "medium",   "earth-erde-mit-wolken-2k.png"  },
    //{  "Moon2",      "Earth2",   {"Earth2"}, 22,     0,          0.008888f, 0.0f,        10.0f,      400,    0.0f,   0.0088888f, 0,          30.0f,      {0.8f, 0.8f, 0.8f},     "medium",   "moonmap1k.png"                 },
    {  "Mars",      "Sun",      {},         60,     0,          0.02f,     270.0f,      25.0f,      7000,   0,      0.0003f,    0.0f,       1.85f,      {0.85f, 0.5f, 0.5f},    "low",      "mars2k.png"                    },
    {  "Jupiter",   "Sun",      {},         140,    0,          0.02f,     270.0f,      3.1f,       11000,  280.0f, 0.00012f,    0.0f,       1.31f,      {0.85f, 0.7f, 0.6f},    "low",      "jupiter2k.png"                 },
    {  "Saturn",    "Sun",      {},         110,    0,          0.02f,     270.0f,      26.7f,      15000,  220.0f, 0.00006f,    0.0f,       2.49f,      {0.7f, 0.7f, 0.4f},     "low",      "saturn2k.png"                  },
    {  "Uranus",    "Sun",      {},         90,     0,          0.02f,     270.0f,      97.7f,      20000,  18.0f,  0.00003f,    0.0f,       0.77f,      {0.7f, 0.7f, 0.85f},    "low",      "uranus2k.png"                  },
    {  "Neptune",   "Sun",      {},         80,     0,          0.02f,     270.0f,      28.0f,      25000,  150.0f, 0.000021f,   0.0f,       1.77f,      {0.4f, 0.4f, 0.9f},     "low",      "neptune2k.png"                 }
};


PlanetInfo planetInfoToScale[] = {
    // name         parent      Related     radius  rotation    rotation   axis tilt    axis tilt   orbital        orbital orbital     nodal       orbital     color                   polygon     texture filename
    //              name        object              angle       angular    orientation  angle       radius         angle   angular     precession  plane tilt                          count   
    //                          names                           velocity   angle                                           velocity    init angle  angle
    {  "Sun",       "",         {},         10.9,    0,          0.00074f,  0.0f,        0.0f,       0,             0.0f,   0.01f,      0,          0,          {1.0f, 1.0f, 0.6f},     "low",      "sunmap.png"                    },
    {  "Mercury",   "Sun",      {},         0.0383,  0,          0.00034f,  0.0f,        0.01f,      908.872,        90.0f, 0.00414f,   0.0f,       7.0f,       {0.8f, 0.8f, 0.8f},     "medium",   "2k_mercury.png"                },
    {  "Venus",     "Sun",      {},         0.0949,  0,          -0.000082f, 0.0f,       2.64f,      1697.971,       90.0f, 0.001626f,  0.0f,       3.39f,      {0.8f, 0.8f, 0.8f},     "medium",   "2k_mercury.png"                },
    {  "Earth",     "Sun",      {"Moon"},   0.1,      0,          0.02f,     0.0f,        23.5f,      2348.508,       30.0f,  0.001f,   0.0f,       0,          {0.55f, 0.82f, 1.0f},   "medium",   "earth-erde-mit-wolken-2k.png"  },
    {  "Moon",      "Earth",    {"Earth"},  0.0272,  0,          0.000729f, 0.0f,        1.54f,      6.035,          0.0f,  0.000689f,  0,          5.25f,      {0.8f, 0.8f, 0.8f},     "medium",   "moonmap1k.png"                 },
    {  "Mars",      "Sun",      {},         0.0532,  0,          0.019417f, 270.0f,      25.2f,      3569.733,       0,      0.0005319f, 0.0f,       1.85f,      {0.85f, 0.5f, 0.5f},    "low",      "mars2k.png"                    },
    {  "Jupiter",   "Sun",      {},         1.121,  0,          0.048192f, 270.0f,      3.1f,       12212.244,      280.0f, 0.000084f,  0.0f,       1.31f,      {0.85f, 0.7f, 0.6f},    "low",      "jupiter2k.png"                 },
    {  "Saturn",    "Sun",      {},         0.945,   0,          0.044943f, 270.0f,      26.7f,      22475.227,      220.0f, 0.0000340f, 0.0f,       2.49f,      {0.7f, 0.7f, 0.4f},     "low",      "saturn2k.png"                  },
    {  "Uranus",    "Sun",      {},         0.401,   0,          -0.02777f, 270.0f,      82.23f,     45020.910,      18.0f,  0.0000119f, 0.0f,       0.77f,      {0.7f, 0.7f, 0.85f},    "low",      "uranus2k.png"                  },
    {  "Neptune",   "Sun",      {},         0.388,   0,          0.029717f, 270.0f,      28.3f,      70877.990,      150.0f, 0.0000061f, 0.0f,       1.77f,      {0.4f, 0.4f, 0.9f},     "low",      "neptune2k.png"                 }

};

int numPlanetInfo = sizeof(planetInfo) / sizeof(planetInfo[0]);
int numPlanetInfoToScale = sizeof(planetInfoToScale) / sizeof(planetInfoToScale[0]);


