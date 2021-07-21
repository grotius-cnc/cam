#ifndef OFFSETS_H
#define OFFSETS_H

#include <variable.h>

struct POINTS{ // To be replaced by gp_Pnt.
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
    double arc_determinant(POINTS a /* arc startpoint */, POINTS b /* a arc circumfence point*/, POINTS c /* arc endpoint*/);
    POINTS arc_center(POINTS a /* arc startpoint */, POINTS b /* arc controlpoint*/, POINTS c /* arc endpoint */);
    POINTS offset_point_on_line(double xs, double ys, double xe, double ye, double offset_from_xs_ys);
    POINTS bulge_to_arc_controlpoint(POINTS p1, POINTS p2, double bulge); //find the arc center
    POINTS rotate_3d(double x_to_rotate,double y_to_rotate, double z_to_rotate, double rotate_degrees_x, double rotate_degrees_y, double rotate_degrees_z);
    void swap_contour(unsigned int i /*contourvec.at(i)*/); // A copy function from contour class.

    //! Create pockets, with or without islands.
    void do_pocket();
    //void process_pocket(cavc::Polyline<double> outerloop, std::vector<cavc::Polyline<double>> islands, unsigned int contourvec_i);
    //void draw_pocket(cavc::OffsetLoopSet<double> results, unsigned int contourvec_i);

};


#endif // OFFSETS_H
