#include "variable.h"

std::vector<datas> datavec, dxfvec;
std::vector<contour> contourvec, pocketvec;
gcode_setup gc;
int linenumber;
std::vector<gcode_setup> gcvec;
std::vector<unsigned int> kpt_sequence; // Keep parts together list.
int maxdepth;

variable::variable()
{

}
