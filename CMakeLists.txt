cmake_minimum_required(VERSION 3.5)
set(PROJECT "cam") # The name of the file excluding .c or .h
project(${PROJECT} C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set variables.
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Set includepaths for header files etc.
include_directories(${SRC_DIR})
include_directories(${SRC_DIR}/libspline)
include_directories(${SRC_DIR}/libocc)
include_directories(${SRC_DIR}/libgcode)
include_directories(${SRC_DIR}/libdxfrw)
include_directories(${SRC_DIR}/libdialog)
include_directories(${SRC_DIR}/libdata)
include_directories(${SRC_DIR}/libcontour)
include_directories(${SRC_DIR}/libcavalier)
include_directories(${SRC_DIR}/libcavalier/src)
include_directories(${SRC_DIR}/libcavalier/include/cavc)
include_directories(${SRC_DIR}/libcavalier/include/cavc/internal)
include_directories(${SRC_DIR}/icons)

# Set path to your opencascade lib.
# https://github.com/tpaviot/oce/releases/download/official-upstream-packages/opencascade-7.4.0.tgz
#
# Do the build process, and make install. Then set filepath.

include_directories(/home/user/linuxcnc/cmake/vendor/oce-upstream-V7_5_0beta/inc)
include_directories(/home/user/linuxcnc/cmake/vendor/oce-upstream-V7_5_0beta/oce-upstream-V7_5_0beta/src)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories("/usr/include/eigen3")
find_package (Eigen3 3.3 REQUIRED)

# Opencascade
find_package (OpenCASCADE REQUIRED)

SET(OpenCASCADE_LIBS
    TKGeomAlgo TKMesh TKHLR TKBO TKShHealing
    TKPrim
    TKernel TKMath TKTopAlgo TKService
    TKG2d TKG3d TKV3d TKOpenGl
    TKBRep TKXSBase TKGeomBase TKGeomAlgo
    TKXSDRAW
    TKLCAF TKXCAF TKCAF TKVCAF
    TKCDF TKBin TKBinL TKBinXCAF TKXml TKXmlL TKXmlXCAF
    # -- IGES support
    TKIGES
    # -- STEP support
    TKSTEP TKXDESTEP TKXDEIGES TKSTEPAttr TKSTEPBase TKSTEP209
    # -- STL support
    TKSTL
    # -- OBJ/glTF support
    TKRWMesh TKMeshVS
    # -- VRML support
    TKVRML
    # -- ViewerTest
    TKViewerTest
    )

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PREFIX_PATH "/opt/qt-creator/5.15.1/gcc_64/lib/cmake")
include_directories("/opt/qt-creator/5.15.1/gcc_64/include/QtWidgets")
include_directories("/opt/qt-creator/5.15.1/gcc_64/include/QtGui")
include_directories("/opt/qt-creator/5.15.1/gcc_64/include")
include_directories("/opt/qt-creator/5.15.1/gcc_64/include/QtCore")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets OpenGL REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets OpenGL REQUIRED)

set(PROJECT_SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/mainwindow.cpp
    ${SRC_DIR}/mainwindow.h
    ${SRC_DIR}/mainwindow.ui
    ${SRC_DIR}/libcavalier/offsets.cpp
    ${SRC_DIR}/libcontour/contours.cpp
    ${SRC_DIR}/libdata/variable.cpp
    ${SRC_DIR}/libdxfrw/drw_classes.cpp
    ${SRC_DIR}/libdxfrw/drw_entities.cpp
    ${SRC_DIR}/libdxfrw/drw_header.cpp
    ${SRC_DIR}/libdxfrw/drw_objects.cpp
    ${SRC_DIR}/libdxfrw/dx_iface.cpp
    ${SRC_DIR}/libdxfrw/intern/drw_dbg.cpp
    ${SRC_DIR}/libdxfrw/intern/drw_textcodec.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgbuffer.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgreader.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgreader15.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgreader18.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgreader21.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgreader24.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgreader27.cpp
    ${SRC_DIR}/libdxfrw/intern/dwgutil.cpp
    ${SRC_DIR}/libdxfrw/intern/dxfreader.cpp
    ${SRC_DIR}/libdxfrw/intern/dxfwriter.cpp
    ${SRC_DIR}/libdxfrw/intern/rscodec.cpp
    ${SRC_DIR}/libdxfrw/libdwgr.cpp
    ${SRC_DIR}/libdxfrw/libdxfrw.cpp
    ${SRC_DIR}/libgcode/gcode.cpp
    ${SRC_DIR}/libocc/draw_primitives.cpp
    ${SRC_DIR}/libspline/bezier_spline.cpp
    ${SRC_DIR}/libspline/cubic_spline.cpp
    ${SRC_DIR}/libspline/spline.cpp
    ${SRC_DIR}/libocc/opencascade.cpp
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT}
        ${PROJECT_SOURCES}
        )
else()
    if(ANDROID)
        add_library(${PROJECT} SHARED
            ${PROJECT_SOURCES}
            )
    else()
        add_executable(${PROJECT}
            ${PROJECT_SOURCES}
            )
    endif()
endif()


# Link.
target_link_libraries(${PROJECT_NAME} PRIVATE pthread
    ${OpenCASCADE_LIBS} Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}
    )

# Make install
# install(TARGETS ${PROJECT} DESTINATION ${BUILD_DIR}/)
