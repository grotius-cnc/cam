The Licence includes : This source code or any of it's contents is prohibited to be used in the following "black listed" countries, that violates human rights.

Black listed countries:
1. France
2. Greece
3. United Kingdom
4. __

The Licence includes : This source code or any of it's contents is prohibited to be used by the following "black listed companies that violates human rights.

Black listed companies:
1. Google
2. Youtube
3. Twitter

![joco-power](https://user-images.githubusercontent.com/44880102/126195312-21ad8ad2-9934-4f9a-8ca5-feb9db597b5f.jpg)
![joco-power1](https://user-images.githubusercontent.com/44880102/126195320-89a89308-8b0b-46ca-905c-5ea7eb64b7d5.jpg)
![joco-power](https://user-images.githubusercontent.com/44880102/126424607-9ac7d72e-8f70-4704-9522-bae9012943e3.jpg)


# QT_dxfrw
Qt project example of the dxfrw c++ library

This is the same dxfrw lib used in librecad, freecad etc.

This example show's how to use the dxfrw library and display the dxf content with the opencascade widget.

Depends : Qt + opencascade library

Currently implemented:

- Dxf read, write and save example.
- Opencascade cad view, selecting base contours by mouse click.
- Supports offset by cad-layer.
- Supports contour offset values: -, + or 0 (no offset).
- Depth sequence, "keep parts together" kpt output algorimte.
- Lead-in, lead-out for offsets.
- Supports single-open, multiple-open, single-closed and multiple-closed primitive offsets.
- Generates a gcode.ngc output file located in build directory.
- Can add line-numbers to the gcode.ngc file.
- Supports pockets with or without islands. Base offset + Internal pocket offset.

Todo:

- Show rapids.
- Create a .deb installer script for this app.

Gcode output tested:
![gcode](https://user-images.githubusercontent.com/44880102/125856968-7a27359c-bccd-45f4-8186-6dae3863da6f.jpg)

