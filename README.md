
# About GoL Lab

GoL Lab is a program that lets you explore the famous cellular automaton Game of Life (GoL) created by the mathematician John Conway in the late 60's. It features view controls, editing and basic simulation tools, such as stability detection and RLE file format parser. GoL Lab is a free software, licensed under the GPL and written in C++.  

## Dev notes

Last commit: Mar 11, 2023  
Please note that the Stability Detection feature needs some improvement, as it currently  
memorizes the last five generations and if a repeating pattern is found, stops the simulation  

## Installation

* From the installer (Windows only)  
  - Download [the installer](https://github.com/clacassa/GoL-Lab/releases).  
  - Just follow the installation instructions.  

## Building the application

GoL Lab uses gtkmm v3.0 as GUI toolkit.  
Unfortunately, gtkmm3 is not easy to distribute. Until I figure how to properly do it, you  
will need to install it on your system:  

#### MacOS  

    brew install gtkmm3  
    
#### Debian/Ubuntu Linux  

    apt-get install libgtkmm-3.0-dev  
    
#### Fedora/RedHat Linux 

    yum install gtkmm3.0-devel  
    
You can then build GoL Lab from the github repository: https://github.com/clacassa/GoL-Lab  
with `make` from your installation directory.

## Customization

You can customize the appearance of GoL Lab by editing the file *etc/gtk-3.0/settings.ini*.  
Note that as the application is built with GTK, Linux users who have a GNOME desktop environment are not concerned by this section, as their GTK system theme will override GoL Lab settings.   
Just edit the value of the `gtk-theme-name` key as following: 
  
To give a native look to the GUI,  
- On MacOS: replace `Venta` with `MacOS`.  
- On Windows: `Venta` should do the job for Windows 10, but you can try `Windows-10-Acrylic`  

If the native look is not you proprity, feel free to try out these good loocking themes:  
- Skeuos-Green
- Orchis

The themes are stored in *share/themes/* and all have a dark variant.  

## Credits

- [MacOS](https://github.com/B00merang-Project/macOS)  
- [Orchis](https://github.com/vinceliuice/Orchis-theme)  
- [Skeuos-Green](https://github.com/daniruiz/skeuos-gtk)  
- [Venta](https://www.gnome-look.org/p/1386774)  
- [Windows-10-Acrylic](https://github.com/B00merang-Project/Windows-10-Acrylic)
