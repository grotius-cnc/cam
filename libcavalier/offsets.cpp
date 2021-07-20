#include "offsets.h"

#include <polylineoffset.hpp>
#include <polylineoffsetislands.hpp>

#include "opencascade.h"
#include "draw_primitives.h"
using namespace occ;

offsets::offsets()
{

}

void offsets::do_pockets(double offset_contour /* offset from contour base */, double pocket_overlap /* pocket overlap 0-100% */, offset_action action, double lead_in, double lead_out){



}

void offsets::do_offset(double offset, offset_action action, double lead_in, double lead_out){

    Quantity_Color color;

    if(offset>=0){
        lead_in+=offset;
        lead_out+=offset;
    }
    if(offset<0){
        lead_in=-abs(lead_in)+offset;       std::cout<<"lead_in: "<<lead_in<<std::endl;
        lead_out=-abs(lead_out)+offset;     std::cout<<"lead_out: "<<lead_out<<std::endl;
    }

    for(unsigned int i=0; i<contourvec.size(); i++){

        if(contourvec.at(i).primitive_sequence.at(0).contourtype==contour_type::multi_open || contourvec.at(i).primitive_sequence.at(0).contourtype==contour_type::single_open){

        }

        if(contourvec.at(i).primitive_sequence.at(0).contourtype==contour_type::multi_closed || contourvec.at(i).primitive_sequence.at(0).contourtype==contour_type::single_closed){
            unsigned int ii=i;
            cavc::Polyline<double> outerloop;

            if(contourvec.at(i).depth%2==0){    // [cw]
                color=Quantity_NOC_RED;
            } else {                            // [ccw]
                color=Quantity_NOC_YELLOW;
            }

            for(unsigned int j=0; j<contourvec.at(i).primitive_sequence.size(); j++){

                if(contourvec.at(i).primitive_sequence.at(j).primitivetype==primitive_type::line){
                    double xs=contourvec.at(i).primitive_sequence.at(j).start.X(); // From to ..
                    double ys=contourvec.at(i).primitive_sequence.at(j).start.Y();
                    outerloop.addVertex(xs, ys, 0);
                }

                if(contourvec.at(i).primitive_sequence.at(j).primitivetype==primitive_type::arc){

                    double xs=contourvec.at(i).primitive_sequence.at(j).start.X();
                    double ys=contourvec.at(i).primitive_sequence.at(j).start.Y();

                    double xc=contourvec.at(i).primitive_sequence.at(j).arcmid.at(1).X();
                    double yc=contourvec.at(i).primitive_sequence.at(j).arcmid.at(1).Y();

                    std::vector<double> bulge=arc_bulge(contourvec.at(i).primitive_sequence.at(j));
                    if(bulge[1]==0){ //single arc
                        outerloop.addVertex(xs, ys, bulge[0]); //startpoint arc + bulge
                    }
                    if(bulge[1]==1){ //dual arc
                        outerloop.addVertex(xs, ys, bulge[0]); //startpoint arc + bulge
                        outerloop.addVertex(xc, yc, bulge[0]); //startpoint arc + bulge
                    }
                }
            }
            outerloop.isClosed() = true;

            // Add islands:
            // std::vector<cavc::Polyline<double>> islands; // Islands must be clockwise
            // cavc::Polyline<double> island;
            // island.addVertex(x,y,0 /*bulge*/);
            // islands.push_back(island);


            // Process data
            std::vector<cavc::Polyline<double>> results;
            if(action==offset_action::offset_contour || action==offset_action::lead_base_contour){
                results = cavc::parallelOffset(outerloop, offset);
            }
            if(action==offset_action::lead_in_contour){
                results = cavc::parallelOffset(outerloop, lead_in);
            }
            if(action==offset_action::lead_out_contour){
                results = cavc::parallelOffset(outerloop, lead_out);
            }

            // std::cout<<"cavalier results.size: "<<results.size()<<std::endl;

            // Draw processed data
            datas d;
            for(unsigned int is=0; is<results.size(); is++){ //cw loops
                for(unsigned int j=0; j<results.at(is).vertexes().size()-1; j++){

                    double xs=results.at(is).vertexes().at(j).x();
                    double ys=results.at(is).vertexes().at(j).y();
                    double xe=results.at(is).vertexes().at(j+1).x();
                    double ye=results.at(is).vertexes().at(j+1).y();

                    if(results.at(is).vertexes().at(j).bulge()==0){ //line
                        if(action==offset_action::offset_contour){
                            Handle(AIS_Shape) ashape=draw_primitives().draw_3d_line({xs,ys,0},{xe,ye,0});
                            ashape=draw_primitives().colorize(ashape,color,1);
                            d.ashape=ashape;
                            d.start.SetX(xs); d.start.SetY(ys); d.start.SetZ(0);
                            d.end.SetX(xe); d.end.SetY(ye); d.end.SetZ(0);
                            d.primitivetype=primitive_type::line;
                            contourvec.at(ii).offset_sequence.push_back(d);
                        }
                    }

                    if(results.at(is).vertexes().at(j).bulge()!=0){ //arc

                        // std::cout<<"bulge: "<<results.at(i).vertexes().at(j).bulge()<<std::endl;

                        POINT p=bulge_to_arc_controlpoint({xs,ys,0},{xe,ye,0},results.at(is).vertexes().at(j).bulge());
                        POINT center=arc_center({xs,ys,0},{p.x,p.y,0},{xe,ye,0});

                        if(action==offset_action::offset_contour){
                            Handle(AIS_Shape) ashape=draw_primitives().draw_3p_3d_arc({xs,ys,0},{p.x,p.y,0},{xe,ye,0});
                            ashape=draw_primitives().colorize(ashape,color,1);
                            d.ashape=ashape;
                            d.start.SetX(xs); d.start.SetY(ys); d.start.SetZ(0);
                            d.end.SetX(xe); d.end.SetY(ye); d.end.SetZ(0);
                            d.arcmid.push_back({p.x,p.y,p.z}); // Controlpoint half way arc's circumfence.
                            d.bulge=results.at(is).vertexes().at(j).bulge(); // If bulge<0=[cw]
                            d.center={center.x,center.y,center.z};
                            d.primitivetype=primitive_type::arc;
                            contourvec.at(i).offset_sequence.push_back(d);
                        }
                    }

                    if(action==offset_action::lead_base_contour){
                        contourvec.at(i).lead_base.points.push_back({xs,ys,0});
                    }
                    if(action==offset_action::lead_in_contour){
                        contourvec.at(i).lead_in.points.push_back({xs,ys,0});
                    }
                    if(action==offset_action::lead_out_contour){
                        contourvec.at(i).lead_out.points.push_back({xs,ys,0});
                    }
                }
                //add last primitive of contour
                double xs=results.at(is).vertexes().back().x();
                double ys=results.at(is).vertexes().back().y();
                double xe=results.at(is).vertexes().front().x();
                double ye=results.at(is).vertexes().front().y();

                // Add last lead_in_out points to the vector.
                if(action==offset_action::lead_base_contour){
                    contourvec.at(i).lead_base.points.push_back({xs,ys,0});
                }
                if(action==offset_action::lead_in_contour){
                    contourvec.at(i).lead_in.points.push_back({xs,ys,0});
                }
                if(action==offset_action::lead_out_contour){
                    contourvec.at(i).lead_out.points.push_back({xs,ys,0});
                }

                if(results.at(is).vertexes().back().bulge()==0){ //line
                    if(action==offset_action::offset_contour){
                        Handle(AIS_Shape) ashape=draw_primitives().draw_3d_line({xs,ys,0},{xe,ye,0});
                        ashape=draw_primitives().colorize(ashape,color,1);
                        d.ashape=ashape;
                        d.start.SetX(xs); d.start.SetY(ys); d.start.SetZ(0);
                        d.end.SetX(xe); d.end.SetY(ye); d.end.SetZ(0);
                        d.primitivetype=primitive_type::line;
                        contourvec.at(ii).offset_sequence.push_back(d);
                    }
                }
                if(results.at(is).vertexes().back().bulge()!=0){ //arc

                    // std::cout<<"bulge last arc: "<<results.at(i).vertexes().back().bulge()<<std::endl;

                    POINT p=bulge_to_arc_controlpoint({xs,ys,0},{xe,ye,0},results.at(is).vertexes().back().bulge());
                    POINT center=arc_center({xs,ys,0},{p.x,p.y,p.z},{xe,ye,0});

                    if(action==offset_action::offset_contour){
                        Handle(AIS_Shape) ashape=draw_primitives().draw_3p_3d_arc({xs,ys,0},{p.x,p.y,0},{xe,ye,0});
                        ashape=draw_primitives().colorize(ashape,color,1);
                        d.ashape=ashape;
                        d.start.SetX(xs); d.start.SetY(ys); d.start.SetZ(0);
                        d.end.SetX(xe); d.end.SetY(ye); d.end.SetZ(0);
                        d.arcmid.push_back({p.x,p.y,p.z}); // Controlpoint half way arc's circumfence.
                        d.bulge=results.at(is).vertexes().back().bulge(); // If bulge<0=[cw]
                        d.center={center.x,center.y,center.z};
                        d.primitivetype=primitive_type::arc;
                        contourvec.at(ii).offset_sequence.push_back(d);
                    }
                }
            }
        } // std::cout<<""<<std::endl;
    }
    create_lead_in_out(lead_in,lead_out);
}

