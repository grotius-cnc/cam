#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>

#include <variable.h>

#include <dx_data.h>
#include <dx_iface.h>

#include <cubic_spline.h>
using namespace Eigen;

#include "opencascade.h"
#include "draw_primitives.h"
using namespace occ;

#include "contours.h"
#include "offsets.h"
#include "gcode.h"

#include <QMainWindow>
#include <QTimer>

// File read operations.
#include <iostream>
#include <fstream>
#include <string>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool open_dxf_file(std::string filename);
    bool save_dxf_file(std::string filename);
    bool write_entity();

    void Update_Opencascade();
    void load_opencascade_primitives();

    void gcode_preview();
    void gcode_get_user_settings();

private slots:
    void rotate_lead_in_out();
    void generate_contours();

    void on_toolButton_open_dxf_pressed();

    void on_toolButton_save_dxf_pressed();

    void on_toolButton_view_top_pressed();

    void on_toolButton_add_line_pressed();

    void on_toolButton_stacket_page_plus_pressed();


private:
    Ui::MainWindow *ui;
    
    // Dxf data
    dx_data fData;
    dx_data fCleanData; // To clean the fData.
    dx_iface *iface = new dx_iface;
    Opencascade* OpencascadeWidget;
};
#endif // MAINWINDOW_H
