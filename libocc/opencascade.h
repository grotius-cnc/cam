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



struct POINT{
    double x=0,y=0,z=0;
};

struct SEGMENT{
    std::vector<Handle(AIS_Shape)> Ais_ShapeVec={}; ///each stepfile can contain multiple shapes, we need the vector.
    gp_Trsf MyTrsf={};
};
extern SEGMENT Segment;
extern std::vector<SEGMENT> SegmentVec;

namespace occ{

class Opencascade: public QGLWidget
{
    Q_OBJECT
public:
    explicit Opencascade(QWidget *parent = nullptr);
    void Readstepfile(const std::string& theStepName);
    void Visit(const TDF_Label& theLabel);

    void load_cube();
    void Redraw();
    void clear();
    void fit_all();

    void draw_point(gp_Pnt point);
    void draw_line(gp_Pnt point1, gp_Pnt point2);
    void draw_3p_arc(gp_Pnt point1, gp_Pnt point2, gp_Pnt point3);
    void draw_circle(gp_Pnt center,double radius);
    void draw_cp_arc(gp_Pnt center, gp_Pnt point1, gp_Pnt point2);
    void draw_acad_arc(gp_Pnt center, double radius, const Standard_Real alpha1, const Standard_Real alpha2);

    void draw_ellipse(gp_Pnt center, gp_Pnt secpoint, double alpha_start, double alpha_end, double ratio);
    void write_text(std::string str, int textheight, double x, double y, double rot_deg);

    void set_orthographic();
    void set_perspective();
    void set_view_front();
    void set_view_back();
    void set_view_left();
    void set_view_right();
    void set_view_top();
    void set_view_bottom();

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

signals:

public slots:
};
}

#endif // OPENCASCADE_H