void offsets::rotate_primairy_contour(unsigned int i /* contourvec[i] */){
    // Rotate contourvec +1
    std::rotate(contourvec.at(i).primitive_sequence.begin(),contourvec.at(i).primitive_sequence.begin()+1,contourvec.at(i).primitive_sequence.end());
}

void offsets::create_lead_in_out(double lead_in, double lead_out){

    Quantity_Color color;

    for(unsigned int i=0; i<contourvec.size(); i++){

        if(contourvec.at(i).depth%2==0){    // [cw]
            color=Quantity_NOC_RED;
        } else {                            // [ccw]
            color=Quantity_NOC_YELLOW;
        }

        // Create lead-in shapes.
        for(unsigned int k=0; k<contourvec.at(i).lead_base.points.size(); k++){
            for(unsigned int l=0; l<contourvec.at(i).lead_in.points.size(); l++){
                gp_Pnt p1,p2;
                p1=contourvec.at(i).lead_in.points.at(l);
                p2=contourvec.at(i).lead_base.points.at(k);
                double lenght=draw_primitives().get_3d_line_lenght(p1,p2);
                // std::cout<<"lenght: "<<lenght<<std::endl;
                if(lenght<abs(lead_in)*2 ){ // Failsafe solution. Abs is used when lead_in has a negative value. This happens when user defines a contour offset value < 0.
                    contourvec.at(i).lead_in.ashape = draw_primitives().draw_3d_line(p1,p2);
                    contourvec.at(i).lead_in.ashape = draw_primitives().colorize(contourvec.at(i).lead_in.ashape,color,0);
                    // OpencascadeWidget->show_shape(contourvec.at(i).lead_in.ashape);
                    // Stop, exit.
                    k=contourvec.at(i).lead_base.points.size();
                    break;
                }
            }
        }

        // Create lead-out shapes.
        for(unsigned int k=0; k<contourvec.at(i).lead_base.points.size(); k++){
            for(unsigned int l=0; l<contourvec.at(i).lead_out.points.size(); l++){
                gp_Pnt p1,p2;
                p1=contourvec.at(i).lead_out.points.at(l);
                p2=contourvec.at(i).lead_base.points.at(k);
                double lenght=draw_primitives().get_3d_line_lenght(p1,p2);
                // std::cout<<"lenght: "<<lenght<<std::endl;
                if(lenght<abs(lead_out)*2){ // Failsafe solution.
                    contourvec.at(i).lead_out.ashape = draw_primitives().draw_3d_line(p1,p2);
                    contourvec.at(i).lead_out.ashape = draw_primitives().colorize(contourvec.at(i).lead_out.ashape,color,0);
                    // OpencascadeWidget->show_shape(contourvec.at(i).lead_out.ashape);
                    // Stop, exit.
                    k=contourvec.at(i).lead_base.points.size();
                    break;
                }
            }
        }
    }
}

