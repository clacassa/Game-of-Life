/*
 *  simulation.h -- GameofLife -- GUI with various options and view controls
 *  Copyright (C) 2022 Cyprien Lacassagne

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include "config.h"
#include <iostream>
#include <vector>
#include <string>

enum Error_reading { READING_OPENING, READING_WORLDSIZE, READING_END, BAD_EXTENSION };
enum Mode { EXPERIMENTAL, NORMAL };

void save_file(std::string filename);
bool read_file(std::string filename);
void line_decoding(std::string line);
void error(Error_reading code);
unsigned get_error_id();

void display();
void init();
bool update(Mode mode = NORMAL);
void adjust_bool_grid();
void toggle_fade_effect();
void fade_update();

void draw_canon_planeur(unsigned x, unsigned y);
void erase_canon_planeur(unsigned x, unsigned y);
void draw_spacefiller(unsigned x, unsigned y);
void erase_spacefiller(unsigned x, unsigned y);

unsigned neighbours(unsigned x, unsigned y);
void birth_test(unsigned x, unsigned y);

void new_birth(unsigned x, unsigned y);
void new_death(unsigned x, unsigned y);

void draw_world(unsigned color_theme);

#endif