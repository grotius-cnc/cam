#ifndef VARIABLE_H
#define VARIABLE_H

#include <gp_Pnt.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Compound.hxx>

enum primitive_type {
    point=0,
    line=1,
    lwpolyline=2, // Wire, linestrip
    circle=3,
    arc=4,
    spline=5,
    ellipse=6,
};

enum contour_type {
    none=0,
    single_open=1,
    single_closed=2,
    multi_open=3,
    multi_closed=4,
};

enum contour_dir {
    cw=0,
    ccw=1,
};

enum offset_action{
    offset_contour=0,
    lead_base_contour=1,
    lead_in_contour=2,
    lead_out_contour=3,
};

struct data {
    Handle(AIS_Shape) ashape;
    primitive_type primitivetype;
    contour_type contourtype=contour_type::none;
    //! Startpoint, endpoint
    gp_Pnt start{0,0,0}, end{0,0,0}, center{0,0,0}; // Center is used by cavalier functions.
    std::vector<gp_Pnt> control, arcmid; // Arcmid is used by cavalier function and is the point at half way arc circumfence.
    double radius=0; // Radius is used by cavalier functions.
    double bulge=0; // For arc's to define if the arc is [cw] or [ccw]. bulge<0=g2
    bool select=0; // Helper for contour algoritme.
};
extern std::vector<data> datavec;

struct lead_in_out {
    Handle(AIS_Shape) ashape; // The lead-in, lead-out shape (line, arc, etc).
    std::vector<gp_Pnt> points; // Choose one of the parallel Points to draw the lead-in, lead-out.
};

struct contour {
    std::vector<data> primitive_sequence;
    contour_dir dir;
    double area=0;

    lead_in_out lead_in, lead_out, lead_base;
    std::vector<data> offset_sequence; // Primitives in cw order.

    // Contour depth.
    std::vector<unsigned int> childcontours;
    bool select=false;
    int depth=0;
};
extern std::vector<contour> contourvec;

struct gcode_setup{
    std::string filename="gcode.ngc";
    std::string linenumber_format="N";
    bool print_linenumbers=true;
    double lead_in=5;
    double lead_out=1;
    double offset=2;
    double travelheight=10;
    double pierceheight=8;
    double piercespeed=500;
    double cutheight=0;
    double power=45;
    double feedrate=1400;

    std::vector<std::string> intro;
    std::vector<std::string> outtro;
};

class variable
{
public:
    variable();
};

#endif // VARIABLE_H

























