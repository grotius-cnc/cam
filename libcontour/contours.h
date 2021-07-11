#ifndef CONTOURS_H
#define CONTOURS_H

#include "opencascade.h"
#include "draw_primitives.h"
using namespace occ;

class contours
{
public:
    contours();

    void main(double tol);

    //! Check if open contours are open or closed. For example a spline can be closed if startpoint = endpoint.
    //! tol = tollerance to classify a startpoint-endpoint match.
    void init_primitives();
    void check_for_single_closed_contours(double tol);
    void check_for_multi_contours(double tol);
    void check_for_single_open_contours(double tol);

    //! Helper functions
    void swap(unsigned int i);
    int find_next(unsigned int i /*source*/, unsigned int &target /*target*/, double tol);

    int find_start_start_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);
    int find_start_end_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);
    int find_end_start_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);
    int find_end_end_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol);

    void print_result();

private:

};

#endif // CONTOURS_H
