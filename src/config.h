/************************************************************************

*	Game of Life -- GUI with various options and view controls
*	Copyright (C) 2022 Cyprien Lacassagne

*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.

*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.

*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <https://www.gnu.org/licenses/>.

*************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

// #define FADE_EFFECT_OPERATIONAL // In development
#include <iostream>

//constexpr unsigned world_size(300);
constexpr unsigned window_size(850);
constexpr unsigned cell_size(1);
constexpr unsigned reserve(250);
constexpr unsigned world_size_max(500);
constexpr unsigned world_size_min(50);

constexpr unsigned dialog_button_margin(400);

class Conf {
public:
    static unsigned world_size;
    static void set_world_size(unsigned wsize);
};

#endif
