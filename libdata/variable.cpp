#include "variable.h"

std::vector<datas> datavec, dxfvec, rapidvec;
std::vector<contour> contourvec, pocketvec;
gcode_setup gc;
double last_x=0,last_y=0,last_z=0;
int linenumber;
std::vector<gcode_setup> gcvec;
std::vector<unsigned int> kpt_sequence; // Keep parts together list.
int maxdepth;

variable::variable()
{

}
