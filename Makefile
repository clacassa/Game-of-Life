 #  Makefile -- GameofLife -- GUI with various options and view controls
 #  Copyright (C) 2022 Cyprien Lacassagne

 #  This program is free software: you can redistribute it and/or modify
 #  it under the terms of the GNU General Public License as published by
 #  the Free Software Foundation, either version 3 of the License, or
 #  (at your option) any later version.

 #  This program is distributed in the hope that it will be useful,
 #  but WITHOUT ANY WARRANTY; without even the implied warranty of
 #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 #  GNU General Public License for more details.

 #  You should have received a copy of the GNU General Public License
 #  along with this program.  If not, see <https://www.gnu.org/licenses/>.

OUT = GameofLife
CXX = g++
CXXFLAGS = -g -Wall -std=c++11 -O3
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
CXXFILES = main.cc simulation.cc gui.cc graphic.cc config.cc
OFILES = $(CXXFILES:.cc=.o)
EXEDIR = ./bin

# On Windows, start the program without a console in the background
# and link an icon to it
ifeq ($(OS),Windows_NT)
#CXXFLAGS += -mwindows
OFILES += ./res/my.res
endif

all: $(EXEDIR)/$(OUT)

$(EXEDIR)/$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

main.o: ./src/main.cc ./src/simulation.h ./src/gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

simulation.o: ./src/simulation.cc ./src/simulation.h ./src/graphic.h ./src/config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

gui.o: ./src/gui.cc ./src/gui.h ./src/graphic_gui.h ./src/graphic.h ./src/simulation.h ./src/config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

graphic.o: ./src/graphic.cc ./src/graphic_gui.h ./src/graphic.h ./src/config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

config.o: ./src/config.cc ./src/config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

.PHONY: clean
clean:
	@rm -f $(EXEDIR)/$(OUT) *.o
