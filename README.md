
# Game-of-Life

## Dev notes

TODO: Bundle the runtime DLLs and make them render the selected theme correctly.  
    For some reason I have experienced unexpected errors such as **Gdk::PixBufError**  
    or **Gtk::CssProviderError**, refusing to load the custom theme, when putting the dlls  
    in the *bin* directory.   

## Installation

As I've put a Makefile in the repo, I encourage you to build with **make**.

This GUI is made with GTK3.0 and the gtkmm C++ wrapper.
All the libraries on which they depend are in the include subfolder. 
All the runtime dlls are in the bin subfolder. 

The second step is about the appearance of the GUI.  
**If you are on Windows, you can ignore this.**  
In the file *./etc/gtk-3.0/settings.ini*:  
If you are on MacOS, replace "Windows10" with "MacOS", at line 2.  
This will set a convenient theme for mac.
If you are on linux, delete this line.


