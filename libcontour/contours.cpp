#include "contours.h"
#include <math.h>

contours::contours()
{

}

void contours::main(double tol, std::string layer){
    contourvec.clear();
    init_primitives(layer);
    check_for_single_closed_contours(tol);
    check_for_multi_contours(tol);
    check_for_single_open_contours(tol);
    area(); // Convert all contours to clockwise [cw].

    // Wich contour is within another. This function gives a depth sequence as output saved as contourvec[i].depth
    // Depth 0 is toplevel. The one's with no insides.
    // Depth 1 is a inside contour of depth 0.
    add_contour_depth_sequence();
    maxdepth=get_max_depth();           // Save the maxdepth value as a global variable.
    kpt_sequence=keep_parts_together(); // Save the keep parts together list as a global variable list.

    // For contour offsets:
    // All contours are standard cw. When depth sequence is know'n, we can apply the ccw dir to contours.
    // add_contour_ccw(); -> done in mainwindow class.

    // print_result();
    // print_depth_sequence();
}

//! Function to get the maxdepth value. A toplevel contour should be depth=0. A inside contour of depth=0 would be depth=1.
int contours::get_max_depth(){
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

//! Standard it cut's following the contour depth sequence. Depth 0 = toplevel. Depth 1 is a inside of depth 0.
std::vector<unsigned int> contours::keep_parts_together(){

    std::vector<unsigned int> kpt_sequence; // KeepPartsTogether_Sequence. The "unsigned int" sequence is contourvec.at[i].

    // Depth result example, 0 is toplevel:
    // 0,1,2
    //
    // Will result in cut sets:
    // 0,1 - 2

    // Find maxdepth value.

    // std::cout<<"maxdepth: "<<maxdepth<<std::endl;

    // If maxdepth is equal, this means the first layer to cut is not a set. ( inner + outer )
    // This depth is cut as normal.
    if(maxdepth%2==0){
        for(unsigned int j=0; j<contourvec.size(); j++){
            if(contourvec.at(j).depth==maxdepth){
                kpt_sequence.push_back(j);
            }
        }
        // Maxdept is set to equal now. Cut sets (outer + inner) can be made.
        maxdepth-=1;
        // std::cout<<"maxdepth reduced to: "<<maxdepth<<std::endl;
    }

    // Starting at the bottom contour, end up at toplevel contour.
    for(int i=maxdepth; i>-1; i--){

        // Starting at a even depth number.
        if(i%2==0){
            for(unsigned int j=0; j<contourvec.size(); j++){
                // The outer contour at even depth number.
                if(contourvec.at(j).depth==i){
                    // Look for childs of [i] in range depth+1.
                    for(unsigned int k=0; k<contourvec.at(j).childcontours.size(); k++){

                        // Find child depth:
                        int child_depth=contourvec.at(contourvec.at(j).childcontours.at(k)).depth;
                        if(child_depth==i+1){
                            // Cut the childs of depth+1/
                            kpt_sequence.push_back(contourvec.at(j).childcontours.at(k));
                            // std::cout<<"child to cut i: "<<contourvec.at(j).childcontours.at(k)<<std::endl;
                        }
                    }
                    // Cut the outer contour depth+0.
                    kpt_sequence.push_back(j);
                    // std::cout<<"father to cut i: "<<j<<std::endl;
                }
            }
        }
    }

    // Print result:
   for(unsigned int i=0; i<kpt_sequence.size(); i++){
       std::cout<<"contour to cut i: "<<kpt_sequence.at(i)<<std::endl;
   }

   return kpt_sequence;
}

void contours::add_contour_ccw(){
    // Find maxdepth value.
    int maxdepth=0;
    for(unsigned int i=0; i<contourvec.size(); i++){
        if(maxdepth<contourvec.at(i).depth){
            maxdepth=contourvec.at(i).depth;
        }
    }
    // std::cout<<"maxdepth:"<<maxdepth<<std::endl;

    // Swap contours to [ccw] if depth value is non-equal.
    for(int i=0; i<maxdepth+1; i++){ // Added +1 to solve error when maxdepth was 1.
        if(i%2==0){ // %2 Slogan to find equal or non-equal.
            // [cw]
        } else {
            for(unsigned int j=0; j<contourvec.size(); j++){ // [ccw]
                if(contourvec.at(j).depth==i){
                    swap_contour(j);
                }
            }
        }
    }
}

void contours::add_contour_depth_sequence(){

    std::vector<unsigned int> contourlist;
    std::vector<unsigned int> insidelist;

    // Create a point array of each contour. Then check if a external contourpoint is inside this polygon.
    for(unsigned int i=0; i<contourvec.size(); i++){
        contourlist.push_back(i);
        std::vector<gp_Pnt> polygon; // Save points of a contour.
        for(unsigned int j=0; j<contourvec.at(i).primitive_sequence.size(); j++){
            polygon.push_back(contourvec.at(i).primitive_sequence.at(j).start);
            for(unsigned int k=0; k<contourvec.at(i).primitive_sequence.at(j).control.size(); k++){
                polygon.push_back(contourvec.at(i).primitive_sequence.at(j).control.at(k));
            }
        }
        polygon.push_back(contourvec.at(i).primitive_sequence.back().end); // Last point of polygon.

        // Perform a pip for each external contour.
        for(unsigned int ii=0; ii<contourvec.size(); ii++){
            if(i!=ii){ // Source contour may not be target contour.
                int ok=point_in_polygon(polygon,contourvec.at(ii).primitive_sequence.front().start); // A point.
                if(ok==1){ // Point is inside this contour, save id of contour.
                    contourvec.at(i).childcontours.push_back(ii);
                }
            }
        }
    }

    /*
    // Print result of child contours.
    for(unsigned int i=0; i<contourvec.size(); i++){
        for(unsigned int j=0; j<contourvec.at(i).childcontours.size(); j++){
            std::cout<<"Contourvec.at: "<<i<<" child contour: "<<contourvec.at(i).childcontours.at(j)<<std::endl;
        }
    } std::cout<<" "<<std::endl; */

    // Write depth values to the contours.
    // Depth 0 is toplevel. The one's with no insides.
    // Depth 1 is a inside contour of depth 0.
    // Depth 2 is a inside contour of depth 1. And so on.

    unsigned int depth=0;
    // Depth 0, init loop.
    contourlist=get_toplevel(contourlist);
    for(unsigned int i=0; i<contourlist.size(); i++){
        contourvec.at(contourlist.at(i)).depth=depth;
        // std::cout<<"contourlist depth 0: "<<contourlist.at(i)<<std::endl;
    }

    // Depth 1+ auto loop until everything is done.
    while(contourlist.size()!=0){
        depth++;
        insidelist=get_childs(contourlist);
        contourlist=get_toplevel(insidelist);
        for(unsigned int i=0; i<contourlist.size(); i++){
            contourvec.at(contourlist.at(i)).depth=depth;
            // std::cout<<"contourlist dept "<<depth<<": "<<contourlist.at(i)<<std::endl;
        }
    }
}

std::vector<unsigned int> contours::get_childs(std::vector<unsigned int> list){
    std::vector<unsigned int> result;
    for(unsigned int i=0; i<list.size(); i++){
        for(unsigned int j=0; j<contourvec.at(list.at(i)).childcontours.size(); j++){
            //no duplicates
            if(std::find(result.begin(), result.end(),contourvec.at(list.at(i)).childcontours.at(j))!=result.end()){
                //do nothing
            } else {
                result.push_back(contourvec.at(list.at(i)).childcontours.at(j));
            }
        }
    }
    return result;
}

std::vector<unsigned int> contours::get_toplevel(std::vector<unsigned int> list){

    // Save the childs of the list.
    std::vector<unsigned int> childs, result;
    for(unsigned int i=0; i<list.size(); i++){
        for(unsigned int j=0; j<contourvec.at(list.at(i)).childcontours.size(); j++){
            //no duplicates
            if(std::find(childs.begin(), childs.end(),contourvec.at(list.at(i)).childcontours.at(j))!=childs.end()){
                //do nothing
            } else {
                childs.push_back(contourvec.at(list.at(i)).childcontours.at(j));
            }
        }
    }

    for(unsigned int i=0; i<childs.size(); i++){
        // std::cout<<"childs: "<<childs.at(i)<<std::endl;
    }

    result=lista_min_listb(list,childs);
    return result;
}

std::vector<unsigned int> contours::lista_min_listb(std::vector<unsigned int> lista, std::vector<unsigned int> listb){

    std::vector<unsigned int> resultlist;
    std::vector<unsigned int>::iterator it,it1;
    resultlist=lista;

    for(it=resultlist.begin(); it<resultlist.end(); it++){
        for(it1=listb.begin(); it1<listb.end(); it1++){
            if(*it==*it1){
                resultlist.erase(it);
            }
        }
    }
    //print output
    //    std::cout<<"contourlist min depthlist:"<<std::endl;
    //    for(int i=0; i<contourlist_min_depthlist.size(); i++){
    //        std::cout<<contourlist_min_depthlist.at(i)<<std::endl;
    //    }
    return resultlist;
}

int contours::point_in_polygon(std::vector<gp_Pnt> polygon, gp_Pnt point){

    int n = polygon.size();
    // pos = array of x,y positions of the polygon to investegate
    // x,y = point to check inside the polygon
    // c = 1 if true

    int i, j, c = 0;
    for (i = 0, j = n-1; i < n; j = i++) {
        if ( ((polygon.at(i).Y()>point.Y()) != (polygon.at(j).Y()>point.Y())) && (point.X() < (polygon.at(j).X()-polygon.at(i).X()) * (point.Y()-polygon.at(i).Y()) / (polygon.at(j).Y()-polygon.at(i).Y()) + polygon.at(i).X()) ){
            c = !c;
        }
    }
    return c;
}

void contours::swap_contour(unsigned int i /*contourvec.at(i)*/){

    // Invert each individual primitive that is member of the contour.
    for(unsigned int j=0; j<contourvec.at(i).primitive_sequence.size(); j++){

        // Invert startpoint with endpoint of the primitive.
        double xs=contourvec.at(i).primitive_sequence.at(j).start.X();
        double ys=contourvec.at(i).primitive_sequence.at(j).start.Y();
        double zs=contourvec.at(i).primitive_sequence.at(j).start.Z();
        double xe=contourvec.at(i).primitive_sequence.at(j).end.X();
        double ye=contourvec.at(i).primitive_sequence.at(j).end.Y();
        double ze=contourvec.at(i).primitive_sequence.at(j).end.Z();

        contourvec.at(i).primitive_sequence.at(j).start.SetX(xe);
        contourvec.at(i).primitive_sequence.at(j).start.SetY(ye);
        contourvec.at(i).primitive_sequence.at(j).start.SetZ(ze);
        contourvec.at(i).primitive_sequence.at(j).end.SetX(xs);
        contourvec.at(i).primitive_sequence.at(j).end.SetY(ys);
        contourvec.at(i).primitive_sequence.at(j).end.SetZ(zs);

        // Invert the controlpoints of the primitive.
        std::reverse(contourvec.at(i).primitive_sequence.at(j).control.begin(), contourvec.at(i).primitive_sequence.at(j).control.end());

    }

    // Invert the contour primitive sequence.
    std::reverse(contourvec.at(i).primitive_sequence.begin(), contourvec.at(i).primitive_sequence.end());
}

void contours::init_primitives(std::string layer){

    std::vector<datas>::iterator it;
    for(it=dxfvec.begin(); it<dxfvec.end(); it++){
        it->select=false;
        // Only copy the selected layer to the datavec.
        if(it->acad_layer==layer){
            datavec.push_back(*it);
        }
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

                // std::cout<<"startmatch value:"<<startmatch<<std::endl;
                // std::cout<<"endmatch value:"<<endmatch<<std::endl;

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
                                // std::cout<<"this is a closed contour"<<std::endl;
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

        // std::cout<<"startmatch value:"<<startmatch<<std::endl;
        // std::cout<<"endmatch value:"<<endmatch<<std::endl;

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

void contours::area(){

    // Calculate if contour is [cw] or [ccw].
    double area=0;
    double xs=0,ys=0,xe=0,ye=0;

    // Basic formula for area, add each contour point with : area+=(it1->X()-it->X())*(it1->Y()+it->Y());
    // Then area/2=result in mm2.

    for(unsigned int i=0; i<contourvec.size(); i++){
        area=0;
        // std::cout<<"contournumber: "<<i<<std::endl;
        if(contourvec.at(i).primitive_sequence.at(0).contourtype==contour_type::multi_closed || contourvec.at(i).primitive_sequence.at(0).contourtype==contour_type::single_closed){

            for(unsigned int j=0; j<contourvec.at(i).primitive_sequence.size(); j++){

                xs=contourvec.at(i).primitive_sequence.at(j).start.X();
                ys=contourvec.at(i).primitive_sequence.at(j).start.Y();
                double xc_front=contourvec.at(i).primitive_sequence.at(j).control.front().X();
                double yc_front=contourvec.at(i).primitive_sequence.at(j).control.front().Y();
                area+=(xc_front-xs)*(yc_front+ys);

                if(contourvec.at(i).primitive_sequence.at(j).control.size()>1){
                    for(unsigned int k=0; k<contourvec.at(i).primitive_sequence.at(j).control.size()-1; k++){
                        double xca=contourvec.at(i).primitive_sequence.at(j).control.at(k).X();
                        double yca=contourvec.at(i).primitive_sequence.at(j).control.at(k).Y();
                        double xcb=contourvec.at(i).primitive_sequence.at(j).control.at(k+1).X();
                        double ycb=contourvec.at(i).primitive_sequence.at(j).control.at(k+1).Y();
                        area+=(xcb-xca)*(ycb+yca);
                    }
                }

                double xc_back=contourvec.at(i).primitive_sequence.at(j).control.back().X();
                double yc_back=contourvec.at(i).primitive_sequence.at(j).control.back().Y();
                xe=contourvec.at(i).primitive_sequence.at(j).end.X();
                ye=contourvec.at(i).primitive_sequence.at(j).end.Y();
                area+=(xe-xc_back)*(ye+yc_back);
            }
        }
        area=area/2; // Area/2=result in mm2.

        if(area<0){ // If a ccw, invert to cw.
            swap_contour(i);
            area*=-1;
        }

        contourvec.at(i).area=area;

        // std::cout<<"area mm2: "<<area<<std::endl;

    } // std::cout<<""<<std::endl;
}

void contours::print_result(){

    std::cout<<"primitive types: point=0, line=1, lwpolyline=2, circle=3, arc=4, spline=5, ellipse=6 "<<std::endl;
    std::cout<<"contour types:  none=0, single_open=1, single_closed=2, multi_open=3, multi_closed=4 "<<std::endl;

    for(unsigned int i=0; i<contourvec.size(); i++){
        std::cout<<"contournumber: "<<i<<std::endl;
        std::cout<<"contourarea  : "<<contourvec.at(i).area<<std::endl;
        std::cout<<"contourdepth : "<<contourvec.at(i).depth<<std::endl;
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

void contours::print_depth_sequence(){
    for(unsigned int i=0; i<contourvec.size(); i++){
        std::cout<<"contournumber: "<<i<<" depth:"<<contourvec.at(i).depth<<std::endl;
    }
}






//// Find childs of selected contours.
//std::vector<unsigned int> leftoverlist;
//for(unsigned int i=0; i<contourvec.size(); i++){
//    if(contourvec.at(i).select==true){
//        // Mark childs with new depth.
//        for(unsigned int j=0; j<contourvec.at(i).childcontours.size(); j++){
//            contourvec.at(contourvec.at(i).childcontours.at(j)).depth=depth;
//            leftoverlist.push_back(contourvec.at(i).childcontours.at(j));
//        }
//    }
//}

//// Print result of leftoverlist.
//for(unsigned int i=0; i<leftoverlist.size(); i++){
//    std::cout<<"leftoverlist: "<<leftoverlist.at(i)<<std::endl;
//}

//// Get childs of the leftoverlist.
//std::vector<unsigned int> insideslist;
//for(unsigned int i=0; i<leftoverlist.size(); i++){
//    unsigned int k=leftoverlist.at(i);
//    for(unsigned int j=0; j<contourvec.at(k).childcontours.size(); j++){
//        insideslist.push_back(contourvec.at(k).childcontours.at(j));
//        std::cout<<"childs leftoverlist: "<<contourvec.at(k).childcontours.at(j)<<std::endl;
//    }
//}

//// Remove insides from leftoverlist.
//std::vector<unsigned int> resultlist = lista_min_listb(leftoverlist,insideslist);

//// Print result of toplevellist
//for(unsigned int i=0; i<resultlist.size(); i++){
//    std::cout<<"result list: "<<resultlist.at(i)<<std::endl;
//}














