POINT offsets::arc_center(POINT a /* arc startpoint */, POINT b /* arc controlpoint*/, POINT c /* arc endpoint */){
    POINT center;

    double x1 = (a.x+b.x)/2;
    double y1 = (a.y+b.y)/2;
    double dy1 = b.x - a.x;
    double dx1 = -(b.y - a.y);

    // Get the perpendicular bisector of (x2, y2) and (x3, y3).
    double x2 = (b.x+c.x)/2;
    double y2 = (b.y+c.y)/2;
    double dy2 = c.x - b.x;
    double dx2 = -(c.y - b.y);

    double endpoint_x0 = x1 + dx1;
    double endpoint_y0 = y1 + dy1;
    double endpoint_x1 = x2 + dx2;
    double endpoint_y1 = y2 + dy2;

    //line 1
    double delta_y0 = endpoint_y0 - y1;
    double delta_x0 = x1 - endpoint_x0;
    double c0 = delta_y0 * x1 + delta_x0 * y1;
    //line 2
    double delta_y1 = endpoint_y1 - y2;
    double delta_x1 = x2 - endpoint_x1;
    double c1 = delta_y1 * x2 + delta_x1 * y2;

    double determinant = delta_y0*delta_x1 - delta_y1*delta_x0;

    if (determinant == 0) // The lines are parallel.
    {
        // lines are parallel
        std::cout<<"Error in arc_center function, class offset.cpp"<<std::endl;;
    }
    else  {
        center.x = (delta_x1*c0 - delta_x0*c1)/determinant; // std::cout<<"function offset.ccp, arc center x: "<<center.x<<std::endl;
        center.y = (delta_y0*c1 - delta_y1*c0)/determinant; // std::cout<<"function offset.ccp, arc center y: "<<center.y<<std::endl;
        center.z = a.z;
    }
    return center;
}

