#ifndef GCODE_H
#define GCODE_H

#include <iostream>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <variable.h>
#include "opencascade.h"
#include "draw_primitives.h"
#include <contours.h>

using namespace occ;

class gcode
{
public:
    gcode();
    void generate(); // Will produce a gcode file in the program build directory with a .ngc extension.

    //! Helper functions:
    std::vector<gp_Pnt> get_lead_in_points(unsigned int i /* contourvec[i]*/);
    std::vector<gp_Pnt> get_lead_out_points(unsigned int i /* contourvec[i]*/);
    unsigned int increment_line_nr();
    std::string new_line_nr(bool print); // If print=0 there are no line numbers printed in gcode output file.

private:
    unsigned int linenumber=0;
    std::string linenumber_format={};
};

#endif // GCODE_H
