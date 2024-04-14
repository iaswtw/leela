#pragma once


// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <iostream>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;



class PT
{
public:
    double x, y;
    struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } color;

    int c;
    PT()
        : c(-1), x(0.0), y(0.0), color{ 0, 0, 0 }
    {
    }
    PT(double xx, double yy, int cc = -1)
        : color{ 0, 0, 0 }
    {
        x = xx; y = yy; c = cc;
    }
    void set(double xx, double yy, int cc = -1)
    {
        x = xx; y = yy; c = cc;
    }
    void set_color(unsigned char r_, unsigned char g_, unsigned char b_) {
        color.r = r_;
        color.g = g_;
        color.b = b_;
    }
    void disp()
    {
        cout << "\nx = " << x << " y = " << y;
    }
};


class VECTOR;

class PNT : public PT
{
public:
    double z;
    PNT()
        : z(0.0)
    { 
    }
    PNT(double xx, double yy, double zz, int cc = -1)
    {
        x = xx; y = yy; z = zz; c = cc;
    }
    PNT(glm::vec3 v)
    {
        x = v.x;  y = v.y;  z = v.z;
    }
    void set(double xx, double yy, double zz, int cc = -1)
    {
        x = xx; y = yy; z = zz; c = cc;
    }
    glm::vec3 toVec3()
    {
        return glm::vec3(x, y, z);
    }
    void translate(double amount, VECTOR& direction);
    PNT  translated(double amount, VECTOR direction);
    PNT translatedX(double amount);
    PNT translatedY(double amount);
    PNT translatedZ(double amount);
    VECTOR extrude(VECTOR& direction);
    VECTOR extrude(VECTOR& direction, float amount);

    double squaredDistanceTo(PNT& p);
    double distanceTo(PNT& p);

    void disp()
    {
        cout << "x = " << x << " y = " << y << " z = " << z << endl;
    }
    PNT operator +(PNT p)
    {
        double xx = x + p.x;
        double yy = y + p.y;
        double zz = z + p.z;
        return PNT(xx, yy, zz);
    }
    PNT& operator +=(PNT p)
    {
        x += p.x;
        y += p.y;
        z += p.z;
        return *this;
    }
    PNT operator -(PNT p)
    {
        double xx = x - p.x;
        double yy = y - p.y;
        double zz = z - p.z;
        return PNT(xx, yy, zz);
    }
};


class SphPnt
{
public:
    double r, theta, phi;

public:
    SphPnt() {
        set(PNT(0, 0, 0));
    }

    SphPnt(PNT cp) {
        set(cp);
    }

    void set(PNT cp) {
        r = sqrt(cp.x*cp.x + cp.y + cp.y + cp.z*cp.z);
        if (r == 0) {
            theta = 0;
            phi = 0;
            return;
        }

        theta = acos(cp.z / r);

        phi = asin(cp.y / sqrt(cp.x*cp.x + cp.y*cp.y));
        if (cp.x >= 0) {
            if (cp.y < 0)
                phi = 2 * M_PI + phi;
        }
        else {
            phi = M_PI - phi;
        }
    }

};


class VECTOR
{
public:
    double x, y, z;         // x, y, z components of the vector
    double l, m, n;         // directional cosines in x, y and z direction
    double d;               // length of the vector
    VECTOR()
        : x(0.0), y(0.0), z(0.0), l(0.0), m(0.0), n(0.0), d(0.0)
    {
    }

    VECTOR(double xx, double yy, double zz)
    {
        x = xx; y = yy; z = zz; calc();
    }
    VECTOR(PNT P1, PNT P2)
    {
        x = P2.x - P1.x; y = P2.y - P1.y; z = P2.z - P1.z; calc();
    }
    VECTOR(glm::vec3 v)
    {
        x = v.x; y = v.y; z = v.z; calc();
    }
    void SET(PNT P1, PNT P2)
    {
        x = P2.x - P1.x; y = P2.y - P1.y; z = P2.z - P1.z; calc();
    }
    void SET(SphPnt sp) {
        x = sp.r * sin(sp.theta) * cos(sp.phi);
        y = sp.r * sin(sp.theta) * sin(sp.phi);
        z = sp.r * cos(sp.theta);
        calc();
    }

