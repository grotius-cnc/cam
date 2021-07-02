#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace Eigen;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // https://www.youtube.com/watch?v=2VIGb31iwds

    OpencascadeWidget = new Opencascade(this);
    ui->gridLayout_opencascade->addWidget(OpencascadeWidget);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::Update_Opencascade);
    timer->start(200);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Update_Opencascade()
{
    OpencascadeWidget->Redraw();
}

bool MainWindow::open_dxf_file(std::string filename){

    dx_iface().clear(&fData);           // Clear previous loaded dxf data.

    bool ok = iface->fileImport( filename, &fData );
    if (!ok) {
        std::cout << "Error reading file " << filename << std::endl;
    } else {
        std::cout << "Ok reading file " << filename << std::endl;

        // Process opencascade primitives.
        load_opencascade_primitives();
    }
    return ok;
}

bool MainWindow::save_dxf_file(std::string filename){

    bool ok = iface->fileExport(filename, DRW::AC1027 , false, &fData);
    if (!ok) {
        std::cout << "Error saving file " << filename << std::endl;
    } else {
        std::cout << "Ok saving file " << filename << std::endl;
    }
    return ok;
}


void MainWindow::fill_databucket(){ // Data storage is done with external variables.
                                    // These variables can be accessed if databucket.h is included.
                                    // databucket.h is a centralized data bucket.

    // First empty the data bucket. You can consider to store it in a temp bucket in case the user does a "cntr+z"
    databucket().clear();

    auto blocks = fData.mBlock->ent;
    for (auto iter = blocks.begin(); iter != blocks.end(); iter++){
        if(DRW::POINT == (*iter)->eType){
            DRW_Point *point = dynamic_cast<DRW_Point*>(*iter);
            p.x=point->basePoint.x;                                             // Databucket uses a p for point. p=external variable.
            p.y=point->basePoint.y;                                             // p "point" is used as data type like double or int.
            p.z=point->basePoint.z;
            pointvec.push_back(p);
            p.x=0; p.y=0; p.z=0;
        }
        if(DRW::LINE == (*iter)->eType){
           DRW_Line *line = dynamic_cast<DRW_Line*>(*iter);
           l.start={line->basePoint.x,line->basePoint.y,line->basePoint.z};     // Databucket uses a l for line. l=external variable.
           l.end={line->secPoint.x,line->secPoint.y,line->secPoint.z};          // Databucket has a bucket "linevec" to store all lines.
           linevec.push_back(l);                                                // Add the line to the databucket.
           l.start={0,0,0}; l.end={0,0,0};                                      // Reset external variable.
        }
        if(DRW::LWPOLYLINE == (*iter)->eType){
            DRW_LWPolyline *lwpolyline = dynamic_cast<DRW_LWPolyline*>(*iter);
            for(unsigned int i=0; i<lwpolyline->vertlist.size()-1; i++){
                l.start={lwpolyline->vertlist.at(i)->x,lwpolyline->vertlist.at(i)->y,0};    // No z in dxfrw lib.
                l.end={lwpolyline->vertlist.at(i+1)->x,lwpolyline->vertlist.at(i+1)->y,0};
                linevec.push_back(l);
            } // Close the polyline.
            l.start={lwpolyline->vertlist.front()->x,lwpolyline->vertlist.front()->y,0};
            l.end={lwpolyline->vertlist.back()->x,lwpolyline->vertlist.back()->y,0};
            linevec.push_back(l);
            l.start={0,0,0}; l.end={0,0,0};
        }
        if(DRW::SPLINE == (*iter)->eType){
            DRW_Spline *spline = dynamic_cast<DRW_Spline*>(*iter);
            for(unsigned int i=0; i<spline->controllist.size(); i++){
                s.points.push_back({spline->controllist.at(i)->x,spline->controllist.at(i)->y,spline->controllist.at(i)->z});
            }
            splinevec.push_back(s);
            s.points.clear();
        }
        if(DRW::ARC == (*iter)->eType){
            DRW_Arc *arc = dynamic_cast<DRW_Arc*>(*iter);
            a.center={arc->center().x,arc->center().y,arc->center().z};
            a.radius=arc->radius();
            a.alpha1=arc->startAngle();
            a.alpha2=arc->endAngle();
            arcvec.push_back(a);
            a.center={0,0,0}; a.radius=0; a.alpha1=0; a.alpha2=0;
        }
        if(DRW::CIRCLE == (*iter)->eType){
            DRW_Circle *circle = dynamic_cast<DRW_Circle*>(*iter);
            c.center={circle->basePoint.x,circle->basePoint.y,circle->basePoint.z};
            c.radius=circle->radious;
            circlevec.push_back(c);
            c.center={0,0,0}; c.radius=0;
        }
    }
}

