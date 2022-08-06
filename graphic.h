#ifndef GRAPHIC_H_INCLUDED
#define GRAPHIC_H_INCLUDED

struct Color {
    double r;
    double g;
    double b;
};

static bool show_grid(false);

constexpr Color black({0.0, 0.0, 0.0});
constexpr Color white({1.0, 1.0, 1.0});
constexpr Color gray({0.5, 0.5, 0.5});

void graphic_draw_world(double xMax, double yMax, unsigned ref_color);
void graphic_draw_cell(unsigned x, unsigned y, unsigned ref_color);

void enable_show_grid();
void disable_show_grid();

#endif