    void set(double xx, double yy, double zz)
    {
        x = xx; y = yy; z = zz; calc();
    }
    void calc()
    {
        d = x * x + y * y + z * z;
        if (d <= 0)
            d = 0;
        else
            d = sqrt(d);

        if (d == 0)
        {
            l = 0; m = 0; n = 0;
        }
        else
        {
            l = x / d; m = y / d; n = z / d;
        }
    }
    void disp()
    {
        printf("x=%.4f; y=%.4f; z=%.4f; d=%.4f; l=%.4f; m=%.4f; n=%.4f\n", x, y, z, d, l, m, n);
    }
    double length()
    {
        return d;
    }
    VECTOR operator -()
    {
        return VECTOR(-x, -y, -z);
    }

    PNT PointAtK(double k) {
        return PNT(l*k*d, m*k*d, n*k*d);
    }

    double GetTheta() {
        SphPnt p(PNT(l, m, n));
        return p.theta;
    }

    double GetPhi() {
        SphPnt p(PNT(l, m, n));
        return p.phi;
    }
    glm::vec3 getGlmVec3()
    {
        return glm::vec3(x, y, z);
    }
    // returns cross product of this vector and `other` vector.
    VECTOR cross(VECTOR other)
    {
        VECTOR prod;
        prod.x =   y * other.z - z * other.y;
        prod.y = -(x * other.z - z * other.x);
        prod.z =   x * other.y - y * other.x;
        prod.calc();
        return (prod);
    }
    //returns dot product of this vector and `other` vector.
    double dot(VECTOR other)
    {
        double prod = x * other.x + y * other.y + z * other.z;
        return (prod);
    }
    // Returns the angle to be traversed when rotating from the `other` vector to this vector.
    // Reference vector `ref` must be perpendicular to this vector.  It is used to decide the sign of rotation.
    //  - If `other` falls on the same side as `ref`, angle is positive.
    //     - `other` and `ref` are on the same half circle w.r.t. `this` vector if dot product of `other` and `ref` is positive.
    //
    float angleFrom(VECTOR other, VECTOR ref)
    {
        double cos_of_angle = dot(other) / length() / other.length();

        //spdlog::info("angleFrom: cos_of_angle = {}", cos_of_angle);

        float angle = acos(cos_of_angle);

        if (ref.dot(*this) < 0.0f) {
            //spdlog::info("angle is negative");
            angle = 2 * M_PI - angle;
        }

        return angle;
    }
};

class PLANE
{
public:
    double l, m, n, rho;
    PLANE()
        : l(0.0), m(0.0), n(0.0), rho(0.0)
    {
    }

    PLANE(PNT D, PNT S, int pointOnPlane = 1)
    {
        double distsquare = (S.x - D.x)*(S.x - D.x) + (S.y - D.y)*(S.y - D.y) + (S.z - D.z)*(S.z - D.z);
        if (distsquare <= 0)
        {
            l = 0; m = 0; n = 0;
        }
        else
        {
            double dist = sqrt(distsquare);
            l = (S.x - D.x) / dist;
            m = (S.y - D.y) / dist;
            n = (S.z - D.z) / dist;
        }
        if (pointOnPlane == 1)
            rho = l * D.x + m * D.y + n * D.z;
        else
            rho = l * S.x + m * S.y + n * S.z;
    }
    PLANE(double ll, double mm, double nn, double rrho, int calc = 0)
    {
        rho = rrho;
        if (calc == 0)
        {
            l = ll; m = mm; n = nn;
        }
        else
        {
            double d = sqrt(ll*ll + mm * mm + nn * nn);
            l = ll / d;  m = mm / d;  n = nn / d; rho = rrho;
        }
    }
    void set(double ll, double mm, double nn, double rrho)
    {
        l = ll; m = mm; n = nn; rho = rrho;
    }
    void SET(PNT D, PNT S, int pointOnPlane = 1)
    {
        double distsquare = (S.x - D.x)*(S.x - D.x) + (S.y - D.y)*(S.y - D.y) + (S.z - D.z)*(S.z - D.z);
        if (distsquare <= 0)
        {
            l = 0; m = 0; n = 0;
        }
        else
        {
            double dist = sqrt(distsquare);
            l = (S.x - D.x) / dist;
            m = (S.y - D.y) / dist;
            n = (S.z - D.z) / dist;
        }
        if (pointOnPlane == 1)
            rho = l * D.x + m * D.y + n * D.z;
        else
            rho = l * S.x + m * S.y + n * S.z;

    }
    void disp()
    {
        printf("\nl = %f; m = %f; n = %f; rho = %f", l, m, n, rho);
    }

};

