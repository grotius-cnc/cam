#include "gcode.h"

gcode::gcode()
{

}

void gcode::generate(){

    linenumber=0;
    linenumber_format=gc.linenumber_format;
    bool pr=gc.print_linenumbers;

    if(gc.intro.size()==0){ // If user has no input given, we add a linuxcnc gcode init.
        gc.intro.push_back("(Intro)\n");
        gc.intro.push_back("G21 (unit:mm)\n");
        gc.intro.push_back("G40 (cutter compensation off)\n");
        gc.intro.push_back("G80 (cancel canned cycle modal motion)\n");
        gc.intro.push_back("G90  (absolute distance, no offsets)\n");
        gc.intro.push_back("G64P0.01 (path following accuracy)\n");
        gc.intro.push_back("F1 (initialize feedrate)\n");
        gc.intro.push_back("S1 (initialize spindle speed/power)\n");
    }
    if(gc.outtro.size()==0){
        gc.outtro.push_back("M30\n");
    }

    int maxdepth=get_max_depth();
    std::string line_nr;

    std::ofstream myfile;
    myfile.open(gc.filename, std::ios::out); //ios app = append true
    myfile.precision(3);
    myfile<<std::fixed;
    for(unsigned int i=0; i<gc.intro.size(); i++){
        myfile<<new_line_nr(pr)<<gc.intro.at(i);
    }


    for(int i=maxdepth; i>-1; i--){
        for(unsigned int j=0; j<contourvec.size(); j++){
            if(contourvec.at(j).depth==i){

                myfile<<new_line_nr(pr)<<"(Contour id:"<<j<<")"<<"\n";

                // Contour lead-in
                std::vector<gp_Pnt> pntvec=get_lead_in_points(j);
                myfile<<new_line_nr(pr)<<"G0 X"<<pntvec.front().X()<<" Y"<<pntvec.front().Y()<<" Z"<<gc.travelheight<<"\n";
                myfile<<new_line_nr(pr)<<"G0 Z"<<gc.pierceheight<<"\n";
                myfile<<new_line_nr(pr)<<"M3 S"<<gc.power<<"\n";
                myfile<<new_line_nr(pr)<<"G1 Z"<<gc.cutheight<<" F"<<gc.piercespeed<<"\n";
                myfile<<new_line_nr(pr)<<"G1 X"<<pntvec.back().X()<<" Y"<<pntvec.back().Y()<<" F"<<gc.feedrate<<"\n";

                // Generate contour gcode
                    for(unsigned int k=0; k<contourvec.at(j).offset_sequence.size(); k++){
                        // [G1]
                        if(contourvec.at(j).offset_sequence.at(k).primitivetype==primitive_type::line){
                            myfile<<new_line_nr(pr)<<"G1 X"<<contourvec.at(j).offset_sequence.at(k).end.X()
                                 <<" Y"<<contourvec.at(j).offset_sequence.at(k).end.Y()
                                <<" Z"<<contourvec.at(j).offset_sequence.at(k).end.Z()
                               <<" F"<<gc.feedrate
                              <<"\n";
                        }
                        // [G2] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                        if(contourvec.at(j).offset_sequence.at(k).primitivetype==primitive_type::arc && contourvec.at(j).offset_sequence.at(k).bulge<0){
                            myfile<<new_line_nr(pr)<<"G2 X"<<contourvec.at(j).offset_sequence.at(k).end.X()
                                 <<" Y"<<contourvec.at(j).offset_sequence.at(k).end.Y()
                                <<" Z"<<contourvec.at(j).offset_sequence.at(k).end.Z()
                               <<" I"<<contourvec.at(j).offset_sequence.at(k).center.X()-contourvec.at(j).offset_sequence.at(k).start.X()
                              <<" J"<<contourvec.at(j).offset_sequence.at(k).center.Y()-contourvec.at(j).offset_sequence.at(k).start.Y()
                             <<" F"<<gc.feedrate
                            <<"\n";
                        }
                        // [G3] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                        if(contourvec.at(j).offset_sequence.at(k).primitivetype==primitive_type::arc && contourvec.at(j).offset_sequence.at(k).bulge>0){
                            myfile<<new_line_nr(pr)<<"G3 X"<<contourvec.at(j).offset_sequence.at(k).end.X()
                                 <<" Y"<<contourvec.at(j).offset_sequence.at(k).end.Y()
                                <<" Z"<<contourvec.at(j).offset_sequence.at(k).end.Z()
                               <<" I"<<contourvec.at(j).offset_sequence.at(k).center.X()-contourvec.at(j).offset_sequence.at(k).start.X()
                              <<" J"<<contourvec.at(j).offset_sequence.at(k).center.Y()-contourvec.at(j).offset_sequence.at(k).start.Y()
                             <<" F"<<gc.feedrate
                            <<"\n";
                        }
                    }

                // Contour lead-out
                pntvec=get_lead_out_points(j);
                myfile<<new_line_nr(pr)<<"G1 X"<<pntvec.front().X()<<" Y"<<pntvec.front().Y()<<"\n";
                myfile<<new_line_nr(pr)<<"M5"<<"\n";
                myfile<<new_line_nr(pr)<<"G0 Z"<<gc.travelheight<<"\n";
            }
        }
    }

    for(unsigned int i=0; i<gc.outtro.size(); i++){
        myfile<<new_line_nr(pr)<<line_nr<<gc.outtro.at(i);
    }
    myfile.close();
}

