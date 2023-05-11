/*
 * config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>

// #define PREBUILT_BINARY_FOR_WINDOWS

#define LIVE_ARRAY_OPTIMIZATION

const std::string PROGRAM_NAME("GoL Lab");
const std::string PROGRAM_VERSION("0.3.0");
const std::string PROGRAM_AUTHOR("Cyprien Lacassagne");
const std::string PROGRAM_ICON("share/icons/gol_lab.ico");

const std::string PATTERNS_DIR("patterns/");
const std::string HELP_FILE("Help.txt");
const std::string SETTINGS_INI_FILE("etc/gtk-3.0/settings.ini");

constexpr unsigned window_width(600);
constexpr unsigned window_height(window_width/2);

constexpr unsigned cell_size(1);
constexpr unsigned reserve(500);

constexpr unsigned world_size_max(5000);
constexpr unsigned world_size_min(50);
constexpr unsigned increment_step(100);

constexpr unsigned startup_timeout_value(20);
constexpr unsigned idle_timeout(5);

constexpr unsigned dialog_button_margin(200);
constexpr float ghost_color(0.6);

class Conf {
public:
    // static unsigned world_size;
    // static unsigned world_y_max;
    static void set_world_size(unsigned wsize);
    static unsigned get_x_max() { return world_x_max; }
    static unsigned get_y_max() { return world_y_max; }
    static std::string working_dir();
private:
    static unsigned world_x_max;
    static unsigned world_y_max;
};

#endif
