#ifndef OPENCASCADE_H
#define OPENCASCADE_H

#include <QWidget>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QApplication>

#include <AIS_InteractiveContext.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_View.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>

#include <QGLWidget>
#ifdef _WIN32
#include <WNT_Window.hxx>
#else
#undef None
#include <Xw_Window.hxx>
#endif

#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <AIS_Shape.hxx>
#include <TDF_Label.hxx>

//show xyz axis
#include <Geom_Axis2Placement.hxx>
#include <AIS_Trihedron.hxx>

#include <draw_primitives.h>

#define gp_Euler gp_Pnt // Used as universal toolset.



struct POINT{
    double x=0,y=0,z=0;
};

struct SEGMENT{
    std::vector<Handle(AIS_Shape)> Ais_ShapeVec={}; ///each stepfile can contain multiple shapes, we need the vector.
    gp_Trsf MyTrsf={};
};
extern SEGMENT Segment;
extern std::vector<SEGMENT> SegmentVec;

extern gp_Trsf level0x1x2x3x4x5x6;

enum primitive_type {
    point=0,
    line=1,
    lwpolyline=2, // Wire, linestrip
    circle=3,
    arc=4,
    spline=5,
    ellipse=6,
};

enum contour_type {
    none=0,
    single_open=1,
    single_closed=2,
    multi_open=3,
    multi_closed=4,
};

struct data {
    Handle(AIS_Shape) ashape;
    primitive_type primitivetype;
    contour_type contourtype=contour_type::none;
    //! Startpoint, endpoint
    gp_Pnt start{0,0,0}, end{0,0,0};
    std::vector<gp_Pnt> control;
    bool select=0;
};
extern std::vector<data> datavec;

struct contour {
    std::vector<data> primitive_sequence;
};
extern std::vector<contour> contourvec;

namespace occ {
class Opencascade: public QGLWidget
{
    Q_OBJECT
public:
    explicit Opencascade(QWidget *parent = nullptr);

    bool Readstepfile(const std::string& theStepName);
    void Visit(const TDF_Label& theLabel);
    void Init_robot();
    void setup_tcp_origin();

    void show_shape(Handle(AIS_Shape) shape);
    void Redraw();
    void update_jointpos(double j0, double j1, double j2, double j3, double j4, double j5);

    // View
    void set_orthographic();
    void set_perspective();
    void set_view_front();
    void set_view_back();
    void set_view_left();
    void set_view_right();
    void set_view_top();
    void set_view_bottom();

    // Zoom
    void zoom_all();

    // Selection
    void get_selections();
    void delete_selections();

    // Erase
    void erase_all();

private:
    void m_initialize_context();
    Handle(AIS_InteractiveContext) m_context;
    Handle(V3d_Viewer) m_viewer;
    Handle(V3d_View) m_view;
    Handle(Graphic3d_GraphicDriver) m_graphic_driver;
    Handle(AIS_InteractiveObject) m_aisViewCube;

    // Xyz axis sign.
    Handle(Geom_Axis2Placement) axis;
    Handle(AIS_Trihedron) aisTrihedron;
    std::vector<Handle(AIS_Trihedron)> aisTrihedrons;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

protected:
    enum CurrentAction3d
    {
        CurAction3d_Nothing,
        CurAction3d_DynamicPanning,
        CurAction3d_DynamicZooming,
        CurAction3d_DynamicRotation
    };

private:
    Standard_Integer m_x_max;
    Standard_Integer m_y_max;
    CurrentAction3d m_current_mode;
    //gp_Trsf current_tcp;

    Handle(AIS_Shape) aisBody_tcp_xaxis, aisBody_tcp_yaxis, aisBody_tcp_zaxis;

    // Create the euler lines
    double toollenght=105;
    double linelenght=25;
    double coneheight=25;
    double conetopdiameter=1;
    double conebottomdiameter=5;
    double textheight=25;


    TopoDS_Edge edge_linepreview;
    Handle(AIS_Shape) aisBody_linepreview;

signals:

public slots:
};
}

#endif // OPENCASCADE_H


