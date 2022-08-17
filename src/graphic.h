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

static bool show_grid(false);
#ifdef FADE_EFFECT_OPERATIONAL
static unsigned fade;
static std::vector<Pos> fadead;
#endif

// Default
constexpr Color black({0.0, 0.0, 0.0});
constexpr Color white({1.0, 1.0, 1.0});
constexpr Color gray({0.5, 0.5, 0.5});

// Other colors
constexpr Color night_blue({0.0, 0.09, 0.2});

#ifdef FADE_EFFECT_OPERATIONAL
// Grayscale for fade effect
constexpr Color gray1({0.82, 0.82, 0.82});
constexpr Color gray2({0.65, 0.65, 0.65});
constexpr Color gray3({0.34, 0.34, 0.34});
constexpr Color gray4({0.19, 0.19, 0.19});
#endif


void graphic_draw_world(double xMax, double yMax, unsigned ref_color);
void graphic_draw_cell(unsigned x, unsigned y, unsigned ref_color);
#ifdef FADE_EFFECT_OPERATIONAL
void graphic_fade_dead(std::vector<Pos> dead, unsigned ref_color);
#endif

void enable_show_grid();
void disable_show_grid();

#endif