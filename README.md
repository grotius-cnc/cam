
![joco-power](https://user-images.githubusercontent.com/44880102/125855980-06be7e1d-4694-434a-9fe6-2458c3e0da6e.jpg)

# QT_dxfrw
Qt project example of the dxfrw c++ library

This is the same dxfrw lib used in librecad, freecad etc.

This example show's how to use the dxfrw library and display the dxf content with the opencascade widget.

Depends : Qt + opencascade library

Currently implemented:

- Dxf read.
- Opencascade cad view.
- Contour offsets cw, ccw.
- Depth sequence, cut sequence. (thermal distortion algoritme).
- Lead-in, lead-out.
- Generates a gcode.ngc output file located in build directory.

Gcode output tested:
![gcode](https://user-images.githubusercontent.com/44880102/125856968-7a27359c-bccd-45f4-8186-6dae3863da6f.jpg)