bool MainWindow::write_entity(){

    std::cout<<"add line item"<<std::endl;
    DRW_Line testline;
    testline.basePoint.x = 10;
    testline.basePoint.y = 20;
    testline.secPoint.x = 30;
    testline.secPoint.y = 30;
    iface->addLine(testline);
    return 1;
}

void MainWindow::load_opencascade_primitives()
{
    OpencascadeWidget->clear();

    // Print the dxf output.
    auto blocks = fData.mBlock->ent;
    for (auto iter = blocks.begin(); iter != blocks.end(); iter++){
        if(DRW::POINT == (*iter)->eType){ std::cout<<"point"<<std::endl;

            DRW_Point *point = dynamic_cast<DRW_Point*>(*iter);
            OpencascadeWidget->draw_point({point->basePoint.x,point->basePoint.y,point->basePoint.z});
        }

        if(DRW::LINE == (*iter)->eType){

            DRW_Line *line = dynamic_cast<DRW_Line*>(*iter);
            OpencascadeWidget->draw_line({line->basePoint.x, line->basePoint.y, line->basePoint.z},{
                                             line->secPoint.x, line->secPoint.y, line->secPoint.z});
        }

        if(DRW::LWPOLYLINE == (*iter)->eType){ std::cout<<"lwpolyline"<<std::endl;

            DRW_LWPolyline *lwpolyline = dynamic_cast<DRW_LWPolyline*>(*iter);
            for(unsigned int i=0; i<lwpolyline->vertlist.size()-1; i++){
                OpencascadeWidget->draw_line({lwpolyline->vertlist.at(i)->x,lwpolyline->vertlist.at(i)->y,0}, // No z in dxfrw lib.
                                             {lwpolyline->vertlist.at(i+1)->x,lwpolyline->vertlist.at(i+1)->y,0});
            } // Close the polyline.
            OpencascadeWidget->draw_line({lwpolyline->vertlist.front()->x,lwpolyline->vertlist.front()->y,0},
                                         {lwpolyline->vertlist.back()->x,lwpolyline->vertlist.back()->y,0});

        }

        if(DRW::SPLINE == (*iter)->eType){ std::cout<<"spline"<<std::endl;

            DRW_Spline *spline = dynamic_cast<DRW_Spline*>(*iter);

            std::vector<Vector3d> path; // input
            for(unsigned int i=0; i<spline->controllist.size(); i++){
                path.push_back(Vector3d(spline->controllist.at(i)->x,
                                        spline->controllist.at(i)->y,
                                        spline->controllist.at(i)->z));
            }
            int divisions = 5; // input
            CubicSpline c_spline(path,divisions);
            c_spline.BuildSpline(path);
            std::vector<double> c_pathx(c_spline.GetPositionProfile().size());
            std::vector<double> c_pathy(c_spline.GetPositionProfile().size());
            std::vector<double> c_pathz(c_spline.GetPositionProfile().size());
            gp_Pnt a,b;

            // Get profile data for position, speed, acceleration, and curvature
            std::vector<double> ti(c_spline.GetPositionProfile().size());
            for(unsigned int i=0;i<c_pathx.size();++i)
            {
                c_pathx[i] = c_spline.GetPositionProfile()[i].x();
                c_pathy[i] = c_spline.GetPositionProfile()[i].y();
                c_pathz[i] = c_spline.GetPositionProfile()[i].z();

                if(i==0){
                    // Store first 3d point.
                    a = {c_pathx[i],c_pathy[i],c_pathz[i]};
                }
                if(i>0){
                    b = {c_pathx[i],c_pathy[i],c_pathz[i]};
                    OpencascadeWidget->draw_line(a,b);
                    a=b;
                }
            }

            //std::cout<<"spline fitlist.size:"<<spline->fitlist.size()<<std::endl;
            //std::cout<<"spline controllist.size:"<<spline->controllist.size()<<std::endl;
        }

        if(DRW::ARC == (*iter)->eType){ std::cout<<"arc"<<std::endl;

            DRW_Arc *arc = dynamic_cast<DRW_Arc*>(*iter);
            OpencascadeWidget->draw_acad_arc({arc->center().x,arc->center().y,arc->center().z}, arc->radius(),
                                             arc->startAngle(),arc->endAngle());
        }

        if(DRW::CIRCLE == (*iter)->eType){ std::cout<<"circle"<<std::endl;

            DRW_Circle *circle = dynamic_cast<DRW_Circle*>(*iter);
            OpencascadeWidget->draw_circle({circle->basePoint.x,circle->basePoint.y,circle->basePoint.z},circle->radious);
        }

        if(DRW::ELLIPSE == (*iter)->eType){ std::cout<<"ellipse"<<std::endl;

            DRW_Ellipse *ellipse = dynamic_cast<DRW_Ellipse*>(*iter);

            //https://www.youtube.com/watch?v=fKiD0sm6gTI
            OpencascadeWidget->draw_ellipse({ellipse->basePoint.x,ellipse->basePoint.y,ellipse->basePoint.z},
                                            {ellipse->secPoint.x,ellipse->secPoint.y,ellipse->secPoint.z},
                                            ellipse->staparam,
                                            ellipse->endparam,
                                            ellipse->ratio);

            // Acad's ellipse nr's, https://github.com/grotius-cnc/cadcam/blob/master/dxf/read_ellipse_AC1027.cpp
            // x,y,z centerpoint    10,20,30
            // x,y,z endpoint mayor 11,21,31 ( coordinates relative to ellipse centerpoint..)
            // ratio                40
            // start angle          41
            // end angle            42
        }

        OpencascadeWidget->fit_all(); // Zoom to extends. Top view is already set when initializing the opencascadewidget at startup.
    }
}