std::vector<double> offsets::arc_bulge(datas p /* primitive */){

    //https://github.com/jbuckmccready/CavalierContours/issues/17

    //bulge neg=g2
    //bulge pos=g3
    double pi_angle_arc=0;
    double pi_angle_start=0;
    double pi_angle_end=0;
    std::vector<double> bulge; //bulge[0]=bulge value, bulge[1]=0 (one arc), bulge[1]=1 (two arcs)


    POINT a={p.start.X(),p.start.Y(),p.start.Z()};   // it->start;
    POINT b={p.end.X(),p.end.Y(),p.end.Z()};

    // std::cout<<"arc start x:"<<p.start.X()<<" y:"<<p.start.Y()<<" z:"<<p.start.Z()<<std::endl;
    // std::cout<<"arc end   x:"<<p.end.X()  <<" y:"<<p.end.Y()  <<" z:"<<p.end.Z()  <<std::endl;

    POINT control={p.arcmid.at(1).X(),p.arcmid.at(1).Y(),p.arcmid.at(1).Z()};
    double d=arc_determinant(a,control,b);

    POINT c={p.center.X(),p.center.Y(),p.center.Z()};

    if(d>0){ //g2, d=determinant
        pi_angle_start = atan2(b.y-c.y, b.x-c.x);
        pi_angle_end = atan2(a.y-c.y, a.x-c.x);
    }
    if(d<0){ //g3
        pi_angle_start = atan2(a.y-c.y, a.x-c.x);
        pi_angle_end  = atan2(b.y-c.y, b.x-c.x);
    }
    if(d==0){
        bulge.push_back(0); //draw straight line
        bulge.push_back(0);
    }

    if(pi_angle_end<pi_angle_start){pi_angle_end+=2*M_PI;}
    pi_angle_arc=pi_angle_end-pi_angle_start;

    if(pi_angle_arc>M_PI){ //split up in 2 arcs
        double half_pi_angle_arc=pi_angle_arc/2;
        double bulges=tan(half_pi_angle_arc/4);
        if(d<0){
            bulge.push_back(abs(bulges));
            bulge.push_back(1);
        }
        if(d>0){
            bulge.push_back(-abs(bulges));
            bulge.push_back(1);
        }

    } else {
        if(d<0){
            bulge.push_back(abs(tan(pi_angle_arc/4))); //keep it as 1 arc
            bulge.push_back(0);
        }
        if(d>0){
            bulge.push_back(-abs(tan(pi_angle_arc/4))); //keep it as 1 arc
            bulge.push_back(0);
        }
    }

    return bulge;
}

