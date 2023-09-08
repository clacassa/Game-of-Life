#include <iostream>
#include "command.h"
#include "config.h"

void Randomize::execute() {
    init();  
    // Randomly fill half of the grid
    unsigned rand_x, rand_y;
    for (unsigned index(0); index < (Conf::get_x_max()*Conf::get_y_max())/2; ++index) {
        rand_x = rand() % Conf::get_x_max();
        rand_y = rand() % Conf::get_y_max();
        new_birth(rand_x, rand_y);
    }
}

void Randomize::undo() {
    init();
    set_state(previous_state);
}

void AddCellCommand::execute() {
    new_birth(x, y);
}

void AddCellCommand::undo() {
    new_death(x, y);
}

void RemoveCellCommand::execute() {
    new_death(x, y);
}

void RemoveCellCommand::undo() {
    new_birth(x, y);
}

void InsertRLEPattern::execute() {
    new_pattern(x, y, pattern);
}

void InsertRLEPattern::undo() {
    del_pattern(x, y, pattern);
}

void IncreaseWSize::execute() {
    if (world_size_max - Conf::get_x_max() < increment_step) {
        Conf::set_world_size(world_size_max);
    }else {
        Conf::set_world_size(Conf::get_x_max() + increment_step);
    }
    if (Conf::get_x_max() == world_size_max) {
        inc->set_sensitive(false);
    }
    if (Conf::get_x_max() > 300) {
        rand->set_sensitive(false);
    }

    dec->set_sensitive(true);
}

void IncreaseWSize::undo() {
    if (Conf::get_x_max() - world_size_min < increment_step) {
        Conf::set_world_size(world_size_min);
    }else {
        Conf::set_world_size(Conf::get_x_max() - increment_step);
    }
    if (Conf::get_x_max() == world_size_min)  {
        dec->set_sensitive(false);
    }
    if (Conf::get_x_max() <= 300) {
        rand->set_sensitive(true);
    }

    inc->set_sensitive(true);
}

DecreaseWSize::DecreaseWSize(Gtk::MenuItem* dec_,
    Gtk::MenuItem* inc_, Gtk::MenuItem* rand_)
:   dec(dec_),
    inc(inc_),
    rand(rand_) 
{}

void DecreaseWSize::execute() {
    if (Conf::get_x_max() - world_size_min < increment_step) {
        Conf::set_world_size(world_size_min);
    }else {
        Conf::set_world_size(Conf::get_x_max() - increment_step);
    }
    if (Conf::get_x_max() == world_size_min)  {
        dec->set_sensitive(false);
    }
    if (Conf::get_x_max() <= 300) {
        rand->set_sensitive(true);
    }
    inc->set_sensitive(true);
}

void DecreaseWSize::undo() {
    if (world_size_max - Conf::get_x_max() < increment_step) {
        Conf::set_world_size(world_size_max);
    }else {
        Conf::set_world_size(Conf::get_x_max() + increment_step);
    }
    if (Conf::get_x_max() == world_size_max) {
        inc->set_sensitive(false);
    }
    if (Conf::get_x_max() > 300) {
        rand->set_sensitive(false);
    }
    dec->set_sensitive(true);
}
