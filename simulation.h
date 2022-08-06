#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include "config.h"
#include <iostream>
#include <vector>
#include <string>

enum Error_reading { READING_OPENING, READING_WORLDSIZE, READING_END, BAD_EXTENSION };
enum Mode { EXPERIMENTAL, NORMAL };

void save_file(std::string filename);
bool read_file(std::string filename);
void line_decoding(std::string line);
void error(Error_reading code);
unsigned get_error_id();

void display();
void init();
bool update(Mode mode = NORMAL);
void adjust_zoom();

void draw_canon_planeur(unsigned x, unsigned y);
void erase_canon_planeur(unsigned x, unsigned y);

void draw_spacefiller(unsigned x, unsigned y);
void erase_spacefiller(unsigned x, unsigned y);

unsigned neighbours(unsigned x, unsigned y);
void birth_test(unsigned x, unsigned y);

void new_birth(unsigned x, unsigned y);
void new_death(unsigned x, unsigned y);

void draw_world(unsigned color_theme);

#endif