double offsets::arc_determinant(POINT a /* arc startpoint */, POINT b /* a arc circumfence point*/, POINT c /* arc endpoint*/){

    double x1 = (b.x + a.x) / 2;
    double y1 = (b.y + a.y) / 2;
    double dy1 = b.x - a.x;
    double dx1 = -(b.y - a.y);

    //Get the perpendicular bisector of (x2, y2) and (x3, y3).
    double x2 = (c.x + b.x) / 2;
    double y2 = (c.y + b.y) / 2;
    double dy2 = c.x - b.x;
    double dx2 = -(c.y - b.y);

    double endpoint_x0 = x1 + dx1;
    double endpoint_y0 = y1 + dy1;
    double endpoint_x1 = x2 + dx2;
    double endpoint_y1 = y2 + dy2;

    //line 1
    double delta_y0 = endpoint_y0 - y1;
    double delta_x0 = x1 - endpoint_x0;

    //line 2
    double delta_y1 = endpoint_y1 - y2;
    double delta_x1 = x2 - endpoint_x1;

    double determinant = delta_y0*delta_x1 - delta_y1*delta_x0;

    return determinant*-1; // solution to inverse the determinant.
}

POINT offsets::offset_point_on_line(double xs, double ys, double xe, double ye, double offset_from_xs_ys){
    //  cross calculation
    //    A-----------B------------C
    // (Xa,Ya)     (Xb,Yb)      (Xc,Yc)
    //    AB = sqrt( (Xb - Xa)² + (Yb - Ya)² )
    //    AC = 1000
    //    Xc = Xa + (AC * (Xb - Xa) / AB)
    //    Yc = Ya + (AC * (Yb - Ya) / AB)
    POINT p;
    double AB=sqrt(pow(xe-xs,2)+pow(ye-ys,2));
    double AC=offset_from_xs_ys;
    p.x=xs+(AC*(xe-xs)/AB);
    p.y=ys+(AC*(ye-ys)/AB);
    return p;
}

POINT offsets::bulge_to_arc_controlpoint(POINT p1, POINT p2, double bulge){ //find the arc center

    //bulge neg=g2
    //bulge pos=g3
    //bulge 0=line
    //bulge 1=semicircle
    //m=(p1+p2)/2
    //Bulge = d1/d2=tan(Delta/4)
    //http://www.lee-mac.com/bulgeconversion.html#bulgearc
    //https://math.stackexchange.com/questions/1337344/get-third-point-from-an-arc-constructed-by-start-point-end-point-and-bulge

    double dist=sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2)); //dist=chord lenght p1-p2
    POINT m={(p1.x+p2.x)/2,(p1.y+p2.y)/2,0}; //m=point half chord lenght p1-p2
    double d1=(abs(bulge))*(0.5*dist); //d1=height from m to arc controlpoint

    //normalize p1, rotate around m, bring m back to x,y 0,0
    double p1x_nor=p1.x-m.x;
    double p1y_nor=p1.y-m.y;

    POINT p1_rot;
    if(bulge<0){ //g2
        p1_rot=rotate_3d(p1x_nor,p1y_nor,0,0,0,-90);
    }
    if(bulge>0){ //g3
        p1_rot=rotate_3d(p1x_nor,p1y_nor,0,0,0,90);
    }


    p1_rot.x+=m.x; //bring the rotated p1 back to orginal coordinates
    p1_rot.y+=m.y;

    POINT q=offset_point_on_line(m.x,m.y,p1_rot.x,p1_rot.y,d1);

    //std::cout<<"arc controlpoint x: "<<q.x<<" y: "<<q.y<<std::endl;
    return q; //arc controlpoint

}

