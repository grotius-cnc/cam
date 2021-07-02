#include "databucket.h"

point p;
std::vector<point> pointvec;
line l;
std::vector<line> linevec;
arc a;
std::vector<arc> arcvec;
circle c;
std::vector<circle> circlevec;
ellipse e;
std::vector<ellipse> ellipsevec;
spline s;
std::vector<spline> splinevec;

databucket::databucket()
{

}

void databucket::clear(){
    std::cout<<"databucket clear"<<std::endl;
    pointvec.clear();
    linevec.clear();
    arcvec.clear();
    circlevec.clear();
    ellipsevec.clear();
    splinevec.clear();
}
void databucket::print_databucket_content(){

    std::cout<<"pointsvec"<<std::endl;
    for(unsigned int i=0; i<pointvec.size(); i++){
        std::cout<<"i:" <<
                   i <<
                   " x:" <<
                   pointvec.at(i).x <<
                   " y:" <<
                   pointvec.at(i).y <<
                   " z:" <<
                   pointvec.at(i).z <<
                   std::endl;
    }

    std::cout<<"linevec, including lwpolylines"<<std::endl;
    for(unsigned int i=0; i<linevec.size(); i++){
        std::cout<<"i:" <<
                   i <<
                   " xs:" <<
                   linevec.at(i).start.x <<
                   " ys:" <<
                   linevec.at(i).start.y <<
                   " zs:" <<
                   linevec.at(i).start.z <<
                   " xe:" <<
                   linevec.at(i).end.x <<
                   " ye:" <<
                   linevec.at(i).end.y <<
                   " ze:" <<
                   linevec.at(i).end.z <<
                   std::endl;
    }

    std::cout<<"arcvec"<<std::endl;
    for(unsigned int i=0; i<arcvec.size(); i++){
        std::cout<<"i:" <<
                   i <<
                   " xc:" <<
                   arcvec.at(i).center.x <<
                   " yc:" <<
                   arcvec.at(i).center.y <<
                   " zc:" <<
                   arcvec.at(i).center.z <<
                   " radius:" <<
                   arcvec.at(i).radius <<
                   " alpha1 (startangle):" <<
                   arcvec.at(i).alpha1 <<
                   " alpha2 (endangle):" <<
                   arcvec.at(i).alpha2 <<
                   " xs (tobecalculated):" <<
                   arcvec.at(i).start.x <<
                   " ys (tobecalculated):" <<
                   arcvec.at(i).start.y <<
                   " zs (tobecalculated):" <<
                   arcvec.at(i).start.z <<
                   " xe (tobecalculated):" <<
                   arcvec.at(i).end.x <<
                   " ye (tobecalculated):" <<
                   arcvec.at(i).end.y <<
                   " ze (tobecalculated):" <<
                   arcvec.at(i).end.z <<
                   " determinant (tobecalculated):" <<
                   arcvec.at(i).determinant <<
                   std::endl;
    }

    std::cout<<"circlevec"<<std::endl;
    for(unsigned int i=0; i<circlevec.size(); i++){
        std::cout<<"i:" <<
                   i <<
                   " xc:" <<
                   circlevec.at(i).center.x <<
                   " yc:" <<
                   circlevec.at(i).center.y <<
                   " zc:" <<
                   circlevec.at(i).center.z <<
                   " radius:" <<
                   circlevec.at(i).radius <<
                   std::endl;
    }

    std::cout<<"splinevec"<<std::endl;
    for(unsigned int i=0; i<splinevec.size(); i++){
        for(unsigned int j=0; j<splinevec.at(i).points.size(); j++){
            std::cout<<"i:" <<
                       i <<
                       " x:" <<
                       splinevec.at(i).points.at(j).x <<
                       " y:" <<
                       splinevec.at(i).points.at(j).y  <<
                       " z:" <<
                       splinevec.at(i).points.at(j).z  <<
                       std::endl;
        }
    }

    std::cout<<"ellipsevec"<<std::endl;
    for(unsigned int i=0; i<ellipsevec.size(); i++){
        std::cout<<"i:" <<
                   i <<
                   " xc:" <<
                   ellipsevec.at(i).center.x <<
                   " yc:" <<
                   ellipsevec.at(i).center.y <<
                   " zc:" <<
                   ellipsevec.at(i).center.z <<
                   " xmayor:" <<
                   ellipsevec.at(i).mayor.x <<
                   " ymayor:" <<
                   ellipsevec.at(i).mayor.y <<
                   " zmayor:" <<
                   ellipsevec.at(i).mayor.z <<
                   " alpha1 (startangle):" <<
                   ellipsevec.at(i).alpha1 <<
                   " alpha2 (endangle):" <<
                   ellipsevec.at(i).alpha2 <<
                   " ratio (minor-mayor axis):" <<
                   ellipsevec.at(i).ratio <<
                   std::endl;
    }
}






















