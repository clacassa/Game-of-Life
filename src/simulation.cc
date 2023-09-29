/*
 * simulation.cc
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

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include "simulation.h"
#include "config.h"

namespace {
    enum ReadingStatus { 
        OK = 0,
        INEXISTANT_FILE,
        INVALID_FORMAT,
        INVALID_DATA,
        COORDINATE_OUT_OF_RANGE
    };

    struct ErrorInfo {
        ReadingStatus type;
        unsigned line_number;
        std::string line;
    };

    struct RLEWidthHeight {
        unsigned width;
        unsigned height;
    };

    typedef std::vector<Coordinates> LineBuffer;

    ReadingStatus line_decoding(std::string line, std::vector<Coordinates>& initial_state);
    RLEWidthHeight get_rle_width_height(std::string rle_header);
    void decode_rle_body(unsigned width, unsigned height, std::string rle_body,
                         std::vector<Coordinates>& cells);

    void fade_update();
    unsigned neighbours(unsigned x, unsigned y);
    void birth_test(unsigned x, unsigned y);

    void print_selection(unsigned i_min, unsigned i_max, unsigned y_min, unsigned y_max);

    Grid grid(World::get_y_max(), std::vector<bool>(World::get_x_max()));
    Grid updated_grid(World::get_y_max(), std::vector<bool>(World::get_x_max()));
#ifdef LIVE_ARRAY_OPTIMIZATION
    LineBuffer live_cells_buffer;
#endif

    std::vector<Coordinates> dead, dead2, dead3, dead4;

    ErrorInfo error = {OK, 0, ""};

    unsigned nb_alive(0);

    int dead_count(0);
    int born_count(0);
    int past_alive(0);
    int past_2_alive(0);
    int past_3_alive(0);
    int past_4_alive(0);
    int past_5_alive(0);
    bool stable(false);
    bool past_stable(false);

    bool fade_effect_enabled(false);

    unsigned x(0);
    unsigned y(0);
    unsigned x_max(0);
    unsigned y_max(0);
} /* unnamed namespace */

std::string simulation::remove_filepath(std::string filename) {
    if (filename.find_last_of('\\') != std::string::npos) {
        unsigned pos = filename.find_last_of('\\');
        return filename.replace(0, pos + 1, "");
    }
    if (filename.find_last_of('/') != std::string::npos) {
        unsigned pos = filename.find_last_of('/');
        return filename.replace(0, pos + 1, "");
    }
    return filename;
}

int simulation::read_file(std::string filename) {
    x = 0;
    y = 0;
    x_max = 0;
    y_max = 0;
    std::string line;
    std::ifstream file(filename);
    ReadingStatus result(OK);

    if (!file.fail()) {
        // The file must be a lif file
        unsigned pos(filename.find_last_of("."));
        std::string extension(filename.substr(pos + 1));
        if (extension != "lif" && extension != "life")
            return INVALID_FORMAT;

        std::vector<Coordinates> initial_state;
        // Read the file line by line, ignoring the ones starting with '#'
        while (getline(file >> std::ws, line)) {
            ++error.line_number;
            if (line[0] == '#' || line.empty())
                continue;
            result = line_decoding(line, initial_state);
            if (result != OK) {
                error.line = line;
                error.type = result;
                if (error.type != COORDINATE_OUT_OF_RANGE)
                    return error.type;
            }
        }

        unsigned world_width(0);
        if (x_max >= y_max*2) {
            do {
                world_width += increment_step;
            }while (world_width <= x_max);
        }else {
            do {
                world_width += increment_step;
            }while (world_width <= y_max*2);
        }
        World::set_world_size(world_width);
        adjust_bool_grid();
        new_pattern(0, 0, initial_state);

        return error.type;
    }
    return INEXISTANT_FILE;
}

void simulation::save_file(std::string filename) {
    std::ofstream saved_file;

    std::cout << "file: " + filename + "\n";
    std::string name(remove_filepath(filename));
    saved_file.open(filename);

    saved_file << "# ========== Configuration File for Gol Lab ==========\n"
                  "#\n"
                  "# File name: " << name << "\n"
                  "# Edited on: "<< __DATE__ << "\n"
                  "# Format: Life 1.06\n"
                  "#\n"
                  "# ====================================================\n\n"
                  "# live cells coordinates\n";

#ifdef LIVE_ARRAY_OPTIMIZATION
    for (auto cell : live_cells_buffer) {
        saved_file << cell.x << " " << cell.y << "\n";
    }
#else
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            if (updated_grid[i][j]) {
                saved_file << j << " " << Conf::get_y_max()-1-i << "\n";
            }
        }
    }