POINT offsets::rotate_3d(double x_to_rotate,double y_to_rotate, double z_to_rotate, double rotate_degrees_x, double rotate_degrees_y, double rotate_degrees_z){

    double cx=cos(rotate_degrees_x*M_PI/180.0); //cos of rotate_degrees
    double sx=sin(rotate_degrees_x*M_PI/180.0);
    double matrix_x[4][4] = { //this matrix only calculates rotate_degrees into cos and sin value's
                              { 1, 0,  0,  0 },
                              { 0, cx,-sx, 0 },
                              { 0, sx, cx, 0 },
                              { 0, 0,  0,  1 }
                            };

    double cy=cos(rotate_degrees_y*M_PI/180.0); //cos of rotate_degrees
    double sy=sin(rotate_degrees_y*M_PI/180.0);
    double matrix_y[4][4] = {
        { cy, 0, sy, 0 },
        { 0,  1, 0,  0 },
        {-sy, 0, cy, 0 },
        { 0,  0, 0,  1 },
    };

    double cz=cos(rotate_degrees_z*M_PI/180.0); //cos of rotate_degrees
    double sz=sin(rotate_degrees_z*M_PI/180.0);
    double matrix_z[4][4] = {
        { cz,-sz, 0, 0 },
        { sz, cz, 0, 0 },
        { 0,   0, 1, 0 },
        { 0,   0, 0, 1 }
    };

    double vector[4] = {0, 0, 0, 1}; //[x,y,z,w] //w=1 for translate, w=0 for direction
    vector[0]=x_to_rotate;
    vector[1]=y_to_rotate;
    vector[2]=z_to_rotate;

    double x=(matrix_x[0][0]*vector[0])+(matrix_x[0][1]*vector[1])+(matrix_x[0][2]*vector[2])+(matrix_x[0][3]*vector[3]);
    double y=(matrix_x[1][0]*vector[0])+(matrix_x[1][1]*vector[1])+(matrix_x[1][2]*vector[2])+(matrix_x[1][3]*vector[3]);
    double z=(matrix_x[2][0]*vector[0])+(matrix_x[2][1]*vector[1])+(matrix_x[2][2]*vector[2])+(matrix_x[2][3]*vector[3]);
    double w=(matrix_x[3][0]*vector[0])+(matrix_x[3][1]*vector[1])+(matrix_x[3][2]*vector[2])+(matrix_x[3][3]*vector[3]);

    //rotate around y axis, take the result of the x axis rotation above :
    vector[0]=x;
    vector[1]=y;
    vector[2]=z;
    x=(matrix_y[0][0]*vector[0])+(matrix_y[0][1]*vector[1])+(matrix_y[0][2]*vector[2])+(matrix_y[0][3]*vector[3]);
    y=(matrix_y[1][0]*vector[0])+(matrix_y[1][1]*vector[1])+(matrix_y[1][2]*vector[2])+(matrix_y[1][3]*vector[3]);
    z=(matrix_y[2][0]*vector[0])+(matrix_y[2][1]*vector[1])+(matrix_y[2][2]*vector[2])+(matrix_y[2][3]*vector[3]);
    w=(matrix_y[3][0]*vector[0])+(matrix_y[3][1]*vector[1])+(matrix_y[3][2]*vector[2])+(matrix_y[3][3]*vector[3]);

    //rotate around z axis, take the result of the y axis rotation above :
    vector[0]=x;
    vector[1]=y;
    vector[2]=z;
    x=(matrix_z[0][0]*vector[0])+(matrix_z[0][1]*vector[1])+(matrix_z[0][2]*vector[2])+(matrix_z[0][3]*vector[3]);
    y=(matrix_z[1][0]*vector[0])+(matrix_z[1][1]*vector[1])+(matrix_z[1][2]*vector[2])+(matrix_z[1][3]*vector[3]);
    z=(matrix_z[2][0]*vector[0])+(matrix_z[2][1]*vector[1])+(matrix_z[2][2]*vector[2])+(matrix_z[2][3]*vector[3]);
    w=(matrix_z[3][0]*vector[0])+(matrix_z[3][1]*vector[1])+(matrix_z[3][2]*vector[2])+(matrix_z[3][3]*vector[3]);

    //    cout<<" x:"<<x<<endl;
    //    cout<<" y:"<<y<<endl;
    //    cout<<" z:"<<z<<endl;
    //    cout<<" w:"<<w<<endl;

    POINT result;
    result.x=x;
    result.y=y;
    result.z=z;
    return result;
}
