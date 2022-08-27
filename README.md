
# Game-of-Life

**Conway's Game of Life** GUI with various controls.

## Dev notes

Last build: 2022/26/08  
Evertything is now bundled in the repo, so everything should work fine now.  
Please note that the Stability Detection feature needs a real improvement; for the moment it just counts the number  
of alive cells and compares it to the last four generations, so this only works for basic oscillators of period 5 and less.  
I have to figure out another way to do this. It false-positive gliders btw...

## Installation

- Extract the zip file in the directory of your choice.  
- Create a Desktop shortcut targetting *installdir/bin/GameofLife.exe*, where *installdir/* is the location where  
you extracted the zip file.  
- The shortcut starts the program in the *installdir/bin/*. You have to edit this path to *installdir/*  
so that the executable can find everything it needs to run (theme, icons, runtime libs...)  
Right click on the shortcut, then go to Properties, then Startup directory.

## Building the application

As I've put a Makefile in the repo, I encourage you to build with **make** from *installdir/*.  

The GUI is made with GTK3+ and the gtkmm C++ wrapper toolkits.
The libraries on which they depend are in the *include* subdirectory and   
All the runtime dlls are in the *bin/* and *lib/gdk-pixbuf-2.0/2.10.0/* directories.  

## Customization

You can customize the appearance of the GUI by editing the file *etc/gtk-3.0/settings.ini*.  
Just edit the value of **gtk-theme-name** as following:  
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
