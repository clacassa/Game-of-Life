/*
 * simulation.h
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

#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include "config.h"
#include "graphic.h"

enum Mode { NORMAL, EXPERIMENTAL };

/**
 * Erase the absolute file path to keep only the file name.
 */
std::string filename_from_filepath(std::string filepath);

/** 
 * Decode pseudo Life 1.06 format.
 * @param filename Plain text file to be read.
 * @return The parsing result (0: succes, otherwise failure)
 */
int read_file(std::string filename);

/**
 * Save the simulation in pseudo Life 1.06 format.
 * @param filename Plain text file to be written.
 * @note If the file does not exist yet, a new file is created.
 */
void save_file(std::string filename);

/**
 * Decode RLE format.
 * @param filename RLE file to be read.
 * @return A vector storing the coordinates couples.
*/
std::vector<Coordinates> get_rle_data(std::string filename);

/**
 * Get all live cells within a rectangular area.
 * @return A vector storing the coordinates couples.
 */
std::vector<Coordinates> get_live_cells_in_area(unsigned x_min, unsigned x_max,
                                                unsigned y_min, unsigned y_max);

unsigned get_alive();
void display();

/**
 * Reset everything.
 */
void init();

/** 
 * Update the simulation (compute the n+1 state).
 * @param mode The specified simulation mode.
 * @return <tt>true</tt> if the simulation has stabilized (requires EXPERIMENTAL mode).
 */
bool update(Mode mode = NORMAL);

/**
 * Resize the 2D vector acording to the current world dimensions.
 */
void adjust_bool_grid();

void toggle_fade_effect();

void new_birth(unsigned x, unsigned y);
void new_death(unsigned x, unsigned y);
void new_pattern(unsigned x, unsigned y, std::vector<Coordinates> pattern);
void del_pattern(unsigned x, unsigned y, std::vector<Coordinates> pattern);

/**
 * Call a graphic function to draw the live cells.
 */
void draw_cells(unsigned color_theme);


namespace message {
    /**
     * The file specified on command line was not found.
     */
    std::string file_does_not_exist();

    /**
     * The file specified on command line is not a plain text file.
     */
    std::string invalid_file_format();

    /**
     * The world dimensions information is not found,
     * or the specified world dimensions are out of range
     * or do not meet the required aspect ratio (width/height).
     */
    std::string invalid_world_dimensions();

    /**
     * A coordinate was found out of range (depends on the world dimensions).
     */
    std::string invalid_cell_coordinate();
} /* namespace message */

#endif
