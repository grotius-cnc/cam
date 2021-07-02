#include "opencascade.h"

#include <OSD.hxx>
#include <AIS_Trihedron.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Selection.hxx>
#include <AIS_ColoredShape.hxx>
#include <AIS_ColoredDrawer.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <STEPControl_Writer.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <gce_MakeRotation.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>
#include <TDF_Label.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_LabelSequence.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopTools.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include "Geom_Axis2Placement.hxx"
#include <GCE2d_MakeSegment.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <XCAFDoc_Area.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_Location.hxx>
#include <XCAFDoc_Volume.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFApp_Application.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>

#include "XCAFPrs_DocumentExplorer.hxx"
#include <TDataStd_Name.hxx>
#include <XCAFDoc_AssemblyItemId.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>

#include <BRepTools.hxx>
#include <Font_BRepFont.hxx>
#include <Font_BRepTextBuilder.hxx>
#include <Bnd_Box.hxx>

//! Make conversion's easy:
#define toRadians M_PI/180.0
#define toDegrees (180.0/M_PI)

using namespace occ;

//strore the data from the stepfile.
std::vector<TopoDS_Shape> Shape;
std::vector<Handle(AIS_Shape)> ais_Shape;
std::vector<Quantity_Color> colv;

SEGMENT Segment;
std::vector<SEGMENT> SegmentVec;

Handle(XCAFDoc_ColorTool) aColorTool;
Handle(XCAFDoc_ShapeTool) aShapeTool;

Opencascade::Opencascade(QWidget *parent) : QGLWidget(parent)
{
    setBackgroundRole( QPalette::NoRole );
    setMouseTracking( true );
}

void Opencascade::Readstepfile(const std::string& theStepName)
{
    SegmentVec.push_back(Segment);

    STEPCAFControl_Controller::Init();

    Handle(TDocStd_Document) aDoc;
    Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
    anApp->NewDocument("MDTV-XCAF", aDoc);

    STEPCAFControl_Reader aStepReader;
    aStepReader.SetColorMode(true);
    aStepReader.SetNameMode(true);
    aStepReader.SetLayerMode(true);
    aStepReader.SetPropsMode(true);

    aStepReader.ReadFile (theStepName.c_str());
    aStepReader.Transfer (aDoc);

    TDF_Label aRootLabel = aDoc->Main();

    aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aRootLabel);
    aColorTool = XCAFDoc_DocumentTool::ColorTool(aRootLabel);

    Visit(aRootLabel);

    m_view->FitAll();
}

void Opencascade::Visit(const TDF_Label& theLabel)
{
    //theLabel.EntryDump(std::cout);
    Quantity_Color aColor;
    TopoDS_Shape aShape;

    Handle(TDataStd_Name) aName;
    if (theLabel.FindAttribute(TDataStd_Name::GetID(), aName))
    {
        //std::cout << "  Name: " << aName->Get() << std::endl;
    }

    bool skip=0;
    if (aShapeTool->IsShape(theLabel))
    {
        //TopoDS_Shape aShape;
        //if a shape can be made from the current label, look for the color of that label:
        if(aShapeTool->GetShape(theLabel, aShape)){
            //std::cout<<"made a shape"<<std::endl;

            if(aColorTool->GetColor(aShape,XCAFDoc_ColorSurf,aColor)){
                skip=0;
            } else { skip=1;}

        }

        if(skip==0){

            SegmentVec.back().Ais_ShapeVec.push_back(new AIS_Shape(aShape));
            //if(aColor.Name()!=Quantity_NOC_YELLOW && aColor.Name()!=Quantity_NOC_RED){
            SegmentVec.back().Ais_ShapeVec.back()->SetColor(aColor);
            //}
            SegmentVec.back().Ais_ShapeVec.back()->SetDisplayMode(AIS_Shaded);
            SegmentVec.back().Ais_ShapeVec.back()->Attributes()->SetFaceBoundaryDraw(true);
            SegmentVec.back().Ais_ShapeVec.back()->Attributes()->SetFaceBoundaryAspect(
                        new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.));
            SegmentVec.back().Ais_ShapeVec.back()->Attributes()->SetIsoOnTriangulation(true);
            m_context->Display(SegmentVec.back().Ais_ShapeVec.back(),Standard_False);

        }
    }

    /*! Repeat the visit function for each childmember. */
    for (TDF_ChildIterator c(theLabel); c.More(); c.Next())
    {
        Visit(c.Value());
    }
}

void Opencascade::Redraw(){
    //m_context->CurrentViewer()->Redraw();
    m_view->Redraw();
}

