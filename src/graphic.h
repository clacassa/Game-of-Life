/*
 * graphic.h
 * This module renders the simulation and any action operating in the 
 * drawing area widget, such as giving a specific color to selected
 * cells and to the clipboard, displaying a grid... 
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

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <vector>
#include "config.h"

struct Color {
    double r;
    double g;
    double b;
};

struct Scheme {
    Color fg;
    Color bg;
    Color highlight;
};

constexpr float ghost_color(0.6);

// -----Colors-----
constexpr Color black({0.0, 0.0, 0.0});
constexpr Color white({1.0, 1.0, 1.0});
constexpr Color yellow({1.0, 1.0, 0.0});
constexpr Color blue({0.0, 0.0, 1.0});
constexpr Color gray({0.5, 0.5, 0.5});
constexpr Color night_blue({0.0, 0.09, 0.2});
constexpr Color dark_blueish_green({0.0, 0.2, 0.2});
constexpr Color gray1({0.82, 0.82, 0.82});
constexpr Color gray2({0.65, 0.65, 0.65});
constexpr Color gray3({0.34, 0.34, 0.34});
constexpr Color gray4({0.19, 0.19, 0.19});

// -----Color Schemes-----
constexpr Scheme DefaultLight({black, white, {0.0, 0.6, 0.3}});
constexpr Scheme DefaultDark({white, black, {1.0, 0.4, 0.7}});
constexpr Scheme Jade({gray1, dark_blueish_green, {0.82, 0.22, 0.52}});
constexpr Scheme Basalt({white, gray3, {1.0, 0.4, 0.7}});
constexpr Scheme Asphalt({gray, black, {0.5, 0.0, 0.2}});
constexpr Scheme Albaster({black, gray1, {0.0, 0.6, 0.3}});
constexpr Scheme DeepBlue({gray2, night_blue, {0.65, 0.05, 0.35}});

void graphic_draw_world(double xMax, double yMax, bool ref_color, bool show_grid, unsigned delta);
void graphic_draw_cell(unsigned x, unsigned y, bool ref_color);
void graphic_draw_cell(unsigned x, unsigned y, const Color);
void graphic_fade_dead(unsigned x, unsigned y, const Color gray);
void graphic_ghost_pattern(unsigned x, unsigned y, std::vector<Coordinates> cells, bool ref_color);
void graphic_draw_select_rec(unsigned x_0, unsigned y_0, unsigned x, unsigned y, bool ref_color);
void graphic_highlight_selected_cells(std::vector<Coordinates> selected_cells, bool ref_color);
void graphic_change_light_color_scheme(unsigned id);
void graphic_change_dark_color_scheme(unsigned id);

#endif
