
# Game-of-Life

## Dev notes

Please compile the source code instead of running the executable.  
The latter doesn't have the last fixes and updates.  
I put it here just for people who don't have compilation tools.  
However everything will be sync between exe and source, once I wil have
published a release.  
For the moment, the gui is mainly made and thought for Windows users.  
I'm working on making it the slightest bit portable.  

## Installation

The first step is to compile the source files, if you have make installed,
run "make".  
If you don't, i.e you are on windows and don't have CygWin nor MSYS2,  
you can still compile the files one by one, then linking them together with g++.

The second step is about the appearance of the gui:  
if you are on Windows, skip this step.  
If you are on linux or mac, delete this line in "./etc/gtk-3.0/settings.ini":  
2 | gtk-theme-name=Windows102