void Opencascade::m_initialize_context()
{
    if (m_context.IsNull()){
        Handle(Aspect_DisplayConnection) m_display_donnection = new Aspect_DisplayConnection();

        if (m_graphic_driver.IsNull())
        {
            m_graphic_driver = new OpenGl_GraphicDriver(m_display_donnection);
        }

        WId window_handle = (WId) winId();
#ifdef _WIN32
        Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle) window_handle);
#else
        Handle(Xw_Window) wind = new Xw_Window(m_display_donnection, (Window) window_handle);
#endif
        m_viewer = new V3d_Viewer(m_graphic_driver);

        m_view = m_viewer->CreateView();
        m_view->SetWindow(wind);

        /*! Set the camera in perspective mode */
        m_view->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Perspective);

        if (!wind->IsMapped())
        {
            wind->Map();
        }
        m_context = new AIS_InteractiveContext(m_viewer);

        m_viewer->SetDefaultLights();
        m_viewer->SetLightOn();

        Quantity_Color cola,colb;
        cola.SetValues(0.3,0.3,0.3,Quantity_TOC_RGB);
        colb.SetValues(0.6,0.6,0.6,Quantity_TOC_RGB);
        m_view->SetBgGradientColors(cola,colb,Aspect_GFM_DIAG2 , false);

        /// View as wireframe or shaded
        //m_context->SetDisplayMode(AIS_WireFrame, Standard_False);
        m_context->SetDisplayMode(AIS_Shaded, Standard_False);

        Handle(Prs3d_Drawer) t_hilight_style = m_context->HighlightStyle();
        t_hilight_style->SetMethod(Aspect_TOHM_COLOR);
        t_hilight_style->SetColor(Quantity_NOC_LIGHTYELLOW);
        t_hilight_style->SetDisplayMode(1);
        t_hilight_style->SetTransparency(0.2f);

        Handle(Prs3d_Drawer) t_select_style = m_context->SelectionStyle();
        t_select_style->SetMethod(Aspect_TOHM_COLOR);
        t_select_style->SetColor(Quantity_NOC_LIGHTSEAGREEN);
        t_select_style->SetDisplayMode(1);
        t_select_style->SetTransparency(0.4f);

        /// Show grid
        m_viewer->SetRectangularGridValues(0,0,1,1,0);
        m_viewer->SetRectangularGridGraphicValues(2.01,2.01,0);
        m_viewer->ActivateGrid(Aspect_GT_Rectangular,Aspect_GDM_Lines);

        /// Show triedron. This is the 3d axis cross at the lower left of the screen.
        m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);

        /// Set background homogenius, one color.
        //  m_view->SetBackgroundColor(Quantity_NOC_GRAY49);
        //  m_viewer->SetDefaultShadingModel(Graphic3d_TypeOfShadingModel::V3d_COLOR);

        /// Set background with gradient stylesheet
        //  Gradient sheme's for SetBgGradientColors:
        //  Aspect_GFM_CORNER1
        //  Aspect_GFM_CORNER2
        //  Aspect_GFM_CORNER3
        //  Aspect_GFM_CORNER4
        //  Aspect_GFM_DIAG1
        //  Aspect_GFM_DIAG2
        //  Aspect_GFM_HOR
        //  Aspect_GFM_NONE
        //  Aspect_GFM_VER

        /*
        /// Show triedron at a specific place on the screen
        axis = new Geom_Axis2Placement(gp::XOY());
        aisTrihedron = new AIS_Trihedron(axis);
        aisTrihedron->SetDatumDisplayMode(Prs3d_DM_WireFrame);
        aisTrihedron->SetDrawArrows(true);
        aisTrihedron->Attributes()->DatumAspect()->LineAspect(Prs3d_DP_XAxis)->SetWidth(2.5);
        aisTrihedron->Attributes()->DatumAspect()->LineAspect(Prs3d_DP_YAxis)->SetWidth(2.5);
        aisTrihedron->Attributes()->DatumAspect()->LineAspect(Prs3d_DP_ZAxis)->SetWidth(2.5);
        aisTrihedron->SetDatumPartColor(Prs3d_DP_XAxis, Quantity_NOC_RED2);
        aisTrihedron->SetDatumPartColor(Prs3d_DP_YAxis, Quantity_NOC_GREEN2);
        aisTrihedron->SetDatumPartColor(Prs3d_DP_ZAxis, Quantity_NOC_BLUE2);
        aisTrihedron->SetLabel(Prs3d_DP_XAxis, "");
        aisTrihedron->SetLabel(Prs3d_DP_YAxis, "");
        aisTrihedron->SetLabel(Prs3d_DP_ZAxis, "");
        //aisTrihedron->SetTextColor(Quantity_NOC_GRAY40);
        aisTrihedron->SetSize(60);
        aisTrihedron->SetTransformPersistence(
                    new Graphic3d_TransformPers(Graphic3d_TMF_ZoomPers, axis->Ax2().Location()));
        aisTrihedron->Attributes()->SetZLayer(Graphic3d_ZLayerId_Topmost);
        aisTrihedron->SetInfiniteState(true);
        m_context->Display(aisTrihedron,Standard_False);
        */

        m_view->MustBeResized();
        m_view->SetProj( V3d_Zpos );
        load_cube();
    }
}

