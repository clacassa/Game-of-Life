/*
 * simulation.h
 * This module makes all the underlying hard work in the program.
 * It stores and computes the state and values of the simulation,
 * and implements crucial operations such as opening, reading and
 * decoding files, as well as saving the simulation.
 *
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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <vector>
#include <string>
#include "config.h"
#include "graphic.h"

typedef std::vector<std::vector<bool>> Grid;

namespace simulation {
    enum Mode { NORMAL, EXPERIMENTAL };
    /**
     * Erase the absolute file path to keep only the file name.
     */
    std::string remove_filepath(std::string filename);

    /** 
     * Decode Life 1.06 format.
     * @param filename Life 1.06 (.lif, .life) file to be read.
     * @return 0 on success or a positive integer if an error occured.
     */
    int read_file(std::string filename);

    /**
     * Save the simulation in Life 1.06 format.
     * @param filename Life 1.06 (.lif, .life) file to be written.
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

    /** 
     * Update the simulation (compute the n+1 state).
     * @param mode The specified simulation mode.
     * @return <tt>true</tt> if the simulation has stabilized (requires EXPERIMENTAL mode).
     */
    bool update(Mode mode = NORMAL);

    /**
     * Reset everything.
     */
    void init();

    bool is_alive(unsigned x, unsigned y);
    unsigned get_population();

    /**
     * Resize the 2D vector acording to the current world dimensions.
     */
    void adjust_bool_grid();

    void new_birth(unsigned x, unsigned y);
    void new_death(unsigned x, unsigned y);
    void new_pattern(unsigned x, unsigned y, std::vector<Coordinates> pattern);
    void del_pattern(unsigned x, unsigned y, std::vector<Coordinates> pattern);

    /**
     * Call a graphic function to draw the live cells.
     */
    void draw_cells(unsigned color_theme);

    void display();

    void toggle_fade_effect();

    Grid get_state();
    void set_state(const Grid);
} /* namespace simulation */

namespace message {

    std::string file_does_not_exist(std::string filename);

    std::string invalid_file_format(std::string filename);

    std::string unusable_file_data(std::string filename);

    std::string invalid_cell_coordinate(std::string filename);
} /* namespace message */

#endif
