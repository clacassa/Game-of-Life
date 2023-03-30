/*
 * graphic.cc
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

#include "graphic_gui.h"
#include "config.h"

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

// Store the current light and dark color schemes
static std::vector<Scheme> current_color_scheme{DefaultLight, DefaultDark};
// Store all the color schemes for an easy access by 'current_color_scheme'
static const std::vector<Scheme> color_scheme_table {
    DefaultLight,
    DefaultDark,
    Jade,
    Basalt,
    Asphalt,
    Albaster,
    DeepBlue
};

static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

void graphic_set_context(const Cairo::RefPtr<Cairo::Context>& cr) {
    ptcr = &cr;
}

void graphic_draw_world(double xMax, double yMax, unsigned ref_color, bool show_grid, unsigned delta) {
    (*ptcr)->set_source_rgb(current_color_scheme[ref_color].bg.r,
                            current_color_scheme[ref_color].bg.g,
 	                        current_color_scheme[ref_color].bg.b);                            
    (*ptcr)->paint();
    (*ptcr)->stroke();

    (*ptcr)->set_source_rgb(gray.r, gray.g, gray.b);
    (*ptcr)->set_line_width(0.25);

    (*ptcr)->move_to(-0.5, -0.5);
    (*ptcr)->line_to(-0.5, yMax-0.5);
    (*ptcr)->line_to(xMax-0.5, yMax-0.5);
    (*ptcr)->line_to(xMax-0.5, -0.5);

    (*ptcr)->close_path();
    (*ptcr)->stroke();

    if (show_grid && delta <= 501) {
        double red(current_color_scheme[ref_color].fg.r);
        double green(current_color_scheme[ref_color].fg.g);
        double blue(current_color_scheme[ref_color].fg.b);
        (*ptcr)->set_source_rgb(ghost_color*red, ghost_color*green, ghost_color*blue);

        unsigned step;
        if (delta <= 201)
            step = 1;
        else if (delta <= 501)
            step = 5;

        for (double i(-0.5); i<=xMax-0.5; i+=step) {
            if ((int)i % 5 == 0)
                (*ptcr)->set_line_width(0.1);
            else
                (*ptcr)->set_line_width(0.05);
            (*ptcr)->stroke();
            (*ptcr)->move_to(i, -0.5);
            (*ptcr)->line_to(i, yMax-0.5);
        }
        for (double j(-0.5); j<=yMax-0.5; j+=step) {
            if ((int)j % 5 == 0)
                (*ptcr)->set_line_width(0.1);
            else
                (*ptcr)->set_line_width(0.05);
            (*ptcr)->stroke();
            (*ptcr)->move_to(-0.5, j);
            (*ptcr)->line_to(xMax-0.5, j);
        }
        (*ptcr)->stroke();
    }
}

void graphic_draw_cell(unsigned x, unsigned y, unsigned ref_color, 
                       float fg_r_offset, float fg_g_offset, float fg_b_offset) {
    if (current_color_scheme[ref_color].fg.r >= 0.5
        && current_color_scheme[ref_color].fg.g >= 0.5
        && current_color_scheme[ref_color].fg.b >= 0.5) {
        fg_r_offset *= -1;
        fg_g_offset *= -1;
        fg_b_offset *= -1;
    }
    (*ptcr)->set_source_rgb(current_color_scheme[ref_color].fg.r+fg_r_offset, 
                            current_color_scheme[ref_color].fg.g+fg_g_offset,
                            current_color_scheme[ref_color].fg.b+fg_b_offset);
    (*ptcr)->set_line_width(0.0001);

    (*ptcr)->move_to(x - cell_size/2., y - cell_size/2.);
    (*ptcr)->line_to(x - cell_size/2., y + cell_size/2.);
    (*ptcr)->line_to(x + cell_size/2., y + cell_size/2.);
    (*ptcr)->line_to(x + cell_size/2., y - cell_size/2.);

    (*ptcr)->close_path();
    (*ptcr)->fill_preserve();
    (*ptcr)->stroke();
}

void graphic_fade_dead(unsigned x, unsigned y, const Color gray) {

    (*ptcr)->set_source_rgb(gray.r, gray.g, gray.b);
    (*ptcr)->set_line_width(0.0001);

    (*ptcr)->move_to(x - cell_size/2., y - cell_size/2.);
    (*ptcr)->line_to(x - cell_size/2., y + cell_size/2.);
    (*ptcr)->line_to(x + cell_size/2., y + cell_size/2.);
    (*ptcr)->line_to(x + cell_size/2., y - cell_size/2.);

    (*ptcr)->close_path();
    (*ptcr)->fill_preserve();
    (*ptcr)->stroke();
}

void graphic_ghost_pattern(unsigned x, unsigned y, 
                           std::vector<Coordinates> cells, unsigned ref_color) {
    (*ptcr)->move_to(x, y);
    for (auto& e : cells) {
        graphic_draw_cell(x + e.x, y + e.y, ref_color, -ghost_color, -ghost_color, 0.5);
    }
    (*ptcr)->stroke();
}

void graphic_draw_select_rec(unsigned x_0, unsigned y_0, unsigned x, 
                             unsigned y, unsigned ref_color) {
    (*ptcr)->set_source_rgba(current_color_scheme[ref_color].fg.r*ghost_color,
                             current_color_scheme[ref_color].fg.g*ghost_color,
                             current_color_scheme[ref_color].fg.b,
                             0.5);

    if (current_color_scheme[ref_color].fg.r == 0
        && current_color_scheme[ref_color].fg.g == 0 
        && current_color_scheme[ref_color].fg.b == 0) {
        (*ptcr)->set_source_rgba(current_color_scheme[ref_color].fg.r,
                                 current_color_scheme[ref_color].fg.g,
                                 current_color_scheme[ref_color].fg.b+ghost_color/2.,
                                 0.5);
    }

    double width = (double)x - (double)x_0;
    double height = (double)y - (double)y_0;

    if (width == 0 && height == 0)
        return;

    double x_corner(width > 0 ? -(double)cell_size/2 : (double)cell_size/2);
    double y_corner(height > 0 ? -(double)cell_size/2 : (double)cell_size/2);
    (*ptcr)->move_to(x_0 + x_corner, y_0 + y_corner);
    (*ptcr)->line_to(x_0 - x_corner + width, y_0 + y_corner);
    (*ptcr)->line_to(x_0 - x_corner + width, y_0 - y_corner + height);
    (*ptcr)->line_to(x_0 + x_corner, y_0 - y_corner + height);

    (*ptcr)->close_path();
    (*ptcr)->fill_preserve();
    (*ptcr)->stroke();
}

void graphic_highlight_selected_cells(std::vector<Coordinates> selected_cells, unsigned ref_color) {
    for (auto& e : selected_cells) {
        graphic_draw_cell(e.x, e.y, ref_color, 0.0, 0.6, 0.3);
    }
}

void graphic_change_light_color_scheme(unsigned id) {
    current_color_scheme[0] = color_scheme_table[id];
}

void graphic_change_dark_color_scheme(unsigned id) {
    current_color_scheme[1] = color_scheme_table[id];
}
