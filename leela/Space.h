#pragma once

#include "Class.h"

#define TO_ORIGIN_FROM_Y        401
#define AT_POINT                402

#define SPC_S_MODE  0
#define SPC_D_MODE  1

typedef enum
{
    Movement_Forward,
    Movement_Backward,

    Movement_RotateLeft,
    Movement_RotateRight,
    Movement_RotateUp,
    Movement_RotateDown,

    Movement_ShiftLeft,
    Movement_ShiftRight,
    Movement_ShiftDown,
    Movement_ShiftUp,

    Movement_LeftAlongSD,
    Movement_RightAlongSD,
    Movement_Shift
} MovementType;

typedef enum
{
    Projection_Isometric,
    Projection_Perspective,
} SpaceProjectionType;




class Space
{
public:
    Space() :
        DS_dist(1200)
    {
        // Initialise the origin and standard axis point
        O.set(0, 0, 0);
        X.set(100, 0, 0);
        Y.set(0, 100, 0);
        Z.set(0, 0, 100);

        // TODO - Initialise the Frame Points


        // TODO - Initialise the Frame Vectors

        // Initialise type of view
        eProjectionType = Projection_Perspective;
    }

    ~Space()
    {
    }
    
    void initFrame()
    {
        int l = int(sqrt((DS_dist*DS_dist) / 3.0));

        S.set(l, l, l);                       // set source point equidistant from x-y, y-z and x-z planes.
        D.set(0, 0, 0);                       // set destination point looking at origin
        DS.SET(D, S);
        PP.SET(D, S);
        R.set(0, 0, -600);                    // downward direction vector
        PNT a(-100, 100, 0);
        R = rotate(a, O, R, 35.264389);
        DR.SET(D, R);
        DL = crossProduct(DR, DS);
        L.x = D.x + 600 * DL.l; L.y = D.y + 600 * DL.m; L.z = D.z + 600 * DL.n;
    }

    void defaultFrame()
    {
        initFrame();
    }

    glm::vec3 getUpwardDirectionVector()
    {
        return glm::vec3(-DR.l, -DR.m, -DR.n);
    }

    glm::vec3 getSourcePoint()
    {
        return glm::vec3(S.x, S.y, S.z);
    }

    glm::vec3 getDirectionPoint()
    {
        return glm::vec3(D.x, D.y, D.z);
    }

    double rad(double z)
    {
        return (z*M_PI / 180);
    }

    double deg(double z)
    {
        return (z * 180 / M_PI);
    }

    double distance(PNT p1, PNT p2)
    {
        double distsquare = (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) + (p1.z - p2.z)*(p1.z - p2.z);
        double dist;
        if (distsquare <= 0)
            dist = 0;
        else
            dist = sqrt(distsquare);
        return (dist);
    }

    // Set the Frame in an absolute manner
    void setFrame(int type, PNT s, VECTOR direction, PNT ref_pt)
    {
        PNT n;
        VECTOR n_ref_pt;

        switch (type)
        {
        case TO_ORIGIN_FROM_Y:

            D.set(0, 0, 0);
            S.set(0, DS_dist, 0);
            DS.SET(D, S);
            PP.SET(D, S);
            R.set(0, 0, -100);
            DR.SET(D, R);
            DL = crossProduct(DR, DS);

            L = D.translated(100, DL);

            moveFrame(Movement_Backward, 1500);
            break;

        case AT_POINT:
            S = s;
            D = S.translated(DS_dist, direction);
            DS.SET(D, S);
            PP.SET(D, S);

            n = nearestPointOnLine(D, S, ref_pt);
            n_ref_pt.SET(n, ref_pt);
            R = D.translated(100, n_ref_pt);

            //VECTOR perpendicular = crossProduct(
            //    crossProduct(DS, VECTOR(D, ref_pt)),
            //    DS);
            //R = D.translated(100, perpendicular);

            DR.SET(D, R);            
            DL = crossProduct(DR, DS);

            L = D.translated(600, DL);

            break;
        }
    }

