#include <iostream>
#include <memory>
#include <vector>
#include <gtkmm.h>
#include "simulation.h"
#include "config.h"

class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
};

typedef std::vector<Command*> History;

class Randomize : public Command {
public:
    Randomize(Grid prev_state)
        : previous_state(prev_state) {}
    virtual void execute();
    virtual void undo();
private:
    Grid previous_state;
};

class AddCellCommand : public Command {
public:
    AddCellCommand(unsigned x_, unsigned y_)
        : x(x_), y(y_) {}
    virtual void execute();
    virtual void undo();
private:
    unsigned x, y;
};

class RemoveCellCommand : public Command {
public:
    RemoveCellCommand(unsigned x_, unsigned y_)
        : x(x_), y(y_) {}
    virtual void execute();
    virtual void undo();
private:
    unsigned x, y;
};

class InsertRLEPattern : public Command {
public:
    InsertRLEPattern(unsigned x_, unsigned y_, std::vector<Coordinates> pattern_)
        : x(x_), y(y_), pattern(pattern_) {}
    virtual void execute();
    virtual void undo();
private:
    unsigned x, y;
    std::vector<Coordinates> pattern;
};

class IncreaseWSize : public Command {
    IncreaseWSize(Gtk::MenuItem* dec_, Gtk::MenuItem* inc_, Gtk::MenuItem* rand_)
        : dec(dec_), inc(inc_), rand(rand_) {}
    virtual void execute();
    virtual void undo();
private:
    Gtk::MenuItem* dec;
    Gtk::MenuItem* inc;
    Gtk::MenuItem* rand;
};

class DecreaseWSize : public Command {
public:
    DecreaseWSize(Gtk::MenuItem*, Gtk::MenuItem*, Gtk::MenuItem*);
    virtual void execute();
    virtual void undo();
private:
    Gtk::MenuItem* dec;
    Gtk::MenuItem* inc;
    Gtk::MenuItem* rand;
};