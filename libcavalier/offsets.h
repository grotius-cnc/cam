#ifndef OFFSETS_H
#define OFFSETS_H

#include <variable.h>

struct POINT{ // To be replaced by gp_Pnt.
    double x,y,z;
};

class offsets
{
public:
    offsets();

    //! Example of standard offset use -abs value for negative offset.
    void do_offset(double offset, enum offset_action action, double lead_in, double lead_out);
    void rotate_primairy_contour(unsigned int i /* contourvec[i] */);
    void create_lead_in_out(double lead_in, double lead_out);

    //! Helper functions, grabbed from old code example :
    std::vector<double> arc_bulge(datas p /* primitive */);
    double arc_determinant(POINT a /* arc startpoint */, POINT b /* a arc circumfence point*/, POINT c /* arc endpoint*/);
    POINT arc_center(POINT a /* arc startpoint */, POINT b /* arc controlpoint*/, POINT c /* arc endpoint */);
    POINT offset_point_on_line(double xs, double ys, double xe, double ye, double offset_from_xs_ys);
    POINT bulge_to_arc_controlpoint(POINT p1, POINT p2, double bulge); //find the arc center
    POINT rotate_3d(double x_to_rotate,double y_to_rotate, double z_to_rotate, double rotate_degrees_x, double rotate_degrees_y, double rotate_degrees_z);

    //! Create pockets, with or without islands.
    void do_pockets(double offset_contour /* offset from contour base */, double pocket_overlap /* pocket overlap 0-100% */, offset_action action, double lead_in, double lead_out);
};


#endif // OFFSETS_H
