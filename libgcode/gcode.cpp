#include "gcode.h"

gcode::gcode()
{

}

void gcode::clear(){
    std::ofstream myfile;
    myfile.open(gc.filename, std::ios::out); //ios::app = append true, ios::out = new
    myfile<<"";
    myfile.close();
}

void gcode::generate(){

    // When depth sequence is calculated directly after the contour recognition, we afterwards group the contours into cut sets (inner+outer contour = one part).
    // std::vector<unsigned int> kpt_secuence=contours().keep_parts_together();

    linenumber_format=gc.linenumber_format;
    bool pr=gc.print_linenumbers;

    std::string line_nr;

    std::ofstream myfile;
    myfile.open(gc.filename, std::ios::app); //ios::app = append true, ios::out = new
    myfile.precision(3);
    myfile<<std::fixed;

    if(gc.operation_type=="intro"){
        for(unsigned int i=0; i<gc.intro.size(); i++){
            myfile<<new_line_nr(pr)<<gc.intro.at(i);
        }
    }

    if(gc.operation_type=="pocket"){
        for(unsigned int i=0; i<pocketvec.size(); i++){
            for(unsigned int j=0; j<pocketvec.at(i).offset_sequence.size(); j++){
                if(j==0){
                    myfile<<new_line_nr(pr)<<"G0 X"<<pocketvec.at(i).offset_sequence.front().start.X()+gc.tooloffset_x
                         <<" Y"<<pocketvec.at(i).offset_sequence.front().start.Y()+gc.tooloffset_y
                        <<" "<<gc.z_axis_format<<gc.travelheight+gc.tooloffset_z
                       <<"\n";
                    myfile<<new_line_nr(pr)<<"G0 "<<gc.z_axis_format<<gc.pierceheight+gc.tooloffset_z<<"\n";
                    myfile<<new_line_nr(pr)<<gc.tool_on_macro<<" S"<<gc.power<<"\n";
                    if(gc.piercedelay>0){
                        myfile<<new_line_nr(pr)<<"G4 P"<<gc.piercedelay<<"\n";
                    }
                    myfile<<new_line_nr(pr)<<"G1 "<<gc.z_axis_format<<gc.cutheight+gc.tooloffset_z<<" F"<<gc.piercespeed<<"\n";
                }
                // [G1]
                if(pocketvec.at(i).offset_sequence.at(j).primitivetype==primitive_type::line){
                    myfile<<new_line_nr(pr)<<"G1 X"<<pocketvec.at(i).offset_sequence.at(j).end.X()+gc.tooloffset_x
                         <<" Y"<<pocketvec.at(i).offset_sequence.at(j).end.Y()+gc.tooloffset_y
                        <<" "<<gc.z_axis_format<<pocketvec.at(i).offset_sequence.at(j).end.Z()+gc.tooloffset_z
                       <<" F"<<gc.feedrate
                      <<"\n";
                }
                // [G2] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                if(pocketvec.at(i).offset_sequence.at(j).primitivetype==primitive_type::arc && pocketvec.at(i).offset_sequence.at(j).bulge<0){
                    myfile<<new_line_nr(pr)<<"G2 X"<<pocketvec.at(i).offset_sequence.at(j).end.X()+gc.tooloffset_x
                         <<" Y"<<pocketvec.at(i).offset_sequence.at(j).end.Y()+gc.tooloffset_y
                        <<" "<<gc.z_axis_format<<pocketvec.at(i).offset_sequence.at(j).end.Z()+gc.tooloffset_z
                       <<" I"<<(pocketvec.at(i).offset_sequence.at(j).center.X()+gc.tooloffset_x)-(pocketvec.at(i).offset_sequence.at(j).start.X()+gc.tooloffset_x)
                      <<" J"<<(pocketvec.at(i).offset_sequence.at(j).center.Y()+gc.tooloffset_y)-(pocketvec.at(i).offset_sequence.at(j).start.Y()+gc.tooloffset_y)
                     <<" F"<<gc.feedrate
                    <<"\n";
                }
                // [G3] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                if(pocketvec.at(i).offset_sequence.at(j).primitivetype==primitive_type::arc && pocketvec.at(i).offset_sequence.at(j).bulge>0){
                    myfile<<new_line_nr(pr)<<"G3 X"<<pocketvec.at(i).offset_sequence.at(j).end.X()+gc.tooloffset_x
                         <<" Y"<<pocketvec.at(i).offset_sequence.at(j).end.Y()+gc.tooloffset_y
                        <<" "<<gc.z_axis_format<<pocketvec.at(i).offset_sequence.at(j).end.Z()+gc.tooloffset_z
                       <<" I"<<(pocketvec.at(i).offset_sequence.at(j).center.X()+gc.tooloffset_x)-(pocketvec.at(i).offset_sequence.at(j).start.X()+gc.tooloffset_x)
                      <<" J"<<(pocketvec.at(i).offset_sequence.at(j).center.Y()+gc.tooloffset_y)-(pocketvec.at(i).offset_sequence.at(j).start.Y()+gc.tooloffset_y)
                     <<" F"<<gc.feedrate
                    <<"\n";
                }
                if(j==pocketvec.at(i).offset_sequence.size()-1){
                    myfile<<new_line_nr(pr)<<"M5"<<"\n";
                    myfile<<new_line_nr(pr)<<"G0 "<<gc.z_axis_format<<gc.travelheight+gc.tooloffset_z<<"\n";
                }
            }
        }
    }

    //! This is output is cutting following the depth sequence. This is not a keep parts together output.
    if(gc.operation_type=="contour"){
        for(unsigned int i=0; i<kpt_sequence.size(); i++){
            int j=kpt_sequence.at(i);

            myfile<<new_line_nr(pr)<<"(Contour id:"<<j<<")"<<"\n";

            // Contour lead-in. Only when contourtypes are closed.
            if(contourvec.at(j).primitive_sequence.front().contourtype==contour_type::single_closed || contourvec.at(j).primitive_sequence.front().contourtype==contour_type::multi_closed){
                std::vector<gp_Pnt> pntvec=get_lead_in_points(j);
                myfile<<new_line_nr(pr)<<"G0 X"<<pntvec.front().X()+gc.tooloffset_x<<" Y"<<pntvec.front().Y()+gc.tooloffset_y<<" "<<gc.z_axis_format<<gc.travelheight+gc.tooloffset_z<<"\n";
                myfile<<new_line_nr(pr)<<"G0 "<<gc.z_axis_format<<gc.pierceheight+gc.tooloffset_z<<"\n";
                myfile<<new_line_nr(pr)<<gc.tool_on_macro<<" S"<<gc.power<<"\n";
                if(gc.piercedelay>0){
                    myfile<<new_line_nr(pr)<<"G4 P"<<gc.piercedelay<<"\n";
                }
                myfile<<new_line_nr(pr)<<"G1 "<<gc.z_axis_format<<gc.cutheight+gc.tooloffset_z<<" F"<<gc.piercespeed<<"\n";
                myfile<<new_line_nr(pr)<<"G1 X"<<pntvec.back().X()+gc.tooloffset_x<<" Y"<<pntvec.back().Y()+gc.tooloffset_y<<" F"<<gc.feedrate<<"\n";
            }
            // Contour lead-in. Only when contourtypes are open.
            if(contourvec.at(j).primitive_sequence.front().contourtype==contour_type::single_open || contourvec.at(j).primitive_sequence.front().contourtype==contour_type::multi_open){
                myfile<<new_line_nr(pr)<<"G0 X"<<contourvec.at(j).primitive_sequence.front().start.X()+gc.tooloffset_x<<" Y"<<contourvec.at(j).primitive_sequence.front().start.Y()+gc.tooloffset_y<<" Z"<<gc.travelheight+gc.tooloffset_z<<"\n";
                myfile<<new_line_nr(pr)<<"G0 "<<gc.z_axis_format<<gc.pierceheight+gc.tooloffset_z<<"\n";
                myfile<<new_line_nr(pr)<<gc.tool_on_macro<<" S"<<gc.power<<"\n";
                myfile<<new_line_nr(pr)<<"G1 "<<gc.z_axis_format<<gc.cutheight+gc.tooloffset_z<<" F"<<gc.piercespeed<<"\n";
            }

            // Generate contour gcode for closed contours.
            if(contourvec.at(j).primitive_sequence.front().contourtype==contour_type::single_closed || contourvec.at(j).primitive_sequence.front().contourtype==contour_type::multi_closed){
                for(unsigned int k=0; k<contourvec.at(j).offset_sequence.size(); k++){
                    // [G1]
                    if(contourvec.at(j).offset_sequence.at(k).primitivetype==primitive_type::line){
                        myfile<<new_line_nr(pr)<<"G1 X"<<contourvec.at(j).offset_sequence.at(k).end.X()+gc.tooloffset_x
                             <<" Y"<<contourvec.at(j).offset_sequence.at(k).end.Y()+gc.tooloffset_y
                            <<" "<<gc.z_axis_format<<contourvec.at(j).offset_sequence.at(k).end.Z()+gc.tooloffset_z
                           <<" F"<<gc.feedrate
                          <<"\n";
                    }
                    // [G2] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                    if(contourvec.at(j).offset_sequence.at(k).primitivetype==primitive_type::arc && contourvec.at(j).offset_sequence.at(k).bulge<0){
                        myfile<<new_line_nr(pr)<<"G2 X"<<contourvec.at(j).offset_sequence.at(k).end.X()+gc.tooloffset_x
                             <<" Y"<<contourvec.at(j).offset_sequence.at(k).end.Y()+gc.tooloffset_y
                            <<" "<<gc.z_axis_format<<contourvec.at(j).offset_sequence.at(k).end.Z()+gc.tooloffset_z
                           <<" I"<<(contourvec.at(j).offset_sequence.at(k).center.X()+gc.tooloffset_x)-(contourvec.at(j).offset_sequence.at(k).start.X()+gc.tooloffset_x)
                          <<" J"<<(contourvec.at(j).offset_sequence.at(k).center.Y()+gc.tooloffset_y)-(contourvec.at(j).offset_sequence.at(k).start.Y()+gc.tooloffset_y)
                         <<" F"<<gc.feedrate
                        <<"\n";
                    }
                    // [G3] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                    if(contourvec.at(j).offset_sequence.at(k).primitivetype==primitive_type::arc && contourvec.at(j).offset_sequence.at(k).bulge>0){
                        myfile<<new_line_nr(pr)<<"G3 X"<<contourvec.at(j).offset_sequence.at(k).end.X()+gc.tooloffset_x
                             <<" Y"<<contourvec.at(j).offset_sequence.at(k).end.Y()+gc.tooloffset_y
                            <<" "<<gc.z_axis_format<<contourvec.at(j).offset_sequence.at(k).end.Z()+gc.tooloffset_z
                           <<" I"<<(contourvec.at(j).offset_sequence.at(k).center.X()+gc.tooloffset_x)-(contourvec.at(j).offset_sequence.at(k).start.X()+gc.tooloffset_x)
                          <<" J"<<(contourvec.at(j).offset_sequence.at(k).center.Y()+gc.tooloffset_y)-(contourvec.at(j).offset_sequence.at(k).start.Y()+gc.tooloffset_y)
                         <<" F"<<gc.feedrate
                        <<"\n";
                    }
                }
            }

            // Generate contour gcode for open contours. (no offset is applied)
            if(contourvec.at(j).primitive_sequence.front().contourtype==contour_type::single_open || contourvec.at(j).primitive_sequence.front().contourtype==contour_type::multi_open){
                for(unsigned int k=0; k<contourvec.at(j).primitive_sequence.size(); k++){
                    // [G1]
                    if(contourvec.at(j).primitive_sequence.at(k).primitivetype==primitive_type::line){
                        myfile<<new_line_nr(pr)<<"G1 X"<<contourvec.at(j).primitive_sequence.at(k).end.X()+gc.tooloffset_x
                             <<" Y"<<contourvec.at(j).primitive_sequence.at(k).end.Y()+gc.tooloffset_y
                            <<" "<<gc.z_axis_format<<contourvec.at(j).primitive_sequence.at(k).end.Z()+gc.tooloffset_z
                           <<" F"<<gc.feedrate
                          <<"\n";
                    }
                    // [G2] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                    if(contourvec.at(j).primitive_sequence.at(k).primitivetype==primitive_type::arc && contourvec.at(j).primitive_sequence.at(k).bulge<0){
                        myfile<<new_line_nr(pr)<<"G2 X"<<contourvec.at(j).primitive_sequence.at(k).end.X()+gc.tooloffset_x
                             <<" Y"<<contourvec.at(j).primitive_sequence.at(k).end.Y()+gc.tooloffset_y
                            <<" "<<gc.z_axis_format<<contourvec.at(j).primitive_sequence.at(k).end.Z()+gc.tooloffset_z
                           <<" I"<<(contourvec.at(j).primitive_sequence.at(k).center.X()+gc.tooloffset_x)-(contourvec.at(j).primitive_sequence.at(k).start.X()+gc.tooloffset_x)
                          <<" J"<<(contourvec.at(j).primitive_sequence.at(k).center.Y()+gc.tooloffset_y)-(contourvec.at(j).primitive_sequence.at(k).start.Y()+gc.tooloffset_y)
                         <<" F"<<gc.feedrate
                        <<"\n";
                    }
                    // [G3] I=offset xcenter-xstart, J=offset ycenter-ystart, G2=clockwise (cw), G3=counterclockwise (ccw)
                    if(contourvec.at(j).primitive_sequence.at(k).primitivetype==primitive_type::arc && contourvec.at(j).primitive_sequence.at(k).bulge>0){
                        myfile<<new_line_nr(pr)<<"G3 X"<<contourvec.at(j).primitive_sequence.at(k).end.X()+gc.tooloffset_x
                             <<" Y"<<contourvec.at(j).primitive_sequence.at(k).end.Y()+gc.tooloffset_y
                            <<" "<<gc.z_axis_format<<contourvec.at(j).primitive_sequence.at(k).end.Z()+gc.tooloffset_z
                           <<" I"<<(contourvec.at(j).primitive_sequence.at(k).center.X()+gc.tooloffset_x)-(contourvec.at(j).primitive_sequence.at(k).start.X()+gc.tooloffset_x)
                          <<" J"<<(contourvec.at(j).primitive_sequence.at(k).center.Y()+gc.tooloffset_y)-(contourvec.at(j).primitive_sequence.at(k).start.Y()+gc.tooloffset_y)
                         <<" F"<<gc.feedrate
                        <<"\n";
                    }
                }
            }

            // Contour lead-out, Only when contourtypes are closed.
            if(contourvec.at(j).primitive_sequence.front().contourtype==contour_type::single_closed || contourvec.at(j).primitive_sequence.front().contourtype==contour_type::multi_closed){
                std::vector<gp_Pnt> pntvec=get_lead_out_points(j);
                myfile<<new_line_nr(pr)<<"G1 X"<<pntvec.front().X()+gc.tooloffset_x<<" Y"<<pntvec.front().Y()+gc.tooloffset_y<<"\n";
                myfile<<new_line_nr(pr)<<"M5"<<"\n";
                myfile<<new_line_nr(pr)<<"G0 "<<gc.z_axis_format<<gc.travelheight+gc.tooloffset_z<<"\n";
            }
            // Contour lead-out. Only when contourtypes are open.
            if(contourvec.at(j).primitive_sequence.front().contourtype==contour_type::single_open || contourvec.at(j).primitive_sequence.front().contourtype==contour_type::multi_open){
                myfile<<new_line_nr(pr)<<"G1 X"<<contourvec.at(j).primitive_sequence.back().end.X()+gc.tooloffset_x<<" Y"<<contourvec.at(j).primitive_sequence.back().end.Y()+gc.tooloffset_y<<"\n";
                myfile<<new_line_nr(pr)<<"M5"<<"\n";
                myfile<<new_line_nr(pr)<<"G0 "<<gc.z_axis_format<<gc.travelheight+gc.tooloffset_z<<"\n";
            }
        }
    }

    if(gc.operation_type=="outtro"){
        for(unsigned int i=0; i<gc.outtro.size(); i++){
            myfile<<new_line_nr(pr)<<line_nr<<gc.outtro.at(i);
        }
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

//! This output is cutting following the depth sequence. This is not a keep parts together output.
/*
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
*/