#endif

    saved_file.close();
}

std::vector<Coordinates> simulation::get_rle_data(std::string filename) {
    std::vector<Coordinates> cells;
    std::string line;
    std::ifstream file(filename);
    std::cout << filename << "\n";
    
    if (!file.fail()) {
        std::cout << "OK\n";

        unsigned width(0);
        unsigned height(0);
        std::string rle_string, rle_body;
        std::ostringstream ss;
        ss << file.rdbuf();
        rle_string = ss.str();

        if (rle_string.find("b3/s23") != std::string::npos) {
            const size_t start_pos = rle_string.find("b3/s23");
            rle_body = rle_string.substr(start_pos+6, rle_string.length());
        }else if (rle_string.find("B3/S23") != std::string::npos) {
            const size_t start_pos = rle_string.find("B3/S23");
            rle_body = rle_string.substr(start_pos+6, rle_string.length());
        }

        while (getline(file, line)) {
            if (line[0] == '#')
                continue;    
            width = get_rle_width_height(line).width;
            height = get_rle_width_height(line).height;
            break;
        }
        decode_rle_body(width, height, rle_body, cells);
    }
    return cells;
}

std::vector<Coordinates> simulation::get_live_cells_in_area(unsigned x_min, unsigned x_max,
                                                unsigned y_min, unsigned y_max) {
    unsigned i_min(World::get_y_max() - 1 - y_max);
    unsigned i_max(World::get_y_max()- 1 - y_min);
    unsigned j_min(x_min);
    unsigned j_max(x_max);

    // print_selection(i_min, i_max, j_min, j_max);

    std::vector<Coordinates> live_cells_in_area;
    for (unsigned i(i_min); i <= i_max; ++i) {
        for (unsigned j(j_min); j <= j_max; ++j) {
            if (updated_grid[i][j])
                live_cells_in_area.push_back({j, World::get_y_max() - 1 - i});
        }
    }
    return live_cells_in_area;
}

bool simulation::update(Mode mode) {
    past_5_alive = past_4_alive;
    past_4_alive = past_3_alive;
    past_3_alive = past_2_alive;
    past_2_alive = past_alive;
    past_alive = born_count;
    born_count = 0;
    dead_count = 0;
    past_stable = stable;
    stable = false;

    // Update vectors for fade effect
    if (fade_effect_enabled)
        fade_update();   

#ifdef LIVE_ARRAY_OPTIMIZATION
    LineBuffer temp_buffer(live_cells_buffer);

    for (auto& row : grid) {
        for (unsigned j(0); j < row.size(); ++j) {
            row[j] = 0;
        }
    }
    for (auto cell : temp_buffer) {
        grid[World::get_y_max() - 1 - cell.y][cell.x] = 1;
    }

    for (auto& cell : temp_buffer) {
        for (int x_offset(-1); x_offset <= 1; ++x_offset) {
            for (int y_offset(-1); y_offset <= 1; ++y_offset) {

                int x_neighb(cell.x+x_offset);
                int y_neighb(cell.y+y_offset);

                if (x_neighb >= 0 && x_neighb < World::get_x_max() &&
                    y_neighb >= 0 && y_neighb < World::get_y_max()) {
                    if (grid[World::get_y_max()-1-y_neighb][x_neighb]
                        && (x_offset != 0 || y_offset != 0))
                        continue;
                    birth_test(x_neighb, y_neighb);
                }
            }
        }
    }
    nb_alive = live_cells_buffer.size();
#else
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            grid[i][j] = updated_grid[i][j];
        }
    }
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            updated_grid[i][j] = false;
            birth_test(j, Conf::get_y_max() - 1 - i);
        }
    }
#endif // LIVE_ARRAY_OPTIMIZATION

    // 5-perdiodic oscillations detection
    if (mode == EXPERIMENTAL) {
        if (born_count == past_alive && past_alive == past_2_alive && past_2_alive == past_3_alive)
            stable = true;
        if (born_count == past_2_alive && past_alive == past_3_alive && past_2_alive == past_4_alive)
            stable = true;
        if (born_count == past_3_alive && past_alive == past_4_alive && past_2_alive == past_5_alive)
            stable = true;
        if (stable && past_stable)
            return true;
    }
    return false;
}