    void rotateFrameAboutD(double horizontal, double vertical)
    {
        // rotate horizontally, i.e. about DR 
        S = rotate(D, R, S, horizontal);
        DS.SET(D, S);
        DL = crossProduct(DR, DS);
        L.x = D.x + 600 * DL.l; L.y = D.y + 600 * DL.m; L.z = D.z + 600 * DL.n;

        PP.SET(D, S);

        // rotate vertically, i.e. about DL
        S = rotate(D, L, S, vertical);
        DS.SET(D, S);
        DR = crossProduct(DL, DR);
        R.x = D.x + 600 * DR.l; D.y = D.y + 600 * DR.m; D.z = D.z + 600 * DR.n;

        PP.SET(D, S);
    }

    void rotateFrame(PNT along, double horizontal, double vertical)
    {
        // rotate horizontally
        S = rotate(along, along.translated(100, DR), S, horizontal);
        D = rotate(along, along.translated(100, DR), D, horizontal);
        R = rotate(along, along.translated(100, DR), R, horizontal);
        L = rotate(along, along.translated(100, DR), L, horizontal);

        DS.SET(D, S);
        DR.SET(D, R);
        DL.SET(D, L);
        PP.SET(D, S);

        // rotate vertically
        // find the second point of the axis to rotate vertically
        S = rotate(along, along.translated(100, DL), S, vertical);
        D = rotate(along, along.translated(100, DL), D, vertical);
        R = rotate(along, along.translated(100, DL), R, vertical);
        L = rotate(along, along.translated(100, DL), L, vertical);

        DS.SET(D, S);
        DR.SET(D, R);
        DL.SET(D, L);
        PP.SET(D, S);
    }

    void setFrameMoveMode(int mode)
    {
        frameMoveMode = char(mode);
    }

    int getFrameMoveMode()
    {
        return frameMoveMode;
    }

    void setDefaultView()
    {
        initFrame();
    }

    /***************************************************************************
      Rotate point p by angle z about a vector representated by a-b.
     ***************************************************************************/
    PNT rotate(PNT A, PNT B, PNT p, double z)
    {
        z = rad(z);

        PNT N = nearestPointOnLine(A, B, p);
        VECTOR AB(A, B);
        VECTOR NP(N, p);
        VECTOR NL = crossProduct(AB, NP);

        // we are rotating from NP to NL. Find components of rotated point on vectors NP and NL.
        double dx = NP.d * cos(z);
        double dy = NP.d * sin(z);

        // Offset the nearest point N using directionals cosines of NP & NL and their respective
        // contributions to the rotated point.
        p.x = N.x + NP.l*dx + NL.l*dy;
        p.y = N.y + NP.m*dx + NL.m*dy;
        p.z = N.z + NP.n*dx + NL.n*dy;

        return p;
    }

    /*!
    ****************************************************************************

     Returns a point on a line (defined by A & B) that is nearest to the given
     point p outside the line.

     How was the formula derived?
        - First, form the parametric equation of line (using parameter k)
          going through segment AB.
             - if k = 0, point is at A. if k = 1, point is at B.
             - Any value of k is possible, i.e. not limited to range [0, 1].
        - Form equation for the distance between given point p and any arbitrary
          point on line going through AB.
        - Note that this distance will be minimum when the point on the line is
          the perpendicular point.
        - Take derivative of this equation. Set it to 0. Then solve for k.
          This will give you the value of k at which the distance is minimum.
        - Finally, find the point on the line using the value of k. That is the
          nearest point.

    ****************************************************************************/
    PNT nearestPointOnLine(PNT A, PNT B, PNT p)
    {
        PNT N;
        double k;

        double den = B.squaredDistanceTo(A);
        if (fabs(den) > 1e-6)
        {
            k = ((p.x - A.x)*(B.x - A.x) + (p.y - A.y)*(B.y - A.y) + (p.z - A.z)*(B.z - A.z)) / den;
        }
        else
        {
            k = .5;     // A & B are too close. declare mid-point of A & B as the nearest point.
        }

        // Calculate nearest point from its caluclated parameter k.
        N.x = A.x + (B.x - A.x) * k;
        N.y = A.y + (B.y - A.y) * k;
        N.z = A.z + (B.z - A.z) * k;

        return N;
    }

