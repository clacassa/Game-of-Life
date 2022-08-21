/*
 *  graphic.h -- GameofLife -- GUI with various options and view controls
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

#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

struct Color {
    double r;
    double g;
    double b;
};

struct Pos {
    unsigned x;
    unsigned y;
};

// Default
constexpr Color black({0.0, 0.0, 0.0});
constexpr Color white({1.0, 1.0, 1.0});
constexpr Color gray({0.5, 0.5, 0.5});

// Other colors
constexpr Color night_blue({0.0, 0.09, 0.2});
constexpr Color dark_blueish_green({0.0, 0.2, 0.2});

// Grayscale for fade effect
constexpr Color gray1({0.82, 0.82, 0.82});
constexpr Color gray2({0.65, 0.65, 0.65});
constexpr Color gray3({0.34, 0.34, 0.34});
constexpr Color gray4({0.19, 0.19, 0.19});

void graphic_draw_world(double xMax, double yMax, unsigned ref_color, bool show_grid);
void graphic_draw_cell(unsigned x, unsigned y, unsigned ref_color);
void graphic_fade_dead(unsigned x, unsigned y, const Color gray);

#endif