void simulation::init() {
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            grid[i][j] = false;
            updated_grid[i][j] = false;
        }
    }
#ifdef LIVE_ARRAY_OPTIMIZATION
    live_cells_buffer.clear();
#endif
    dead.clear();
    dead2.clear();
    dead3.clear();
    dead4.clear();

    nb_alive = 0;
}

bool simulation::is_alive(unsigned x, unsigned y) {
    return updated_grid[World::get_y_max() - 1 - y][x];
}

unsigned simulation::get_population() {
    return nb_alive;
}

void simulation::adjust_bool_grid() {
    if (World::get_y_max() < grid.size()) {
        unsigned len(grid.size());
        for (unsigned i(World::get_y_max()); i < len; ++i) {
            grid.pop_back();
            updated_grid.pop_back();
        }
        for (unsigned i(0); i < grid.size(); ++i) {
            unsigned len(grid[i].size());
            for (unsigned j(World::get_x_max()); j < len; ++j) {
                grid[i].pop_back();
                updated_grid[i].pop_back();
            }
        }
    }else {
        unsigned len(grid.size());
        for (unsigned i(grid.size()); i < World::get_y_max(); ++i) {
            grid.push_back(std::vector<bool>(World::get_x_max(), false));
            updated_grid.push_back(std::vector<bool>(World::get_x_max(), false));
        }
        unsigned x_len(grid[0].size());
        for (unsigned i(0); i < len; ++i) {
            for (unsigned j(x_len); j < World::get_x_max(); ++j) {
                grid[i].push_back(false);
                updated_grid[i].push_back(false);
            }
        }
    }    
#ifdef LIVE_ARRAY_OPTIMIZATION
    live_cells_buffer.clear();
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            if (updated_grid[i][j])
                live_cells_buffer.push_back({j, World::get_y_max()-1-i});
        }
    }
#endif
}

void simulation::new_birth(unsigned x, unsigned y) {
    if (!updated_grid[World::get_y_max() - 1 - y][x]) {
        updated_grid[World::get_y_max() - 1 - y][x] = true;
#ifdef LIVE_ARRAY_OPTIMIZATION
        live_cells_buffer.push_back({x, y});
        nb_alive = live_cells_buffer.size();
#endif
    }
}

void simulation::new_death(unsigned x, unsigned y) {
    if (updated_grid[World::get_y_max() - 1 - y][x]) {
        updated_grid[World::get_y_max() - 1 - y][x] = false;
#ifdef LIVE_ARRAY_OPTIMIZATION
        for (size_t i(0); i < live_cells_buffer.size(); ++i) {
            if (live_cells_buffer[i].x == x && live_cells_buffer[i].y == y) {
                live_cells_buffer.erase(live_cells_buffer.begin() + i);
                break;
            }
        }
        nb_alive = live_cells_buffer.size();
#endif
    }
}

void simulation::new_pattern(unsigned x, unsigned y, std::vector<Coordinates> pattern) {
    for (auto& e : pattern) {
        if (x + e.x >= 0 && x + e.x <= World::get_x_max() - 1 &&
            y + e.y >= 0 && y + e.y <= World::get_y_max() - 1) {
            new_birth(x + e.x, y + e.y);
        }
    }
}

void simulation::del_pattern(unsigned x, unsigned y, std::vector<Coordinates> pattern) {
    for (auto& e : pattern) {
        if (e.x >= 0 && e.x <= World::get_x_max() - 1 &&
            e.y >= 0 && e.y <= World::get_y_max() - 1) {
            new_death(x + e.x, y + e.y);
        }
    }
}

