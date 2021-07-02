#ifndef DATABUCKET_H
#define DATABUCKET_H

#include <iostream>
#include <vector>

struct point {
    double x=0,y=0,z=0;
};
extern point p;
extern std::vector<point> pointvec;

struct line {
    point start;
    point end;
};
extern line l;
extern std::vector<line> linevec;

struct arc {
    point center;
    double radius=0;
    double alpha1=0, alpha2=0;
    // Calculated after loading primitives in databucket.cpp class:
    point start;
    point end;
    double determinant; // Tells you if the arc is cw or ccw
    // To add bulge info for cavaliercontours
};
extern arc a;
extern std::vector<arc> arcvec;

struct circle {
    point center;
    double radius=0;
};
extern circle c;
extern std::vector<circle> circlevec;

struct ellipse {
    point center;
    point mayor;
    double ratio;
    double alpha1=0, alpha2=0;
};
extern ellipse e;
extern std::vector<ellipse> ellipsevec;

struct spline {
    std::vector<point> points;
};
extern spline s;
extern std::vector<spline> splinevec;

class databucket
{
public:
    databucket();

    void clear();
    void print_databucket_content();

};

#endif // DATABUCKET_H












