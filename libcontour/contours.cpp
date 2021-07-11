#include "contours.h"

/* The datavec from opencascade.h is used here.
 *
 * 1. Check open contour types if they are indeed open. "contourtype::open"
 *
 *
 * */

contours::contours()
{

}

void contours::main(double tol){
    contourvec.clear();
    init_primitives();
    check_for_single_closed_contours(tol);
    check_for_multi_contours(tol);
    check_for_single_open_contours(tol);
    print_result();
}

void contours::init_primitives(){

    std::vector<data>::iterator it;
    for(it=datavec.begin(); it<datavec.end(); it++){
        it->select=false;
    }
}

void contours::check_for_single_closed_contours(double tol){

    // Find primitives that are closed contours:
    int match=0;
    for(unsigned int i=0; i<datavec.size(); i++){
        match=find_start_end_match(i,i,tol);
        if(match){
            datavec.at(i).contourtype=contour_type::single_closed;
            datavec.at(i).select=true;
            contour c;
            c.primitive_sequence.push_back(datavec.at(i));
            contourvec.push_back(c); // contourvec is related to contournumber
        }
    }
}

void contours::check_for_multi_contours(double tol){

    int startmatch=0, endmatch=0;
    for(unsigned int i=0; i<datavec.size(); i++){

        for(unsigned int j=0; j<datavec.size(); j++){
            startmatch=0; endmatch=0;
            if(i!=j && datavec.at(i).select==0 && datavec.at(j).select==0){ // Don't compare the same primitive or previous selected primitives.

                startmatch+=find_start_start_match(i,j,tol);
                startmatch+=find_start_end_match(i,j,tol);
                endmatch+=find_end_start_match(i,j,tol);
                endmatch+=find_end_end_match(i,j,tol);

                 std::cout<<"startmatch value:"<<startmatch<<std::endl;
                 std::cout<<"endmatch value:"<<endmatch<<std::endl;

                contour c;

                if(startmatch==1 && endmatch==1){ // This can be a dual closed contour. Found out by testing.
                    startmatch=0;
                }

                if(startmatch==1 && endmatch==0){ // Swap primitive direction.
                    swap(i);
                    startmatch=0; endmatch=1;
                }

                if(startmatch==0 && endmatch==1){ // Match ok.



                    datavec.at(i).contourtype=contour_type::multi_open;
                    datavec.at(i).select=true;
                    c.primitive_sequence.push_back(datavec.at(i));

                    // Find next attached primitive.
                    unsigned int target;
                    bool found=0;
                    unsigned int source=i;
                    while(1){
                        found=find_next(source,target,tol);

                        if(found){
                            datavec.at(target).contourtype=contour_type::multi_open;
                            datavec.at(target).select=true;
                            c.primitive_sequence.push_back(datavec.at(target));
                            source=target;
                        }
                        if(!found){

                            bool closed=find_end_start_match(source,i,tol); // If its a closed contour, change the contourtype flags.
                            if(closed){
                                std::cout<<"this is a closed contour"<<std::endl;
                                for(unsigned int k=0; k<c.primitive_sequence.size(); k++){
                                    c.primitive_sequence.at(k).contourtype=contour_type::multi_closed;
                                }
                            }
                            contourvec.push_back(c); // contourvec is related to contournumber
                            break;
                        }
                    }
                }
            }
        }
    }
}

void contours::check_for_single_open_contours(double tol){

    int startmatch=0, endmatch=0;
    for(unsigned int i=0; i<datavec.size(); i++){
        startmatch=0; endmatch=0;
        for(unsigned int j=0; j<datavec.size(); j++){
            if(i!=j && datavec.at(i).select==0){ // Don't compare the same primitive or previous selected primitives.

                startmatch+=find_start_start_match(i,j,tol);
                startmatch+=find_start_end_match(i,j,tol);
                endmatch+=find_end_start_match(i,j,tol);
                endmatch+=find_end_end_match(i,j,tol);

                contour c;

                if(startmatch==0 && endmatch==0){ // Found a single open primitive, like a line, arc or spline.
                    datavec.at(i).contourtype=contour_type::single_open;
                    datavec.at(i).select=true;
                    c.primitive_sequence.push_back(datavec.at(i));
                    contourvec.push_back(c);
                }
            }
        }
    }

    // When we have only one primitive item in the screen.
    if(datavec.size()==1 && datavec.at(0).select==0){
        startmatch+=find_start_start_match(0,0,tol);
        startmatch+=find_start_end_match(0,0,tol);
        endmatch+=find_end_start_match(0,0,tol);
        endmatch+=find_end_end_match(0,0,tol);

        std::cout<<"startmatch value:"<<startmatch<<std::endl;
        std::cout<<"endmatch value:"<<endmatch<<std::endl;

        contour c;

        if(startmatch==1 && endmatch==1){
            datavec.at(0).contourtype=contour_type::single_open;
            datavec.at(0).select=true;
            c.primitive_sequence.push_back(datavec.at(0));
            contourvec.push_back(c);
        }
    }
}

void contours::swap(unsigned int i /*ojbect to swap*/){

    double start_x=datavec.at(i).start.X();
    double start_y=datavec.at(i).start.Y();
    double start_z=datavec.at(i).start.Z();

    double end_x=datavec.at(i).end.X();
    double end_y=datavec.at(i).end.Y();
    double end_z=datavec.at(i).end.Z();

    datavec.at(i).start.SetX(end_x);
    datavec.at(i).start.SetY(end_y);
    datavec.at(i).start.SetZ(end_z);

    datavec.at(i).end.SetX(start_x);
    datavec.at(i).end.SetY(start_y);
    datavec.at(i).end.SetZ(start_z);

    std::reverse(datavec.at(i).control.begin(), datavec.at(i).control.end());
}

