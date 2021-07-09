#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <vector>

#include <dx_data.h>
#include <dx_iface.h>

#include <databucket.h>

#include <cubic_spline.h>
using namespace Eigen;

#include "opencascade.h"
#include "draw_primitives.h"
using namespace occ;

#include <QMainWindow>
#include <QTimer>


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

private slots:
    void on_toolButton_open_dxf_pressed();

    void on_toolButton_save_dxf_pressed();

    void on_toolButton_view_top_pressed();

    void on_toolButton_add_line_pressed();

    void on_toolButton_calculate_contours_pressed();

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
