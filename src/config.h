/*
 *  config.h -- GoL Lab -- GUI with various options and view controls
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

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>

const std::string PROGRAM_NAME("GoL Lab");
const std::string PROGRAM_VERSION("0.2.1");
const std::string PROGRAM_AUTHOR("Cyprien Lacassagne");
const std::string PROGRAM_ICON("share/icons/gol_lab.ico");

const std::string PATTERNS_DIR("patterns/");
const std::string SETTINGS_INI_FILE("etc/gtk-3.0/settings.ini");

constexpr unsigned window_size(850);    // window size in pixels
constexpr unsigned cell_size(1);
constexpr unsigned reserve(300);    // At startup, the world is a square of side reserve.
                                    // The # of cells is reserve^2
                                    // This value must be included between world_size_min
                                    // and world_size_max

constexpr unsigned world_size_max(2000); // Upper limit for the side of the world.
                                        // Feel free to change it
constexpr unsigned world_size_min(50);
constexpr unsigned startup_timeout_value(30);
constexpr unsigned idle_timeout(5);

constexpr unsigned dialog_button_margin(200);
constexpr float ghost_color(0.6);

class Conf {
public:
    static unsigned world_size;
    static unsigned world_y_max;
    static void set_world_size(unsigned wsize);
};

#endif