//! Helper function, it=source, it1=attached target to find. If no target is found the function returns a 0.
int contours::find_next(unsigned int i /*source*/, unsigned int &target /*target*/, double tol){

    int startmatch=0, endmatch=0;

    for(unsigned int j=0; j<datavec.size(); j++){
        if(i!=j && datavec.at(j).select==0){ // Don't compare the same primitive or previous selected primitives.

            startmatch+=find_end_start_match(i,j,tol);
            endmatch+=find_end_end_match(i,j,tol);

            if(endmatch==1 && startmatch==0){ // Swap primitive direction.
                swap(j);
                endmatch=0; startmatch=1;
            }

            if(startmatch==1 && endmatch==0){ // Match ok.
                target=j;
                return 1;
            }
        }
    }
    return 0; // Nothing is found.
}

//! Helper function
int contours::find_start_start_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol){

    double P0_xs=datavec.at(i).start.X();
    double P0_ys=datavec.at(i).start.Y();
    double P0_zs=datavec.at(i).start.Z();
    double P1_xs=datavec.at(j).start.X();
    double P1_ys=datavec.at(j).start.Y();
    double P1_zs=datavec.at(j).start.Z();

    if(P0_xs<P1_xs+tol && P0_xs>P1_xs-tol && P0_ys<P1_ys+tol && P0_ys>P1_ys-tol && P0_zs<P1_zs+tol && P0_zs>P1_zs-tol){
        return 1;
    }
    return 0;
}

int contours::find_start_end_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol){

    double P0_xs=datavec.at(i).start.X();
    double P0_ys=datavec.at(i).start.Y();
    double P0_zs=datavec.at(i).start.Z();
    double P1_xe=datavec.at(j).end.X();
    double P1_ye=datavec.at(j).end.Y();
    double P1_ze=datavec.at(j).end.Z();

    if(P0_xs<P1_xe+tol && P0_xs>P1_xe-tol && P0_ys<P1_ye+tol && P0_ys>P1_ye-tol && P0_zs<P1_ze+tol && P0_zs>P1_ze-tol){
        return 1;
    }
    return 0;
}

int contours::find_end_start_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol){

    double P0_xe=datavec.at(i).end.X();
    double P0_ye=datavec.at(i).end.Y();
    double P0_ze=datavec.at(i).end.Z();
    double P1_xs=datavec.at(j).start.X();
    double P1_ys=datavec.at(j).start.Y();
    double P1_zs=datavec.at(j).start.Z();

    if(P0_xe<P1_xs+tol && P0_xe>P1_xs-tol && P0_ye<P1_ys+tol && P0_ye>P1_ys-tol && P0_ze<P1_zs+tol && P0_ze>P1_zs-tol){
        return 1;
    }
    return 0;
}

int contours::find_end_end_match(unsigned int i /*source*/, unsigned int j /*target*/, double tol){

    double P0_xe=datavec.at(i).end.X();
    double P0_ye=datavec.at(i).end.Y();
    double P0_ze=datavec.at(i).end.Z();
    double P1_xe=datavec.at(j).end.X();
    double P1_ye=datavec.at(j).end.Y();
    double P1_ze=datavec.at(j).end.Z();

    if(P0_xe<P1_xe+tol && P0_xe>P1_xe-tol && P0_ye<P1_ye+tol && P0_ye>P1_ye-tol && P0_ze<P1_ze+tol && P0_ze>P1_ze-tol){
        return 1;
    }
    return 0;
}



void contours::print_result(){

    std::cout<<"primitive types: point=0, line=1, lwpolyline=2, circle=3, arc=4, spline=5, ellipse=6 "<<std::endl;
    std::cout<<"contour types:  none=0, single_open=1, single_closed=2, multi_open=3, multi_closed=4 "<<std::endl;

    for(unsigned int i=0; i<contourvec.size(); i++){
        std::cout<<"contournumber: "<<i<<std::endl;
        for(unsigned int j=0; j<contourvec.at(i).primitive_sequence.size(); j++){

            std::cout<<"    contoursequence         : " << j <<std::endl;
            std::cout<<"    contourtype             : " << contourvec.at(i).primitive_sequence.at(j).contourtype<<std::endl;
            std::cout<<"    primitive type          : " << contourvec.at(i).primitive_sequence.at(j).primitivetype<<std::endl;
            std::cout<<"        startpoint x: " << contourvec.at(i).primitive_sequence.at(j).start.X()<<std::endl;
            std::cout<<"                   y: " << contourvec.at(i).primitive_sequence.at(j).start.Y()<<std::endl;
            std::cout<<"                   z: " << contourvec.at(i).primitive_sequence.at(j).start.Z()<<std::endl;
            std::cout<<"        endpoint   x: " << contourvec.at(i).primitive_sequence.at(j).end.X()<<std::endl;
            std::cout<<"                   y: " << contourvec.at(i).primitive_sequence.at(j).end.Y()<<std::endl;
            std::cout<<"                   z: " << contourvec.at(i).primitive_sequence.at(j).end.Z()<<std::endl;

        }
    } std::cout<<""<<std::endl;

}





















































