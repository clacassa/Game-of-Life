
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

| Action  | Shortcut | Description |  
|  :--    |   :--    |    :--      |
| New     | Ctrl + N | Resets everything with a new blank simulation (closes any opened file) |  
| Open    | Ctrl + O | Opens a text file to configure the simulation |
| Save    | Ctrl + S | Saves the simulation |
| Save as | Ctrl + Shift + S | Renames the opend file or creates a new file, and saves the simulation |
| Quit    | Ctrl + Q | Quits instantly the application without saving |
| Cut     | Ctrl + X | Cuts the selection in the clipboard |
| Copy    | Ctrl + C | Copies the selection in the clipboard |
| Clear   | Del/Suppr | Clears the selection ("kills" the live cells in the selection rectangle) |
| Paste   | Ctrl + V | Pastes the content of the clipboard |
| Select all | Ctrl + A | Selects the whole board |
| Random Fill | Ctrl + R | Fills 50% of the board randomly |
| Rotate 180° | | Rotates the selection counter-clockwise by 180° |
| Flip vertically | Y | Flips the inserted pattern or the pasted selection vertically |
| Flip horizontally | X | Same as above but horizontally |
| Cursor Mode > Draw | F2 | Switches to drawing mode |
| Cursor Mode > Drag | F3 | Switches to drag mode |
| Cursor Mode > Select | F4 | Switches to selection mode |
| Zoom in | Ctrl + + / Mouse wheel up | Zooms in by 10% |
| Zoom out | Ctrl + - / Mouse wheel down | Zooms out by 10% |
| Reset zoom | Ctrl + 0 | Restores the zoom to its default value |
| Show grid | | Toggles the display of the grid |
| Fade effect | | Toggles an effect in which the cells fade when they die (decreases the fps) |
| Dark theme | Ctrl + D | Toggles light/dark theme |
| Color scheme | | Allows you to change the color scheme independantly for light/dark mode |
| Stability detection | Ctrl + T | Toggles the detection of stability : when enabled, the simulation stops as soon as it stabilizes |
| World size > Increase | Ctrl + PageUp | Increases the world dimensions by 100 cells |
| World size > Decrease | Ctrl + PageDown | Decreases the world dimensions by 100 cells |
| Insert pattern | Ctrl + P | Inserts a pattern among the ones stored in `patterns/` |
| Help | F1 | Shows a help message similar to this brief documentation |
| About GoL Lab | | Shows information relative to GoL Lab, such as the build, the logo, the license and the author |

## Customization

You can customize the appearance of GoL Lab by editing the file `etc/gtk-3.0/settings.ini`.  
Note that as the application is built with GTK, Linux users might not be concerned by this section, as their system theme can override GoL Lab settings.  
  
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
