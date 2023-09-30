/*
 * command.cc
 * This file is part of GoL Lab, a simulator of Conway's game of life.
 *
 * Copyright (C) 2022-2023 Cyprien Lacassagne
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

#include <iostream>
#include "gui.h"
#include "command.h"
#include "config.h"

void CutCommand::execute() {
    main_area->set_clipboard(main_area->get_selection());
    main_area->set_pattern(main_area->rebase_coords(main_area->get_clipboard()));
    if (!main_area->get_clipboard().empty()) {
        simulation::clear_pattern(0, 0, main_area->get_clipboard());
        main_area->set_selection(simulation::get_live_cells_in_area(0, 0, 0, 0));
    }
    cut->set_sensitive(false);
    tool_cut->set_sensitive(false);
    copy->set_sensitive(false);
    tool_copy->set_sensitive(false);
    clear->set_sensitive(false);
    paste->set_sensitive();
    tool_paste->set_sensitive();
}

void CutCommand::undo() {
    main_area->set_clipboard(simulation::get_live_cells_in_area(0, 0, 0, 0));
    main_area->set_selection(prev_clipboard);
    simulation::new_pattern(0, 0, prev_clipboard);

    cut->set_sensitive();
    tool_cut->set_sensitive();
    copy->set_sensitive();
    tool_copy->set_sensitive();
    clear->set_sensitive();
    paste->set_sensitive(false);
    tool_paste->set_sensitive(false);
}

void CopyCommand::execute() {
    main_area->set_clipboard(main_area->get_selection());
    main_area->set_pattern(main_area->rebase_coords(main_area->get_clipboard()));

    cut->set_sensitive(false);
    tool_cut->set_sensitive(false);
    copy->set_sensitive(false);
    tool_copy->set_sensitive(false);
    clear->set_sensitive(false);
    paste->set_sensitive();
    tool_paste->set_sensitive();
}

void CopyCommand::undo() {
    main_area->set_clipboard(simulation::get_live_cells_in_area(0, 0, 0, 0));

    cut->set_sensitive();
    tool_cut->set_sensitive();
    copy->set_sensitive();
    tool_copy->set_sensitive();
    clear->set_sensitive();
    paste->set_sensitive(false);
    tool_paste->set_sensitive(false);
}

void ClearCommand::execute() {
    if (!main_area->get_selection().empty()) {
        simulation::clear_pattern(0, 0, main_area->get_selection());
        main_area->set_selection(simulation::get_live_cells_in_area(0, 0, 0, 0));
    }

    cut->set_sensitive(false);
    tool_cut->set_sensitive(false);
    copy->set_sensitive(false);
    tool_copy->set_sensitive(false);
    clear->set_sensitive(false);
}

void ClearCommand::undo() {
    main_area->set_selection(prev_clipboard);
    simulation::new_pattern(0, 0, prev_clipboard);
}

void PasteCommand::execute() {
    rotate->set_sensitive();
    vflip->set_sensitive();
    hflip->set_sensitive();
    *inserting = true;
}

void PasteCommand::undo() {
    rotate->set_sensitive(false);
    vflip->set_sensitive(false);
    hflip->set_sensitive(false);
    *inserting = false;
}

void RandomSeedCommand::execute() {
    simulation::init();  
    unsigned world_area(simulation::get_width()*simulation::get_height());
    unsigned rand_x, rand_y;
    for (unsigned index(0); index < world_area / 4; ++index) {
        rand_x = rand() % simulation::get_width();
        rand_y = rand() % simulation::get_height();
        simulation::set_cell(rand_x, rand_y);
    }
}

void RandomSeedCommand::undo() {
    simulation::init();
    simulation::set_state(prev_state);
}

void RemoveCellCommand::execute() {
    simulation::clear_cell(x, y);
}

void RemoveCellCommand::undo() {
    simulation::set_cell(x, y);
}

void InsertPatternCommand::execute() {
    simulation::new_pattern(x, y, pattern);
}

void InsertPatternCommand::undo() {
    simulation::init();
    simulation::set_state(prev_state);
}

void EnlargeWorldCommand::execute() {
    if (world_size_max - simulation::get_width() < increment_step) {
        simulation::resize_world(world_size_max);
    }else {
        simulation::resize_world(simulation::get_width() + increment_step);
    }
    if (simulation::get_width() == world_size_max) {
        inc->set_sensitive(false);
    }
    if (simulation::get_width() > random_seed_max_world_width) {
        rand->set_sensitive(false);
    }

    dec->set_sensitive();
}

void EnlargeWorldCommand::undo() {
    if (simulation::get_width() - world_size_min < increment_step) {
        simulation::resize_world(world_size_min);
    }else {
        simulation::resize_world(simulation::get_width() - increment_step);
    }
    if (simulation::get_width() == world_size_min)  {
        dec->set_sensitive(false);
    }
    if (simulation::get_width() <= random_seed_max_world_width) {
        rand->set_sensitive();
    }

    inc->set_sensitive();
}

void ShrinkWorldCommand::execute() {
    if (simulation::get_width() - world_size_min < increment_step) {
        simulation::resize_world(world_size_min);
    }else {
        simulation::resize_world(simulation::get_width() - increment_step);
    }
    if (simulation::get_width() == world_size_min)  {
        dec->set_sensitive(false);
    }
    if (simulation::get_width() <= random_seed_max_world_width) {
        rand->set_sensitive();
    }
    inc->set_sensitive();
}

void ShrinkWorldCommand::undo() {
    if (world_size_max - simulation::get_width() < increment_step) {
        simulation::resize_world(world_size_max);
    }else {
        simulation::resize_world(simulation::get_width() + increment_step);
    }
    if (simulation::get_width() == world_size_max) {
        inc->set_sensitive(false);
    }
    if (simulation::get_width() > random_seed_max_world_width) {
        rand->set_sensitive(false);
    }
    dec->set_sensitive();
}
