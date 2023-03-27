
## About GoL Lab

GoL Lab is a program to explore the famous cellular automaton Game of Life (GoL) created by the mathematician John Conway in the late 60's. It features view controls, editing and basic simulation tools, such as stability detection and RLE file format parser. GoL Lab is a free software, licensed under the GPL and written in C++.  

## Dev notes

Version in development: 0.3.0 (Soon 1.0.0 !)  
Please note that the Stability Detection feature needs some improvement, as it currently memorizes the last five generations and if a repeating pattern is found, stops the simulation.  

## Installation (Windows only)

* From the installer (Build 0.2.1, August 2022);  
  - Download [the installer](https://github.com/clacassa/GoL-Lab/releases).  
  - Just follow the installation instructions.  

## Building the application

GoL Lab uses gtkmm-3.0.    
Unfortunately, this library is huge and not easy to distribute. Until I figure how to properly do it, you will need to install it on your system:  

#### MacOS  

    brew install gtkmm3  
    
#### Debian/Ubuntu Linux  

    apt-get install libgtkmm-3.0-dev  
    
#### Fedora/RedHat Linux  

    yum install gtkmm3.0-devel  
    
You can then download and exctract the source code, and build GoL Lab with `make` from your installation directory.  

## Features  

The followging features are currently implemented in GoL Lab :  

| Action | Shortcut | Description |  
| :--    | :--      | :--         |
| New    | Ctrl + N | Resets everything with a new blank simulation (close any opened file)
| Open   | Ctrl + V |
| Save

## Customization

You can customize the appearance of GoL Lab by editing the file `etc/gtk-3.0/settings.ini`.  
Note that as the application is built with GTK, Linux users who have a GNOME desktop environment are not concerned by this section, as their GTK system theme will override GoL Lab settings.  
  
Just edit the value of the `gtk-theme-name` key as following: 
  
To give a native look to the GUI,  
- On MacOS: replace `Venta` with `MacOS`.  
- On Windows:  `Venta` should do the job, but you can try the built-in Windows theme `win32`. Note however that some ui elements are missing and that Dark mode is not available.   

If the native look is not you proprity, feel free to try out these good loocking themes:  
- Skeuos-Green
- Shades-of-gray  (really nice, but comes with no Light mode)  

The themes are stored in `share/themes`.  

## Todo

## Credits

- [MacOS](https://github.com/B00merang-Project/macOS)  
- [Skeuos-Green](https://github.com/daniruiz/skeuos-gtk)  
- [Venta](https://www.gnome-look.org/p/1386774)  
