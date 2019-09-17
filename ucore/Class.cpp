#include "pch.h"

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