    // returns V1xV2
    VECTOR crossProduct(VECTOR V1, VECTOR V2)
    {
        VECTOR prod;
        prod.x = V1.y * V2.z - V1.z * V2.y;
        prod.y = -(V1.x * V2.z - V1.z * V2.x);
        prod.z = V1.x * V2.y - V1.y * V2.x;
        prod.calc();
        return (prod);
    }

    //returns V1.V2
    inline double dotProduct(VECTOR V1, VECTOR V2)
    {
        double prod = V1.x * V2.x + V1.y * V2.y + V1.z * V2.z;
        return (prod);
    }

    // Central function of the Space class.  This function moves the frame
    // according to the specified direction type & by the specified amount.
    void moveFrame(MovementType eDir, double increment, PNT p = PNT(0, 0, 0))
    {
        PNT Pnt;

        switch (eDir)
        {
        case Movement_Forward:
            S.translate(-increment, DS);
            D.translate(-increment, DS);
            R.translate(-increment, DS);
            L.translate(-increment, DS);
            PP.SET(D, S);
            break;

        case Movement_Backward:
            S.translate(increment, DS);
            D.translate(increment, DS);
            R.translate(increment, DS);
            L.translate(increment, DS);
            PP.SET(D, S);
            break;

        case Movement_RotateLeft:
            switch (frameMoveMode)
            {
            case SPC_S_MODE:
                Pnt.x = R.x + (S.x - D.x);  Pnt.y = R.y + (S.y - D.y);  Pnt.z = R.z + (S.z - D.z);
                D = rotate(Pnt, S, D, increment);
                L = rotate(Pnt, S, L, increment);
                R = rotate(Pnt, S, R, increment);
                break;
            case SPC_D_MODE:
                S = rotate(R, D, S, -increment);
                L = rotate(R, D, L, -increment);
                break;
            }
            DS.SET(D, S);
            DL.SET(D, L);
            DR.SET(D, R);
            PP.SET(D, S);
            break;

        case Movement_RotateRight:
            switch (frameMoveMode)
            {
            case SPC_S_MODE:
                Pnt.x = R.x + (S.x - D.x);  Pnt.y = R.y + (S.y - D.y);  Pnt.z = R.z + (S.z - D.z);
                D = rotate(Pnt, S, D, -increment);
                L = rotate(Pnt, S, L, -increment);
                R = rotate(Pnt, S, R, -increment);
                break;
            case SPC_D_MODE:
                S = rotate(R, D, S, increment);
                L = rotate(R, D, L, increment);
                break;
            }
            DS.SET(D, S);
            DL.SET(D, L);
            DR.SET(D, R);
            PP.SET(D, S);
            break;

        case Movement_RotateDown:
            switch (frameMoveMode)
            {
            case SPC_S_MODE:
                Pnt.x = L.x + (S.x - D.x);  Pnt.y = L.y + (S.y - D.y);  Pnt.z = L.z + (S.z - D.z);
                D = rotate(S, Pnt, D, increment);
                R = rotate(S, Pnt, R, increment);
                L = rotate(S, Pnt, L, increment);
                break;
            case SPC_D_MODE:
                Pnt.x = L.x + (S.x - D.x);  Pnt.y = L.y + (S.y - D.y);  Pnt.z = L.z + (S.z - D.z);
                S = rotate(L, D, S, increment);
                R = rotate(L, D, R, increment);
                break;
            }
            DS.SET(D, S);
            DR.SET(D, R);
            DL.SET(D, L);
            PP.SET(D, S);
            break;

        case Movement_RotateUp:
            switch (frameMoveMode)
            {
            case SPC_S_MODE:
                Pnt.x = L.x + (S.x - D.x);  Pnt.y = L.y + (S.y - D.y);  Pnt.z = L.z + (S.z - D.z);
                D = rotate(S, Pnt, D, -increment);
                R = rotate(S, Pnt, R, -increment);
                L = rotate(S, Pnt, L, -increment);
                break;
            case SPC_D_MODE:
                Pnt.x = L.x + (S.x - D.x);  Pnt.y = L.y + (S.y - D.y);  Pnt.z = L.z + (S.z - D.z);
                S = rotate(L, D, S, -increment);
                R = rotate(L, D, R, -increment);
                break;
            }
            DS.SET(D, S);
            DR.SET(D, R);
            DL.SET(D, L);
            PP.SET(D, S);
            break;

        case Movement_RightAlongSD:
            R = rotate(S, D, R, increment);
            L = rotate(S, D, L, increment);
            DR.SET(D, R);
            DL.SET(D, L);
            break;

        case Movement_LeftAlongSD:
            R = rotate(S, D, R, -increment);
            L = rotate(S, D, L, -increment);
            DR.SET(D, R);
            DL.SET(D, L);
            break;

        case Movement_ShiftLeft:
            moveFrame(Movement_RotateLeft, 90);
            moveFrame(Movement_Forward, increment);
            moveFrame(Movement_RotateRight, 90);
            break;

        case Movement_ShiftRight:
            moveFrame(Movement_RotateRight, 90);
            moveFrame(Movement_Forward, increment);
            moveFrame(Movement_RotateLeft, 90);
            break;

        case Movement_ShiftDown:
            moveFrame(Movement_RotateDown, 90);
            moveFrame(Movement_Forward, increment);
            moveFrame(Movement_RotateUp, 90);
            break;

        case Movement_ShiftUp:
            moveFrame(Movement_RotateUp, 90);
            moveFrame(Movement_Forward, increment);
            moveFrame(Movement_RotateDown, 90);
            break;

        case Movement_Shift:
            //            D.set(D.x+p.x, D.y+p.y, D.z+p.z);
            //            S.set(S.x+p.x, S.y+p.y, S.z+p.z);
            //            R.set(R.x+p.x, R.y+p.y, R.z+p.z);
            //            L.set(L.x+p.x, L.y+p.y, L.z+p.z);
            D += p;
            S += p;
            R += p;
            L += p;

            DS.SET(D, S);
            DL.SET(D, L);
            DR.SET(D, R);
            PP.SET(D, S);
            break;
        }
    }

