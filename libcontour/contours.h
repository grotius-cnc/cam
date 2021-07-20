#ifndef CONTOURS_H
#define CONTOURS_H

#include <variable.h>
#include "opencascade.h"
#include "draw_primitives.h"
using namespace occ;

class contours
{
public:
    contours();

    void main(double tol, std::string layer);

    //! Wich layer to cut?
    void init_primitives(std::string layer);

    //! Check if open contours are open or closed. For example a spline can be closed if startpoint = endpoint.
    //! tol = tollerance to classify a startpoint-endpoint match.
    void check_for_single_closed_contours(double tol);
    void check_for_multi_contours(double tol);
    void check_for_single_open_contours(double tol);

    //! Add contour depth squence. This is not a "keep parts together" algoritme. It cut's depth for depth.
    //! It algoritme is preferred to be used to avoid thermal distortion.
    void add_contour_depth_sequence();
    void add_contour_ccw();
    std::vector<unsigned int> get_childs(std::vector<unsigned int> list);
    std::vector<unsigned int> get_toplevel(std::vector<unsigned int> list);
    std::vector<unsigned int> lista_min_listb(std::vector<unsigned int> lista, std::vector<unsigned int> listb);

    //! Helper functions
    void area();
    void swap(unsigned int i);
    void swap_contour(unsigned int i /*contourvec.at(i)*/);
    int find_next(unsigned int i /*source*/, unsigned int &target /*target*/, double tol);

    int find_start_start_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);
    int find_start_end_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);
    int find_end_start_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);
    int find_end_end_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);

    void print_result();
    void print_depth_sequence();

    int point_in_polygon(std::vector<gp_Pnt> polygon, gp_Pnt point);

    //! Keep parts together algorimte, shortened "kpt".
    //! Return a organized list in cut seauence of contourvec.at[i]
    std::vector<unsigned int> keep_parts_together();

private:

};

#endif // CONTOURS_H