void MainWindow::on_toolButton_view_top_pressed()
{
    OpencascadeWidget->set_orthographic();
    OpencascadeWidget->set_view_top();
}

void MainWindow::on_toolButton_open_dxf_pressed(){

    std::string filename=ui->lineEdit_dxf_filename->text().toStdString();
    open_dxf_file(filename);    // Read the dxf, for now it's directly displayed by opencascade.
    fill_databucket();          // Store the dxf data in our databucket when the dxf is loaded.
                                // At this stage you could empty the dxf data by clearing the "fData"
                                // See mainwindow.h : dx_data fData;

    databucket().print_databucket_content();    // Show us what's inside the data bucket after the dxf is loaded.
}

void MainWindow::on_toolButton_save_dxf_pressed()
{
    std::string filename=ui->lineEdit_dxf_filename->text().toStdString();
    save_dxf_file(filename);
}

void MainWindow::on_toolButton_add_line_pressed()
{
    write_entity();
}

void MainWindow::on_toolButton_calculate_contours_pressed()
{

}

void MainWindow::on_toolButton_stacket_page_plus_pressed()
{
    if(ui->stackedWidget->currentIndex() != ui->stackedWidget->count()-1){
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+1);
    } else {
    //if(ui->stackedWidget->currentIndex() == ui->stackedWidget->count()-1){
        ui->stackedWidget->setCurrentIndex(0);
    }

    std::cout<<"curindex:"<<ui->stackedWidget->currentIndex()<<std::endl;
    //}
}