    void pushFrame()
    {
        OLD_S = S;
        OLD_D = D;
        OLD_R = R;
        OLD_L = L;
    }

    void popFrame()
    {
        S = OLD_S;
        D = OLD_D;
        R = OLD_R;
        L = OLD_L;

        DS.SET(D, S);
        DR.SET(D, R);
        DL.SET(D, L);
        PP.SET(D, S);
    }

    void swapFrame()
    {
        PNT TS, TD, TR, TL;

        // save the current frame to a temporary frame
        TS = S;
        TD = D;
        TR = R;
        TL = L;

        // copy old frame to current frame
        popFrame();

        // copy temporary frame to old frame
        OLD_S = TS;
        OLD_D = TD;
        OLD_R = TR;
        OLD_L = TL;
    }

public:

    // The Origin and Standard Axis points
    PNT O, X, Y, Z;
    // The 3D Frame
    PNT S, D, R, L;
    PNT OLD_S, OLD_D, OLD_R, OLD_L;
    // Frame Vectors
    VECTOR DS, DR, DL;
    // The Picture Plane
    PLANE PP;
    // The Parallel Picture Plane
    PLANE PPP;

    int DS_dist;        // distance between D and S reference points

    // screen coordinates used in SDL_SetVideoMode
    int screen_width, screen_height;

    double screen_scale_factor;
    double time_scale_factor;
    // maximum x and y co-ordinates
    //    int maxx, maxy, cx, cy;

    // Type of view
    SpaceProjectionType eProjectionType;
    // frame Move Mode
    char frameMoveMode;

    // Page number of the screen in VGAMED mode
    unsigned PageNo;

};