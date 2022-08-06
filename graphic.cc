#include "graphic_gui.h"
#include "config.h"

#include <vector>
#include <iostream>

static const std::vector<Color> color_table({black, white, gray});
static const Cairo::RefPtr<Cairo::Context>* ptcr(nullptr);

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
        (*ptcr)->set_line_width(0.05);
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

void enable_show_grid() {
    show_grid = true;
}

void disable_show_grid() {
    show_grid = false;
}
