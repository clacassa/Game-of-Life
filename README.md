
# GoL Lab

**Conway's Game of Life** GUI written in C++.  

## Dev notes

Last build: 2022/26/08  
Evertything is now bundled in the repo, so everything should work fine now.  
Please note that the Stability Detection feature needs a real improvement; for the moment it just counts the number  
of alive cells and compares it to the last four generations, so this only works for basic oscillators of period 5 and less.  
I have to figure out another way to do this. It false-positive gliders btw...

## Installation

- From the installer (Windows only)
  - Just follow the instructions of the wizard.

- From this repo (zip file)
  - Extract the zip file to the directory of your choice.  
  - You have to run the executable from the installation directory, not from the bin subdirectory. Otherwise, the executable
    won't find the themes, icons and other dll files.

## Building the application

You can build GoL Lab with **make** from the installation directory.  

## Customization

You can customize the appearance of the GUI by editing the file *etc/gtk-3.0/settings.ini*.  
Just edit the value of the **gtk-theme-name** key as following:  
To give a native look to the GUI,  
- On MacOS: replace **Venta** with **MacOS**.  
- On Linux: replace **Venta** with the name of your Desktop theme.  
- On Windows: **Venta** should do the job for Windows 10, but you can try **Windows-10-Acrylic** (contrasted).  

If the native look is not you proprity, here is the full list of featured themes:  
- MacOS
- Skeuos-Green
- Venta
- Windows-10-Acrylic
- Orchis

The themes are stored in *share/themes/* and all have a dark variant.  

## Credits

- [MacOS](https://github.com/B00merang-Project/macOS)  
- [Orchis](https://github.com/vinceliuice/Orchis-theme)  
- [Skeuos-Green](https://github.com/daniruiz/skeuos-gtk)  
- [Venta](https://www.gnome-look.org/p/1386774)  
- [Windows-10-Acrylic](https://github.com/B00merang-Project/Windows-10-Acrylic)
