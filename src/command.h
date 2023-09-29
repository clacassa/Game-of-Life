/*
 * command.h
 * This module defines the commands derived from the Command abstract
 * class. The purpose of this module is to implement a multi-level
 * undo/redo mechanism using the Command pattern.
 * 
 * This file is part of GoL Lab, a simulator of Conway's game of life.
 *
 * Copyright (C) 2022 - Cyprien Lacassagne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <memory>
#include <vector>
#include <gtkmm.h>
#include "simulation.h"
#include "config.h"

constexpr unsigned random_seed_max_world_width(500);

class MainArea;

class Command {
public:
    virtual ~Command() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
};

typedef std::vector<Command*> History;

class CutCommand : public Command {
public:
    CutCommand(MainArea* main_area_, std::vector<Coordinates> prev_clipboard_, Gtk::MenuItem* cut_,
                Gtk::ToolButton* tool_cut_, Gtk::MenuItem* copy_, Gtk::ToolButton* tool_copy_,
                Gtk::MenuItem* clear_, Gtk::MenuItem* paste_, Gtk::ToolButton* tool_paste_) 
        : main_area(main_area_), prev_clipboard(prev_clipboard_), cut(cut_), tool_cut(tool_cut_),
            copy(copy_), tool_copy(tool_copy_), clear(clear_), paste(paste_), tool_paste(tool_paste_) {}
    virtual void execute();
    virtual void undo();
private:
    MainArea* main_area;
    std::vector<Coordinates> prev_clipboard;
    Gtk::MenuItem* cut;
    Gtk::ToolButton* tool_cut;
    Gtk::MenuItem* copy;
    Gtk::ToolButton* tool_copy;
    Gtk::MenuItem* clear;
    Gtk::MenuItem* paste;
    Gtk::ToolButton* tool_paste;
};

class CopyCommand : public Command {
public:
    CopyCommand(MainArea* main_area_, Gtk::MenuItem* cut_,
                Gtk::ToolButton* tool_cut_, Gtk::MenuItem* copy_, Gtk::ToolButton* tool_copy_,
                Gtk::MenuItem* clear_, Gtk::MenuItem* paste_, Gtk::ToolButton* tool_paste_) 
        : main_area(main_area_), cut(cut_), tool_cut(tool_cut_),
            copy(copy_), tool_copy(tool_copy_), clear(clear_), paste(paste_), tool_paste(tool_paste_) {}
    virtual void execute();
    virtual void undo();
private:
    MainArea* main_area;
    Gtk::MenuItem* cut;
    Gtk::ToolButton* tool_cut;
    Gtk::MenuItem* copy;
    Gtk::ToolButton* tool_copy;
    Gtk::MenuItem* clear;
    Gtk::MenuItem* paste;
    Gtk::ToolButton* tool_paste;
};

class ClearCommand : public Command {
public:
    ClearCommand(MainArea* main_area_, std::vector<Coordinates> prev_clipboard_, Gtk::MenuItem* cut_,
                Gtk::ToolButton* tool_cut_, Gtk::MenuItem* copy_, Gtk::ToolButton* tool_copy_,
                Gtk::MenuItem* clear_) 
        : main_area(main_area_), prev_clipboard(prev_clipboard_), cut(cut_), tool_cut(tool_cut_),
            copy(copy_), tool_copy(tool_copy_), clear(clear_) {}
    virtual void execute();
    virtual void undo();
private:
    MainArea* main_area;
    std::vector<Coordinates> prev_clipboard;
    Gtk::MenuItem* cut;
    Gtk::ToolButton* tool_cut;
    Gtk::MenuItem* copy;
    Gtk::ToolButton* tool_copy;
    Gtk::MenuItem* clear;
};

class PasteCommand : public Command {
public:
    PasteCommand(Gtk::MenuItem* rotate_, Gtk::MenuItem* vflip_, Gtk::MenuItem* hflip_, bool* inserting_)
        : rotate(rotate_), vflip(vflip_), hflip(hflip_), inserting(inserting_) {}
    virtual void execute();
    virtual void undo();
private:
    Gtk::MenuItem* rotate;
    Gtk::MenuItem* vflip;
    Gtk::MenuItem* hflip;
    bool* inserting;
};

class RandomSeedCommand : public Command {
public:
    RandomSeedCommand(Grid prev_state_)
        : prev_state(prev_state_) {}
    virtual void execute();
    virtual void undo();
private:
    Grid prev_state;
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

class InsertPatternCommand : public Command {
public:
    InsertPatternCommand(unsigned x_, unsigned y_, Grid prev_state_, std::vector<Coordinates> pattern_)
        : x(x_), y(y_), prev_state(prev_state_), pattern(pattern_) {}
    virtual void execute();
    virtual void undo();
private:
    unsigned x, y;
    Grid prev_state;
    std::vector<Coordinates> pattern;
};

class EnlargeWorldCommand : public Command {
public:
    EnlargeWorldCommand(Gtk::MenuItem* dec_, Gtk::MenuItem* inc_, Gtk::MenuItem* rand_)
        : dec(dec_), inc(inc_), rand(rand_) {}
    virtual void execute();
    virtual void undo();
private:
    Gtk::MenuItem* dec;
    Gtk::MenuItem* inc;
    Gtk::MenuItem* rand;
};

class ShrinkWorldCommand : public Command {
public:
    ShrinkWorldCommand(Gtk::MenuItem* dec_, Gtk::MenuItem* inc_, Gtk::MenuItem* rand_)
        : dec(dec_), inc(inc_), rand(rand_) {}
    virtual void execute();
    virtual void undo();
private:
    Gtk::MenuItem* dec;
    Gtk::MenuItem* inc;
    Gtk::MenuItem* rand;
};

#endif