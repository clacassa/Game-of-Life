
## About GoL Lab

GoL Lab stands for "Game of Life Laboratory" whose purpose it to explore the famous cellular automaton Game of Life (GoL) thought by the mathematician John Conway in the late 60's. It features view controls, editing and basic simulation tools, such as stability detection and RLE file format parser. GoL Lab is a free software, licensed under the GPL and written in C++. 

Version 1.0.0 09/29/2023
Copyright (C) 2022-2023 Cyprien Lacassagne   

## Installation (Windows only)

* From the installer  
  - Download and run [the installer](https://github.com/clacassa/GoL-Lab/releases).  
  - Just follow the installation instructions.  

## Building the application

GoL Lab uses gtkmm-3.0.    
Unfortunately, this library is not easy to distribute. Until I figure how to properly do it, you will have to install it on your system :  

#### MacOS  

    brew install pkgconfig
    brew install gtkmm3  
    
#### Debian/Ubuntu Linux  

    apt-get install libgtkmm-3.0-dev  
    
#### Fedora/RedHat Linux  

    yum install gtkmm3.0-devel  
    
You can then download and exctract the source code, and build GoL Lab with `make` from your installation directory.  

## Features  

The current version of GoL Lab implements the following features:  

| Name                  | Shortcut         | Description |
| --------------------- | ---------------- | ----------- |
| New                   | Ctrl + N         | Resets everything with a new blank simulation (closes any open file) |  
| Open                  | Ctrl + O         | Opens a .lif file defining the initial configuration of the simulation |
| Save                  | Ctrl + S         | Saves the simulation to the current opened file |
| Save as               | Ctrl + Shift + S | Renames the opened file or creates a new file, and saves the simulation |
| Quit                  | Ctrl + Q         | Quits instantly the application without saving |
| Undo                  | Ctrl + Z         | Cancels the last executed action
| Redo                  | Ctrl + Shift + Z | Executes the last cancelled action
| Cut                   | Ctrl + X         | Cuts the selection in the clipboard |
| Copy                  | Ctrl + C         | Copies the selection in the clipboard |
| Clear                 | Del/Suppr        | Clears the selection ("kills" the selected live cells) |
| Paste                 | Ctrl + V         | Pastes the pattern stored in the clipboard |
| Select all            | Ctrl + A         | Selects all the live cells |
| Random Fill           | Ctrl + R         | Fills 25% of the board at random |
| Rotate 90°            | R                | Rotates the pattern being inserted counter-clockwise by 90° |
| Flip vertically       | Y                | Flips vertically the pattern being inserted |
| Flip horizontally     | X                | Same as above but horizontally |
| Cursor Mode > Draw    | F2               | Allows you to add/remove cells one by one |
| Cursor Mode > Drag    | F3, Mouse wheel click | Allows you to pan the view frame |
| Cursor Mode > Select  | F4               | Allows you to select a group of cells |
| Zoom in               | Ctrl + +, Mouse wheel up | Zooms in by 10% |
| Zoom out              | Ctrl + -, Mouse wheel down | Zooms out by 10% |
| Restore zoom          | Ctrl + 0         | Restores the zoom to its default value |
| Show grid             | Ctrl + G         | Toggles the display of the grid |
| Fade effect           |                  | Toggles an effect in which the cells fade when they die (decreases the fps) |
| Dark theme            | Ctrl + D         | Toggles light/dark theme |
| Color scheme          |                  | Allows you to change the color scheme independantly for light/dark mode |
| Stability detection   | Ctrl + T         | Toggles the detection of stability : stops the simulation when it enters in a stable state |
| World size > Increase | Ctrl + PageUp    | Enlarge the world by 100 cells in width and 50 cells in height |
| World size > Decrease | Ctrl + PageDown  | Shrink the world by 100 cells in width and 50 cells in height |
| Insert pattern        | Ctrl + P         | Allows you to insert an RLE formatted pattern from the directory `patterns` |
| Help                  | F1               | Shows a help message similar to this brief documentation |
| About GoL Lab         |                  | Shows information relative to GoL Lab, such as its version and the license |

## Customization (Windows and MacOS only)

Like any other GTK application, you can customize the look of GoL Lab. All you have to do is to download a theme from [this website](https://www.gnome-look.org/browse?cat=135). Then extract it to the foler `GoL Lab/share/themes`, and specify the new theme as so :  
 - Open the file `etc/gtk-3.0/settings.ini`
 - Edit the "gtk-theme-name" key by replacing "Adwaita" with the name of the folder you just extracted.
 - Save the file.
Now the new theme should apply automatically next time you launch the application.

If you are interested in providing a native look to GoL Lab, here are some popular quality theme:
- [MacOS](https://github.com/B00merang-Project/macOS)  
- [Windows10](https://github.com/B00merang-Project/Windows-10)

## Todo

- Provide build system and/or installers (desktop/app package) for Linux and MacOS.

