#ifndef CONFIG_H
#define CONFIG_H

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
