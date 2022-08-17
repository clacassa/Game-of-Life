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

#include "graphic_gui.h"
#include "config.h"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

static const std::vector<Color> color_table({black, white, gray});
#ifdef FADE_EFFECT_OPERATIONAL
static const std::vector<Color> gray_fade({gray1, gray2, gray3, gray4});
#endif
static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);
static unsigned fade_count(4);

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr) {
    ptcr = &cr;
}

void graphic_draw_world(double xMax, double yMax, unsigned ref_color) {
    (*ptcr)->set_source_rgb(color_table[ref_color].r, color_table[ref_color].g,
 	                                                    color_table[ref_color].b);
    (*ptcr)->set_line_width(1.0);
    (*ptcr)->move_to(0, 0);
    (*ptcr)->line_to(0, yMax-1);
    (*ptcr)->line_to(xMax-1, yMax-1);
    (*ptcr)->line_to(xMax-1, 1);
    (*ptcr)->close_path();
    (*ptcr)->fill_preserve();
    (*ptcr)->stroke();

    (*ptcr)->set_source_rgb(gray.r, gray.g, gray.b);
    (*ptcr)->set_line_width(0.25);
    (*ptcr)->move_to(-0.5, -0.5);
    (*ptcr)->line_to(-0.5, yMax-0.5);
    (*ptcr)->line_to(xMax-0.5, yMax-0.5);
    (*ptcr)->line_to(xMax-0.5, -0.5);
    (*ptcr)->close_path();
    (*ptcr)->stroke();

    if (show_grid) {
        (*ptcr)->set_source_rgb(gray.r, gray.g, gray.b);
        (*ptcr)->set_line_width(0.075);
        for (double i(-0.5); i<=xMax-0.5; ++i) {
            (*ptcr)->move_to(i, -0.5);
            (*ptcr)->line_to(i, yMax-0.5);
        }
        for (double j(-0.5); j<=yMax-0.5; ++j) {
            (*ptcr)->move_to(-0.5, j);
            (*ptcr)->line_to(xMax-0.5, j);
        }
        (*ptcr)->stroke();    
    }
}

void graphic_draw_cell(unsigned x, unsigned y, unsigned ref_color) {
    (*ptcr)->set_source_rgb(color_table[ref_color].r, color_table[ref_color].g,
                                                      color_table[ref_color].b);
    (*ptcr)->set_line_width(0.0001);
    (*ptcr)->move_to(x-cell_size/2., y-cell_size/2.);
    (*ptcr)->line_to(x-cell_size/2., y+cell_size/2.);
    (*ptcr)->line_to(x+cell_size/2., y+cell_size/2.);
    (*ptcr)->line_to(x+cell_size/2., y-cell_size/2.);
    (*ptcr)->close_path();
    (*ptcr)->fill_preserve();
    (*ptcr)->stroke();
}

#ifdef FADE_EFFECT_OPERATIONAL
void graphic_fade_dead(std::vector<Pos> dead, unsigned ref_color) {
    (*ptcr)->set_source_rgb(gray_fade[fade].r, gray_fade[fade].g,
                                                 gray_fade[fade].b);
    ++fade_count;
    if (fade_count >= 5)  {
        std::vector<Pos> fadead(dead);
        fade_count = 0;
        fade = ref_color;
    }
    for (unsigned i(0); i < fadead.size(); ++i) {
        (*ptcr)->set_line_width(0.0001);
        (*ptcr)->move_to(fadead[i].x - cell_size/2., fadead[i].y - cell_size/2.);
        (*ptcr)->line_to(fadead[i].x - cell_size/2., fadead[i].y + cell_size/2.);
        (*ptcr)->line_to(fadead[i].x + cell_size/2., fadead[i].y + cell_size/2.);
        (*ptcr)->line_to(fadead[i].x + cell_size/2., fadead[i].y - cell_size/2.);
        (*ptcr)->close_path();
        (*ptcr)->fill_preserve();
        (*ptcr)->stroke();
    }
    if (fadead.empty()) {
        (*ptcr)->set_line_width(1);
        (*ptcr)->move_to(18, 18);
        (*ptcr)->line_to(46, 46);
        (*ptcr)->fill_preserve();
        (*ptcr)->stroke();
    }
    
    if (fade == 0) fade = 6;
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    if (ref_color == 1) ++fade;
    else --fade;
}
#endif

void enable_show_grid() {
    show_grid = true;
}

void disable_show_grid() {
    show_grid = false;
}
