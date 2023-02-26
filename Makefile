OUT = gol_lab
CXX = g++
CXXFLAGS = -g -Wall -O3 -std=c++17 
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
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