unsigned int gcode::increment_line_nr(){
    return linenumber++;
}

std::string gcode::new_line_nr(bool print){
    std::string str;
    if(print){
        str+=linenumber_format;
        str+=std::to_string(increment_line_nr());
        str+=" ";
    };
    return str;
}

std::vector<gp_Pnt> gcode::get_lead_in_points(unsigned int i /* contourvec[i]*/){
    std::vector<gp_Pnt> pntvec;
    const TopoDS_Edge& edge = TopoDS::Edge(contourvec.at(i).lead_in.ashape->Shape());
    TopoDS_Vertex v1,v2;
    TopExp::Vertices(edge,v1,v2);
    gp_Pnt p1= BRep_Tool::Pnt(v1);
    gp_Pnt p2= BRep_Tool::Pnt(v2);
    pntvec.push_back(p1);
    pntvec.push_back(p2);
    // std::cout<<"edge p1 x: "<<p1.X()<<" y:"<<p1.Y()<<" z:"<<p1.Z()<<std::endl;
    // std::cout<<"edge p2 x: "<<p2.X()<<" y:"<<p2.Y()<<" z:"<<p2.Z()<<std::endl;
    return pntvec;
}

std::vector<gp_Pnt> gcode::get_lead_out_points(unsigned int i /* contourvec[i]*/){
    std::vector<gp_Pnt> pntvec;
    const TopoDS_Edge& edge = TopoDS::Edge(contourvec.at(i).lead_out.ashape->Shape());
    TopoDS_Vertex v1,v2;
    TopExp::Vertices(edge,v1,v2);
    gp_Pnt p1= BRep_Tool::Pnt(v1);
    gp_Pnt p2= BRep_Tool::Pnt(v2);
    pntvec.push_back(p1);
    pntvec.push_back(p2);
    // std::cout<<"edge p1 x: "<<p1.X()<<" y:"<<p1.Y()<<" z:"<<p1.Z()<<std::endl;
    // std::cout<<"edge p2 x: "<<p2.X()<<" y:"<<p2.Y()<<" z:"<<p2.Z()<<std::endl;
    return pntvec;
}

int gcode::get_max_depth(){

    // Find maxdepth value.
    int maxdepth=0;
    for(unsigned int i=0; i<contourvec.size(); i++){
        if(maxdepth<contourvec.at(i).depth){
            maxdepth=contourvec.at(i).depth;
        }
    }
    // std::cout<<"maxdepth:"<<maxdepth<<std::endl;
    return maxdepth;
}