void Opencascade::paintEvent(QPaintEvent *)
{
    if (m_context.IsNull())
    {
        m_initialize_context();
    }
    m_view->Redraw();
}

void Opencascade::resizeEvent(QResizeEvent *)
{
    if( !m_view.IsNull() )
    {
        m_view->MustBeResized();
    }
}

void Opencascade::mousePressEvent(QMouseEvent *event)
{
    if((event->buttons()&Qt::LeftButton) && (event->buttons()&Qt::RightButton))
    {
        m_x_max=event->x();
        m_y_max=event->y();
    }
    else if(event->buttons()&Qt::LeftButton)
    {
        m_context->MoveTo(event->pos().x(),event->pos().y(),m_view,Standard_True);

        AIS_StatusOfPick t_pick_status = AIS_SOP_NothingSelected;
        if(qApp->keyboardModifiers()==Qt::ControlModifier)
        {
            t_pick_status = m_context->ShiftSelect(true);
        }
        else
        {
            t_pick_status = m_context->Select(true);
        }
    }
    else if(event->buttons()&Qt::MidButton)
    {
        m_x_max=event->x();
        m_y_max=event->y();
        m_view->StartRotation(event->x(),event->y());
    }
}

void Opencascade::mouseReleaseEvent(QMouseEvent *event)
{
    m_context->MoveTo(event->pos().x(),event->pos().y(),m_view,Standard_True);
}

void Opencascade::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons()&Qt::LeftButton) && (event->buttons()&Qt::RightButton))
    {
        m_view->Pan(event->pos().x()-m_x_max,m_y_max-event->pos().y());
        m_x_max=event->x();
        m_y_max=event->y();
    }
    else if(event->buttons()&Qt::MidButton)
    {
        if(qApp->keyboardModifiers()==Qt::ShiftModifier)
        {
            m_view->Pan(event->pos().x()-m_x_max,m_y_max-event->pos().y());
            m_x_max=event->x();
            m_y_max=event->y();
        }
        else
        {
            m_view->Rotation(event->x(),event->y());
        }
    }
    else
    {
        m_context->MoveTo(event->pos().x(),event->pos().y(),m_view,Standard_True);
    }
}

void Opencascade::wheelEvent(QWheelEvent *event)
{
    m_view->StartZoomAtPoint(event->position().x(),event->position().y());
    m_view->ZoomAtPoint(0, 0, event->angleDelta().y(), 0);
}

// Empty screen
void Opencascade::clear()
{
    m_context->EraseAll(0);
    m_view->FitAll();
    m_view->Redraw();

    // Load the cube again :
    load_cube();
}

void Opencascade::load_cube(){
    // This is the interactivve 3d box
    opencascade::handle<AIS_ViewCube> aisViewCube = new AIS_ViewCube;
    aisViewCube->SetBoxColor(Quantity_NOC_GRAY75);
    //aisViewCube->SetFixedAnimationLoop(false);
    aisViewCube->SetDrawAxes(false);
    aisViewCube->SetSize(55);
    aisViewCube->SetFontHeight(12);
    aisViewCube->SetTransformPersistence(
                new Graphic3d_TransformPers(
                    Graphic3d_TMF_TriedronPers,
                    Aspect_TOTP_LEFT_UPPER,
                    Graphic3d_Vec2i(85, 85)));
    m_context->Display(aisViewCube, false);


    //aisViewCube->Attributes()->DatumAspect()->LineAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
    //const Handle_Prs3d_DatumAspect& datumAspect = aisViewCube->Attributes()->DatumAspect();
    //datumAspect->ShadingAspect(Prs3d_DP_XAxis)->SetColor(Quantity_NOC_RED2);
    //datumAspect->ShadingAspect(Prs3d_DP_YAxis)->SetColor(Quantity_NOC_GREEN2);
    //datumAspect->ShadingAspect(Prs3d_DP_ZAxis)->SetColor(Quantity_NOC_BLUE2);
    //m_aisViewCube = aisViewCube;
}

