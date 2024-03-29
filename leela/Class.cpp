#include "Class.h"

/*****************************************************************************************
 NOTE on translate and translated family of functions:

 translate()
     Returns void. Error should result if attempted to assign the return value of this method.

 translated()
    Returns a *new* 3-d point.

 translated[X|Y|Z]()
   Returns a *new* 3-d point translated along the specified axis.


 ****************************************************************************************)/

/*
 * Translate this point by the given 'amount' in the given 'direction'.
 */
void PNT::translate(double amount, VECTOR& direction)
{
    set(x + amount * direction.l,
        y + amount * direction.m,
        z + amount * direction.n);
}

/*
 * Returns a new 3-d point that is a translated version of this point by 'amount'
 * in 'direction'.
 */
PNT PNT::translated(double amount, VECTOR& direction)
{
    return PNT(x + amount * direction.l,
        y + amount * direction.m,
        z + amount * direction.n);
}

PNT PNT::translatedX(double amount)
{
    return PNT(x + amount, y, z);
}

PNT PNT::translatedY(double amount)
{
    return PNT(x, y + amount, z);
}

/*
 * Returns a new 3-d point that is a translated version of this point by 'amount'
 * along Z axis.
 */
PNT PNT::translatedZ(double amount)
{
    return PNT(x, y, z + amount);
}

// create and return a new vector where:
//  - the first point of the vector is `this` point.
//  - the second point is formed by translating the `this` point in the direction of `direction_vector`.
//  - length of new vector is same as the length of `direction_vector`.
VECTOR PNT::extrude(VECTOR& direction_vector)
{
    return extrude(direction_vector, direction_vector.length());
}

// create and return a new vector where:
//  - the first point of the vector is `this` point.
//  - the second point is formed by translating the `this` point in the direction of `direction_vector`.
//  - length of new vector is the given `amount`.
VECTOR PNT::extrude(VECTOR& direction_vector, float amount)
{
    PNT p2 = translated(amount, direction_vector);
    VECTOR v(*this, p2);
    return v;
}



/*
 * Returns the square of the distance between given point and this point.
 */
double PNT::squaredDistanceTo(PNT& p)
{
    return (p.x - x) * (p.x - x) +
        (p.y - y) * (p.y - y) +
        (p.z - z) * (p.z - z);
}

double PNT::distanceTo(PNT& p)
{
    double sq_dist = squaredDistanceTo(p);
    return sqrt(sq_dist);
}