void simulation::draw_cells(unsigned color_theme) {

    if (fade_effect_enabled) {
        if (color_theme) {
            for (unsigned i(0); i < dead4.size(); ++i) {
                graphic_fade_dead(dead4[i].x, dead4[i].y, gray4);
            }
            for (unsigned i(0); i < dead3.size(); ++i) {
                graphic_fade_dead(dead3[i].x, dead3[i].y, gray3);
            }
            for (unsigned i(0); i < dead2.size(); ++i) {
                graphic_fade_dead(dead2[i].x, dead2[i].y, gray2);
            }
            for (unsigned i(0); i < dead.size(); ++i) {
                graphic_fade_dead(dead[i].x, dead[i].y, gray1);
            }
        }else {
            for (unsigned i(0); i < dead4.size(); ++i) {
                graphic_fade_dead(dead4[i].x, dead4[i].y, gray1);
            }
            for (unsigned i(0); i < dead3.size(); ++i) {
                graphic_fade_dead(dead3[i].x, dead3[i].y, gray2);
            }
            for (unsigned i(0); i < dead2.size(); ++i) {
                graphic_fade_dead(dead2[i].x, dead2[i].y, gray3);
            }
            for (unsigned i(0); i < dead.size(); ++i) {
                graphic_fade_dead(dead[i].x, dead[i].y, gray4);
            } 
        }
    }
#ifdef LIVE_ARRAY_OPTIMIZATION
    for (auto cell : live_cells_buffer) {
        graphic_draw_cell(cell.x, cell.y, color_theme);
    }
#else
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            if (updated_grid[i][j]) {
                graphic_draw_cell(j, World::get_y_max() - i -1, color_theme);
            }
        }
    }
#endif
}

void simulation::display() {
    std::cout << "\n------GRID------\n";
    for (unsigned i(0); i < grid.size(); ++i) {
        std::cout << "\n";
        for (unsigned j(0); j < grid[i].size(); ++j) {
            std::cout << grid[i][j];
        }
    }
    std::cout << "\n------UPDATED GRID------\n";
    for (unsigned i(0); i < grid.size(); ++i) {
        std::cout << "\n";
        for (unsigned j(0); j < grid[i].size(); ++j) {
            std::cout << updated_grid[i][j];
        }
    }
}

void simulation::toggle_fade_effect() {
    if (fade_effect_enabled) {
        fade_effect_enabled = false;
        dead.clear();
        dead2.clear();
        dead3.clear();
        dead4.clear();
    }else
        fade_effect_enabled = true;
}

Grid simulation::get_state() {
    return updated_grid;
}

void simulation::set_state(const Grid saved_state) {
    for (unsigned i(0); i < saved_state.size(); ++i) {
        for (unsigned j(0); j < saved_state[i].size(); ++j) {
            if (saved_state[i][j])
                new_birth(j, World::get_y_max() - 1 - i);
        }
    }
}

std::string message::file_does_not_exist(std::string filename) {
    return filename + " : no such file was found.\n"
           "Please check the spelling and the location of the file and try again.";
}

std::string message::invalid_file_format(std::string filename) {
    return "The file " + filename + " has an invalid format. Please use Life 1.06 format.";
}

std::string message::unusable_file_data(std::string filename) {
    return "File: " + filename + "\n"
           "Line " + std::to_string(error.line_number) + ":\t" + error.line + "\n\n"
           "The simulation could not be configured because the required informations were not "
           "found. The file contains unrelated data.";
}

std::string message::invalid_cell_coordinate(std::string filename) {
    return "File: " + filename + "\n"
           "Line " + std::to_string(error.line_number) + ":\t" + error.line +"\n\n"
           "At least one cell could not be added to the grid because at least one of its coordinates "
           "exceeds the world boundaries being [0, " + std::to_string(world_size_max - 1)
           + "] on x and [0, " + std::to_string(world_size_max/2 - 1) + "] on y.";
}

namespace {
    ReadingStatus line_decoding(std::string line, std::vector<Coordinates>& initial_state) {
        std::istringstream data(line);
        if (!(data >> x >> y)) {
            return INVALID_DATA;
        }
        // std::cout << "x: " << x << " y: " << y << "\n";
        if (x < 0 || y < 0 || x >= world_size_max || y >= world_size_max/2) {
            return COORDINATE_OUT_OF_RANGE;
        }
        if (x > x_max)
            x_max = x;
        if (y > y_max)
            y_max = y;
        initial_state.push_back({x, y});
        return OK;
    }

    RLEWidthHeight get_rle_width_height(std::string line) {
        auto x = line.find("x=");
        auto y = line.find("y=");

        unsigned width = std::stoi(std::string(x+2, y));
        unsigned height = std::stoi(std::string(y+2, line.length()));

        return {width, height};
    }