void Opencascade::fit_all(){
     m_view->FitAll();
}

// View options
void Opencascade::set_orthographic(){
    m_view->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
    m_view->Update();
}

void Opencascade::set_perspective(){
    m_view->Camera()->SetProjectionType (Graphic3d_Camera::Projection_Perspective);
    m_view->Redraw();
    //m_view->Update();
}

void Opencascade::set_view_front()
{
    m_view->SetProj( V3d_Yneg );
}

void Opencascade::set_view_back()
{
    m_view->SetProj( V3d_Ypos );
}

void Opencascade::set_view_top()
{
    m_view->SetProj( V3d_Zpos );
}

void Opencascade::set_view_bottom()
{
    m_view->SetProj( V3d_Zneg );
}

void Opencascade::set_view_left()
{
    m_view->SetProj( V3d_Xneg );
}
void Opencascade::set_view_right()
{
    m_view->SetProj( V3d_Xpos );
}


// Draw primitives:

void Opencascade::draw_point(gp_Pnt point){
    //make a point
    //gp_Pnt point1(0,0,0);

    //make a vertex from the point
    TopoDS_Vertex vertex1 = BRepBuilderAPI_MakeVertex(point);

    //display the vertex
    Handle(AIS_Shape) aisBody1 = new AIS_Shape(vertex1);
    //aisBody1->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);
    m_context->SetColor(aisBody1,Quantity_NOC_LAVENDER,Standard_False);
    m_context->SetMaterial(aisBody1,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(aisBody1,Standard_False);
}

void Opencascade::draw_line(gp_Pnt point1, gp_Pnt point2){

    //make a point
    //gp_Pnt point1(0,0,0);

    //make a vertex from the point
    TopoDS_Vertex vertex1 = BRepBuilderAPI_MakeVertex(point1);

    //make one more point
    //gp_Pnt point2(100,0,0);

    //make an edge from the two points
    TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(point1,point2);

    //display the vertex (point cross)
    //Handle(AIS_Shape) aisBody1 = new AIS_Shape(vertex1);
    //aisBody1->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);
    //m_context->SetColor(aisBody1,Quantity_NOC_LAVENDER,Standard_False);
    //m_context->SetMaterial(aisBody1,Graphic3d_NOM_PLASTIC,Standard_False);
    //m_context->Display(aisBody1,Standard_False);

    //display the edge
    Handle(AIS_Shape) aisBody2 = new AIS_Shape(edge1);

    //Transformation matrix:
    //aisBody2->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);
    m_context->SetColor(aisBody2,Quantity_NOC_BLUE4,Standard_False);
    m_context->SetMaterial(aisBody2,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(aisBody2,Standard_False);
}

void Opencascade::draw_circle(gp_Pnt center,double radius){

    //draw circle :
    gp_Dir dir(0,0,1); // you can change this
    gp_Circ circle(gp_Ax2( center, dir),radius);
    BRepBuilderAPI_MakeEdge makeEdge(circle);
    Handle(AIS_Shape) shape = new AIS_Shape(TopoDS_Edge());
    m_context->SetColor(shape,Quantity_NOC_BLUE4,Standard_False);
    m_context->SetMaterial(shape,Graphic3d_NOM_PLASTIC,Standard_False);
    shape ->Set(makeEdge.Edge());

    //Transformation matrix:
    //shape->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);
    m_context->Display(shape , 1);
}

#include "gp_Elips.hxx"
#include <GC_MakeArcOfEllipse.hxx>
void Opencascade::draw_ellipse(gp_Pnt center, gp_Pnt secpoint, double alpha_start, double alpha_end, double ratio){

    //https://github.com/grotius-cnc/QT_CadCam_rev0/blob/master/display/display.h
    // Acad's ellipse nr's, https://github.com/grotius-cnc/cadcam/blob/master/dxf/read_ellipse_AC1027.cpp
    // x,y,z centerpoint    10,20,30
    // x,y,z endpoint mayor 11,21,31 ( coordinates relative to ellipse centerpoint..)
    // ratio                40
    // start angle          41
    // end angle            42

    //Standard_Real alpha1=alpha_start;
    //Standard_Real alpha2=alpha_end;

    //center point to endpoint mayor axis..
    double MayorRadius = sqrt(pow(secpoint.X()-center.X(),2) + pow(secpoint.Y()-center.Y(),2) + pow(secpoint.Z()-center.Z(),2));
    //ratio minor axis to mayor axis..
    double MinorRadius = ratio*MayorRadius ;

    gp_Dir xDirection(secpoint.X()-center.X(),secpoint.Y()-center.Y(),secpoint.Z()-center.Z()); // Direction is auto normalized by occ.
    gp_Dir normalDirection(0,0,1);
    gp_Ax2 axis(center,normalDirection,xDirection);

    gp_Elips ellipse(axis,MayorRadius,MinorRadius);

    Handle(Geom_TrimmedCurve) aArcOfEllipse = GC_MakeArcOfEllipse(ellipse,alpha_start,alpha_end,0);

    TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aArcOfEllipse);

    //display the edge
    Handle(AIS_Shape) aisBody = new AIS_Shape(aEdge);

    //Transformation matrix:
    //aisBody2->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);

    m_context->SetColor(aisBody,Quantity_NOC_BLUE4,Standard_False);
    m_context->SetMaterial(aisBody,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(aisBody,Standard_False);
}

void Opencascade::draw_3p_arc(gp_Pnt point1,gp_Pnt point2,gp_Pnt point3){

    //gp_Pnt point1(0,0,0);
    //gp_Pnt point2(50,50,0);
    //gp_Pnt point3(100,0,0);
    Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(point1,point2,point3);

    TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);

    //display the edge
    Handle(AIS_Shape) aisBody2 = new AIS_Shape(aEdge2);

    //Transformation matrix:
    //aisBody2->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);

    m_context->SetColor(aisBody2,Quantity_NOC_BLUE4,Standard_False);
    m_context->SetMaterial(aisBody2,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(aisBody2,Standard_False);
}

void Opencascade::draw_cp_arc(gp_Pnt center, gp_Pnt point1, gp_Pnt point2){

    double radius=center.Distance(point2);
    //std::cout<<"radius:"<<radius<<std::endl;
    gp_Dir dir(0,0,1); // you can change this
    gp_Circ circle(gp_Ax2( center, dir),radius);

    Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(circle,point1,point2,0);

    TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);

    //display the edge
    Handle(AIS_Shape) aisBody2 = new AIS_Shape(aEdge2);

    //Transformation matrix:
    //aisBody2->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);

    m_context->SetColor(aisBody2,Quantity_NOC_BLUE4,Standard_False);
    m_context->SetMaterial(aisBody2,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(aisBody2,Standard_False);
}

void Opencascade::draw_acad_arc(gp_Pnt center, double radius, const Standard_Real alpha1, const Standard_Real alpha2){

    //std::cout<<"radius:"<<radius<<std::endl;
    gp_Dir dir(0,0,1); // you can change this
    gp_Circ circle(gp_Ax2( center, dir),radius);

    Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(circle,alpha1,alpha2,0);

    TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);

    //display the edge
    Handle(AIS_Shape) aisBody2 = new AIS_Shape(aEdge2);

    //Transformation matrix:
    //aisBody2->SetLocalTransformation(ChainVec.back().WorkFrameVec.back().MyTrsf);

    m_context->SetColor(aisBody2,Quantity_NOC_BLUE4,Standard_False);
    m_context->SetMaterial(aisBody2,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(aisBody2,Standard_False);
}

void Opencascade::write_text(std::string str, int textheight, double x, double y, double rot_deg){ //https://www.youtube.com/watch?v=31SXQLpdNyE

    m_context->EraseAll(0);

    const char *chartext=str.c_str();

    if(textheight==0 || textheight<0){textheight=1;}

    Font_BRepFont aBrepFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", textheight);
    Font_BRepTextBuilder aTextBuilder;
    TopoDS_Shape Text_Shape = aTextBuilder.Perform(aBrepFont, NCollection_String(chartext));

    Handle(AIS_Shape) Text_ais_Shape = new AIS_Shape(Text_Shape);

    gp_Trsf MyTrsf_rot;
    MyTrsf_rot.SetRotation(gp_Ax1(gp_Pnt(
                                      0,
                                      0,
                                      0), gp_Dir(
                                      0,                         //rotation flag x
                                      0,                         //rotation flag y
                                      1)), rot_deg * M_PI /180);
    gp_Trsf MyTrsf_trans;
    MyTrsf_trans.SetTranslation(gp_Pnt(0,0,0),gp_Pnt(x,y,0));
    Text_ais_Shape->SetLocalTransformation(MyTrsf_trans*MyTrsf_rot);

    m_context->SetColor(Text_ais_Shape,Quantity_NOC_BLACK,Standard_False);
    //m_context->SetMaterial(aisBody2,Graphic3d_NOM_PLASTIC,Standard_False);
    m_context->Display(Text_ais_Shape,Standard_True);

    //set_view_top();
    //m_view->ZFitAll();
    //m_view->FitAll();
    //m_view->Redraw();
}











