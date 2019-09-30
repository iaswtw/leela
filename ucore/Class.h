#pragma once


// GLM includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    PT() { c = -1; }
    PT(double xx, double yy, int cc = -1)
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
    PNT() { c = -1; }
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
    void translate(double amount, VECTOR& direction);
    PNT  translated(double amount, VECTOR& direction);
    PNT translatedX(double amount);
    PNT translatedY(double amount);
    PNT translatedZ(double amount);
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
    double x, y, z, l, m, n, d;
    VECTOR() { }
    VECTOR(double xx, double yy, double zz)
    {
        x = xx; y = yy; z = zz; calc();
    }
    VECTOR(PNT P1, PNT P2)
    {
        x = P2.x - P1.x; y = P2.y - P1.y; z = P2.z - P1.z; calc();
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

    PNT PointAt(double k) {
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
};

class PLANE
{
public:
    double l, m, n, rho;
    PLANE() { }
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