    void decode_rle_body(unsigned x, unsigned y, std::string rle_body,
                         std::vector<Coordinates>& cells) {
        unsigned row(y);
        unsigned col(x);
        unsigned cnt(0);
        char cod;
        for (unsigned ptr(0); ptr < rle_body.length(); ++ptr) {
            cod = rle_body[ptr];
            switch (cod) {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    cnt = cnt * 10 + (cod - '0');
                    break;
                case '$':
                case '!':
                    if (cnt == 0)
                        cnt = 1;
                    row += cnt;
                    col = x;
                    cnt = 0;
                    break;
                case 'b':
                    if (cnt == 0)
                        cnt = 1;
                    col += cnt;
                    cnt = 0;
                    break;
                case 'o':
                    if (cnt == 0)
                        cnt = 1;
                    while (cnt > 0) {
                        cells.push_back({col, row});
                        ++col;
                        --cnt;
                    }
                    cnt = 0;
                    break;
            }
        }
    }

    void fade_update() {
        dead4.clear();
        for (unsigned i(0); i < dead3.size(); ++i) {
            dead4.push_back(dead3[i]);
        }
        dead3.clear();
        for (unsigned i(0); i < dead2.size(); ++i) {
            dead3.push_back(dead2[i]);
        }
        dead2.clear();
        for (unsigned i(0); i < dead.size(); ++i) {
            dead2.push_back(dead[i]);
        }
        dead.clear();
    }

    void birth_test(unsigned x, unsigned y) {
        unsigned n(neighbours(x, y));
        if (!grid[World::get_y_max() - 1 - y][x]) {
            if (n == 3) {
                simulation::new_birth(x, y);
                ++born_count;
            }
        }else {
            if (n == 2 || n == 3) {
                simulation::new_birth(x, y);
                --born_count;
            }else {
                simulation::new_death(x, y);
                if (fade_effect_enabled)
                    dead.push_back({x, y});
                ++dead_count;
            }
        }
    }

    unsigned neighbours(unsigned x, unsigned y) {
        unsigned n(0);

        for (int x_offset(-1); x_offset <= 1; ++x_offset) {
            for (int y_offset(-1); y_offset <= 1; ++y_offset) {

                if (x_offset == 0 && y_offset == 0)
                    continue;

                int x_neighb(x+x_offset);
                int y_neighb(y+y_offset);

                if (x_neighb >= 0 && x_neighb < World::get_x_max() &&
                    y_neighb >= 0 && y_neighb < World::get_y_max())
                    if (grid[World::get_y_max()-1-y_neighb][x_neighb])
                        ++n;
            }
        }
        return n;
    }

    void print_selection(unsigned i_min, unsigned i_max, unsigned j_min, unsigned j_max) {
        unsigned w(j_max/5 - j_min/5);
        unsigned h(i_max/5 - i_min/5);
        if (w <= 40 && h <= 40) {
            unsigned off1(std::to_string(j_min).length());
            unsigned off2(std::to_string(j_max).length());
            unsigned off3(std::to_string(i_min).length());
            unsigned off4(std::to_string(i_max).length());
            unsigned j_offset(off1 > off2 ? off1 : off2);
            unsigned i_offset(off3 > off4 ? off3 : off4);

            std::cout << "\x1b[2J\x1b[H";
            // j_min______j_max
            for (unsigned i(0); i < i_offset; ++i) {
                std::cout << " ";
            }
            std::cout << j_min;
            if (j_offset > off1) {
                for (unsigned i(0); i < j_offset-off1; ++i) {
                    std::cout << "_";
                }
            }
            for (unsigned i(0); i < ((w-j_offset < 10000) ? 2*(w-j_offset)+off1 : 0); ++i) {
                std::cout << "_";
            }
            std::cout << j_max << "\n";
            // i_min|      |
            if (i_offset > off3) {
                for (unsigned i(0); i < i_offset-off3; ++i) {
                    std::cout << " ";
                }
            }
            std::cout << i_min << "|";
            for (unsigned i(0); i < 2*w; ++i) {
                std::cout << " ";
            }
            std::cout << "|\n";
            // |      |
            for (unsigned i(0); i < h; ++i) {
                for (unsigned k(0); k < i_offset; ++k) {
                    std::cout << " ";
                }
                std::cout << "|";
                for (unsigned j(0); j < 2*w; ++j) {
                    std::cout << " ";
                }
                std::cout << "|\n";
            }
            // i_max|______|
            std::cout << i_max << "|";
            for (unsigned i(0); i < 2*w; ++i) {
                std::cout << "_";
            }
            std::cout << "|\n\n";
        }
    }
} /* unnamed namespace */
