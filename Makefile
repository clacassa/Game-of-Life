# Makefile
# This file is part of GoL Lab, a simulator of Conway's game of life.
#
# Copyright (C) 2022 - Cyprien Lacassagne
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

OUT = gol_lab
CXX = g++
CXXFLAGS = -g -Wall -O3 -std=c++17
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
EXEDIR = ./bin
SRC_DIR = ./src
OBJ_DIR = ./obj
SRCS = main.cc gui.cc command.cc simulation.cc graphic.cc config.cc
CXXFILES = $(SRCS:%=$(SRC_DIR)/%)
OFILES = $(SRCS:.cc=.o)

# MacOS
ifeq ($(UNAME_S), Darwin)
export PKG_CONFIG_PATH = $PKG_CONFIG_PATH:/usr/local/Cellar/libffi/3.2.1/lib/pkgconfig
endif

# Additionnal build flags for Windows (-mwindows: no terminal in the background)
ifeq ($(OS), Windows_NT)
CXXFLAGS += -mwindows
LDLIBS += -L ./lib/ -lshlwapi
RESFILES = ./res/my.res
else
	@echo "Removing the unnecessary dll files"
	@rm -f $(EXEDIR)/*.dll
endif


all: setup $(EXEDIR)/$(OUT)
	@mv $(OFILES) $(OBJ_DIR)

$(EXEDIR)/$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) $(RESFILES) -o $@ $(LDLIBS)

%.o: $(SRC_DIR)/%.cc
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)


.PHONY: setup
setup:
	@mkdir -p $(OBJ_DIR)
	@if [ -f "$(OBJ_DIR)/main.o" ]; \
	then mv $(OBJ_DIR)/* . ;\
	fi

.PHONY: run
run: all
	$(EXEDIR)/$(OUT)

.PHONY: clean
clean:
	@rm -f $(EXEDIR)/$(OUT) && rm -r -f  $(OBJ_DIR)

.PHONY: depend
depend:
	@echo " *** DEPENDENCIES UPDATE ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
		$(CXX) -MM $(CXXFLAGS) $(CXXFILES) | \
		egrep -v "/usr/include") > Makefile.new
	@mv Makefile.new Makefile
#
# -- Regles de dependances generes automatiquement -- 
#
# DO NOT DELETE THIS LINE
main.o: src/main.cc src/simulation.h src/config.h src/graphic.h src/gui.h
gui.o: src/gui.cc src/gui.h src/simulation.h src/config.h src/graphic.h \
 src/graphic_gui.h src/prefs.h
command.o: src/command.cc src/command.h src/simulation.h src/config.h \
 src/graphic.h
simulation.o: src/simulation.cc src/simulation.h src/config.h \
 src/graphic.h
graphic.o: src/graphic.cc src/graphic_gui.h src/graphic.h src/config.h
config.o: src/config.cc src/config.h
