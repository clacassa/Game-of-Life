OUT = gol_lab
CXX = g++
CXXFLAGS = -g -Wall -O3 -std=c++17 
# LINKING = `pkg-config --cflags gtkmm-3.0`
# LDLIBS = `pkg-config --libs gtkmm-3.0`
LINKING = -I ./include/atk-1.0 -I ./include/atkmm-1.6 -I ./lib/atkmm-1.6/include -I ./include/cairo \
		  -I ./include/cairomm-1.0 -I ./lib/cairomm-1.0/include -I ./include/freetype2 \
		  -I ./include/gdkmm-3.0 -I ./lib/gdkmm-3.0/include -I ./include/gdk-pixbuf-2.0 \
		  -I ./include/giomm-2.4 -I ./lib/giomm-2.4/include -I ./include/glib-2.0 \
		  -I ./lib/glib-2.0/include -I ./include/glibmm-2.4 -I ./lib/glibmm-2.4/include \
		  -I ./include/gtk-3.0 -I ./include/gtkmm-3.0 -I ./lib/gtkmm-3.0/include \
		  -I ./include/harfbuzz -I ./include/pango-1.0 -I ./include/pangomm-1.4 \
		  -I ./lib/pangomm-1.4/include -I ./include/sigc++-2.0 -I ./lib/sigc++-2.0/include \
		  -I ./include/fontconfig

LDLIBS = -L ./lib/ -lgtkmm-3.0 -latkmm-1.6 -lgdkmm-3.0 -lgiomm-2.4 -lpangomm-1.4 -lglibmm-2.4 \
			       -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 \
			       -lcairo-gobject -lgio-2.0 -lcairomm-1.0 -lcairo -lsigc-2.0 -lgdk_pixbuf-2.0 \
			       -lgobject-2.0 -lglib-2.0

EXEDIR = ./bin
SRC_DIR = ./src
OBJ_DIR = ./obj
SRCS = main.cc gui.cc simulation.cc graphic.cc config.cc
CXXFILES = $(SRCS:%=$(SRC_DIR)/%)
OBJ = $(SRCS:.cc=.o)
OFILES = $(OBJ:%=$(OBJ_DIR)/%)

# On Windows, start the program without a console in the background
# and link an icon to itself so it appears in the taskbar
ifeq ($(OS),Windows_NT)
# CXXFLAGS += -mwindows
RESFILES = ./res/my.res
endif

all: $(EXEDIR)/$(OUT)

$(EXEDIR)/$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) $(RESFILES) -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

$(OBJ_DIR)/main.o: $(SRC_DIR)/simulation.h $(SRC_DIR)/gui.h

$(OBJ_DIR)/simulation.o: $(SRC_DIR)/simulation.h $(SRC_DIR)/graphic.h $(SRC_DIR)/config.h

$(OBJ_DIR)/gui.o: $(SRC_DIR)/gui.h $(SRC_DIR)/graphic_gui.h $(SRC_DIR)/graphic.h \
	$(SRC_DIR)/simulation.h $(SRC_DIR)/config.h

$(OBJ_DIR)/graphic.o: $(SRC_DIR)/graphic_gui.h $(SRC_DIR)/graphic.h $(SRC_DIR)/config.h

$(OBJ_DIR)/config.o: $(SRC_DIR)/config.h


.PHONY: clean
clean:
	@rm -f $(EXEDIR)/$(OUT) $(OFILES) && rmdir $(OBJ_DIR)

