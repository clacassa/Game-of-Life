OUT = GameofLife
CXX = g++
CXXFLAGS = -g -Wall -std=c++11
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
CXXFILES = main.cc simulation.cc gui.cc graphic.cc config.cc
OFILES = $(CXXFILES:.cc=.o)
EXEDIR = ./bin

all: $(EXEDIR)/$(OUT)

$(EXEDIR)/$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) ./res/my.res -o $@ $(LDLIBS)

main.o: main.cc simulation.h gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

simulation.o: simulation.cc simulation.h graphic.h config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

gui.o: gui.cc gui.h graphic_gui.h graphic.h simulation.h config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

graphic.o: graphic.cc graphic_gui.h graphic.h config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

config.o: config.cc config.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

.PHONY: clean
clean:
	@rm -f *.o *.exe  $(EXEDIR)/$(OUT) *.cc~ *.h~ projet
