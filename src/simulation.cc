/*
 *  simulation.cc -- GoL Lab -- GUI with various options and view controls
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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>
#include "simulation.h"
#include "config.h"

namespace {
    enum reading_State { WORLD_SIZE, NB_CELLS, COORDINATES, END, OK };
    enum Error_reading { READING_OPENING, READING_WORLDSIZE, READING_END, BAD_EXTENSION };

    struct RLEWidthHeight {
        unsigned width;
        unsigned height;
    };

    typedef std::vector<std::vector<bool>> Grid;

    void line_decoding(std::string line);
    void error(Error_reading code);
    RLEWidthHeight get_rle_width_height(std::string rle_header);
    void decode_rle_body(unsigned width, unsigned height, std::string rle_body, std::vector<Pos>& cells);

    void fade_update();
    unsigned neighbours(unsigned x, unsigned y);
    void birth_test(unsigned x, unsigned y);

    void print_selection(unsigned i_min, unsigned i_max, unsigned y_min, unsigned y_max);

    Grid grid(Conf::world_y_max, std::vector<bool>(Conf::world_size));
    Grid updated_grid(Conf::world_y_max, std::vector<bool>(Conf::world_size));
    std::vector<Pos> dead, dead2, dead3, dead4;

    unsigned nb_alive(0);
    int nb_born(0);
    int past_alive(0);
    int past_2_alive(0);
    int past_3_alive(0);
    int past_4_alive(0);
    int past_5_alive(0);
    int nb_dead(0);
    bool stable(false);
    bool past_stable(false);
    bool fade_effect_enabled(false);
    int state(WORLD_SIZE);
    unsigned i(0), total(0);
    unsigned x(0), y(0), w_size(0);
    int error_id(0);
    bool success(true);
}

std::string filename_from_filepath(std::string filepath) {
    if (filepath.find_last_of('\\') != std::string::npos) {
        unsigned pos = filepath.find_last_of('\\');
        return filepath.replace(0, pos + 1, "");
    }
    if (filepath.find_last_of('/') != std::string::npos) {
        unsigned pos = filepath.find_last_of('/');
        return filepath.replace(0, pos + 1, "");
    }
    return filepath;
}

bool read_file(std::string filename) {
    state = WORLD_SIZE;
    i = 0;
    total = 0;
    error_id = 0;
    x = 0;
    y = 0;
    w_size = 0;

    std::string line;
    std::ifstream file(filename);
    if (!file.fail()) {
        // The file must be a text file
        unsigned pos(filename.find_last_of("."));
        std::string extension(filename.substr(pos + 1));
        if (extension != "txt") {
            error(BAD_EXTENSION);
            return false;
        }
        // Read the file line by line, ignoring those starting with '#'
        while (getline(file >> std::ws, line)) {
            if (line[0] == '#') continue;
            line_decoding(line);
        }
        line_decoding("last_line");
        if (success) {
            error_id = 0;
            return true;
        }else {
            success = true;
            return false;
        }
    }else {
        error(READING_OPENING);
        return false;
    }
}



void save_file(std::string filename) {
    std::ofstream saved_file;
    unsigned nb_born__(0);
    
    for (auto e : updated_grid) {
        for (auto f : e) {
            if (f) ++nb_born__;
        }
    }

    std::string name(filename_from_filepath(filename));
    saved_file.open(filename);

    saved_file << "# ========== Configuration File for Gol Lab";
    saved_file << " ========== \n#\n# " << name << "\n";
    saved_file << "# Last edited on "<< __DATE__ << "\n#\n\n";
    saved_file << "# world size:\n";
    saved_file << Conf::world_size << "\n";
    saved_file << "# alive cells:\n";
    saved_file << nb_born__ << "\n";
    saved_file << "# coordinates:\n";
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            if (updated_grid[i][j]) {
                saved_file << j << " " << Conf::world_y_max-1-i << "\n";
            }
        }
    }
    saved_file.close();
}



std::vector<Pos> get_rle_data(std::string filename) {
    std::vector<Pos> cells;
    std::string line;
    std::ifstream file(PATTERNS_DIR + filename);
    std::cout << PATTERNS_DIR + filename << "\n";
    
    if (!file.fail()) {
        std::cout << "OK\n";

        unsigned width(0), height(0);
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
            if (line[0] == '#') continue;    
            width = get_rle_width_height(line).width;
            height = get_rle_width_height(line).height;
            break;
        }
        decode_rle_body(width, height, rle_body, cells);
    }
    return cells;
}

std::vector<Pos> get_live_cells_in_area(unsigned x_min, unsigned x_max,
                                        unsigned y_min, unsigned y_max) {
    unsigned i_min(Conf::world_y_max - 1 - y_max);
    unsigned i_max(Conf::world_y_max- 1 - y_min);
    unsigned j_min(x_min);
    unsigned j_max(x_max);

    // print_selection(i_min, i_max, j_min, j_max);

    std::vector<Pos> live_cells_in_area;
    for (unsigned i(i_min); i <= i_max; ++i) {
        for (unsigned j(j_min); j <= j_max; ++j) {
            if (updated_grid[i][j])
                live_cells_in_area.push_back({j, Conf::world_y_max - 1 - i});
        }
    }
    return live_cells_in_area;
}

unsigned get_error_id() {
    return error_id;
}

unsigned get_alive() {
    return nb_alive;
}

void adjust_bool_grid() {
    if (Conf::world_size < grid.size()) {
        unsigned len(grid.size());
        for (unsigned i(Conf::world_y_max); i < len; ++i) {
            grid.pop_back();
            updated_grid.pop_back();
        }
        for (unsigned i(0); i < grid.size(); ++i) {
            unsigned len(grid[i].size());
            for (unsigned j(Conf::world_size); j < len; ++j) {
                grid[i].pop_back();
                updated_grid[i].pop_back();
            }
        }
    }else {
        unsigned len(grid.size());
        for (unsigned i(grid.size()); i < Conf::world_y_max; ++i) {
            grid.push_back(std::vector<bool>(Conf::world_size, false));
            updated_grid.push_back(std::vector<bool>(Conf::world_size, false));
        }
        for (unsigned i(0); i < len; ++i) {
            for (unsigned j(len); j < Conf::world_size; ++j) {
                grid[i].push_back(false);
                updated_grid[i].push_back(false);
            }
        }
    }
}

void new_birth(unsigned x, unsigned y) {
    if (!updated_grid[Conf::world_y_max - 1 - y][x]) {
        updated_grid[Conf::world_y_max - 1 - y][x] = true;
        ++nb_born;
        if (!grid[Conf::world_y_max-1-y][x]) {
            ++nb_alive;
        }
    }
    
}

void new_death(unsigned x, unsigned y) {
    if (updated_grid[Conf::world_y_max - 1 - y][x]) {
        updated_grid[Conf::world_y_max - 1 - y][x] = false;
        --nb_alive;
    }
}

void new_pattern(unsigned x, unsigned y, std::vector<Pos> pattern) {
    for (auto& e : pattern) {
        if (x + e.x >= 0 && x + e.x < Conf::world_size - 1 &&
            y + e.y >= 0 && y + e.y < Conf::world_y_max - 1) {
            new_birth(x + e.x, y + e.y);
        }
    }
}

void del_pattern(unsigned x, unsigned y, std::vector<Pos> pattern) {
    for (auto& e : pattern) {
        if (e.x >= 0 && e.x < Conf::world_size - 1 &&
            e.y >= 0 && e.y < Conf::world_y_max - 1) {
            new_death(x + e.x, y + e.y);
        }
    }
}

bool update(Mode mode) {
    past_5_alive = past_4_alive;
    past_4_alive = past_3_alive;
    past_3_alive = past_2_alive;
    past_2_alive = past_alive;
    past_alive = nb_born;
    nb_born = 0;
    nb_dead = 0;
    past_stable = stable;
    stable = false;

    // Update vectors for fade effect
    if (fade_effect_enabled) fade_update();

    // Update all cells
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            grid[i][j] = updated_grid[i][j];
        }
    }
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            updated_grid[i][j] = false;
            birth_test(j, Conf::world_y_max - 1 - i);
        }
    }
    // 5-perdiodic oscillations detection
    if (mode == EXPERIMENTAL) {
        if (nb_born == past_alive && past_alive == past_2_alive && past_2_alive == past_3_alive) {
            stable = true;
        }
        if (nb_born == past_2_alive && past_alive == past_3_alive && past_2_alive == past_4_alive) {
            stable = true;
        }
        if (nb_born == past_3_alive && past_alive == past_4_alive && past_2_alive == past_5_alive) {
            stable = true;
        }
        if (stable && past_stable) {
            return true;
        }
    }
    return false;
}

void draw_cells(unsigned color_theme) {

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
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            if (updated_grid[i][j]) {
                graphic_draw_cell(j, Conf::world_y_max - i -1, color_theme);
            }
        }
    }
}

void display() {
    for (unsigned i(0); i < grid.size(); ++i) {
        std::cout << "\n";
        for (unsigned j(0); j < grid[i].size(); ++j) {
            std::cout << updated_grid[i][j];
        }
    }
}
 
void init() {
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            grid[i][j] = false;
            updated_grid[i][j] = false;
        }
    }
    dead.clear();
    dead2.clear();
    dead3.clear();
    dead4.clear();

    nb_alive = 0;
}

void toggle_fade_effect() {
 if (fade_effect_enabled) {
    fade_effect_enabled = false;
    dead.clear();
    dead2.clear();
    dead3.clear();
    dead4.clear();
 }else {
    fade_effect_enabled = true;
 }
}

namespace {
    void line_decoding(std::string line) {
        std::istringstream data(line);
        switch(state) {
        case WORLD_SIZE:
            if (!(data >> w_size)) {
                state = END;
                break;
            }
            else ++i;
            if (w_size > world_size_max || w_size < world_size_min) {
                state = END;
                break;
            }else {
                Conf::set_world_size(w_size);
                if (w_size < grid.size()) {
                    unsigned len(grid.size());
                    for (unsigned i(Conf::world_y_max); i < len; ++i) {
                        grid.pop_back();
                        updated_grid.pop_back();
                    }
                    for (unsigned i(0); i < grid.size(); ++i) {
                        unsigned len(grid[i].size());
                        for (unsigned j(Conf::world_size); j < len; ++j) {
                            grid[i].pop_back();
                            updated_grid[i].pop_back();
                        }
                    }
                }else {
                    unsigned len(grid.size());
                    for (unsigned i(grid.size()); i < Conf::world_y_max; ++i) {
                        grid.push_back(std::vector<bool>(w_size, false));
                        updated_grid.push_back(std::vector<bool>(w_size, false));
                    }
                    for (unsigned i(0); i < len; ++i) {
                        for (unsigned j(len); j < w_size; ++j) {
                            grid[i].push_back(false);
                            updated_grid[i].push_back(false);
                        }
                    }
                }
                state = NB_CELLS;
            }
            if (i == 1) state = NB_CELLS;
            break;
        case NB_CELLS:
            data >> total;
            if (total < 0 || total > world_size_max*world_size_max) {
                state = END;
            }
            else state = COORDINATES;
            break;
        case COORDINATES:
            data >> x >> y;
            if (x < 0 || y < 0 || x >= w_size || y >= w_size) {
                error(READING_END);
            }
            else new_birth(x, y);
            ++i;
            if (i == total + 1) {
                state = OK;
            }else state = COORDINATES;
            break;
        case END:
            error(READING_END);
            break;
        case OK:
            break;
        default:
            break;
        }
    }

    void error(Error_reading code) {
        switch(code) {
            case READING_OPENING:
                error_id = 1;
                success = false;
                break;
            case READING_WORLDSIZE:
                error_id = 2;
                success = false;
                break;
            case READING_END: 
                error_id = 3;
                success = false;
                init();
                break;
            case BAD_EXTENSION:
                error_id = 4;
                success = false;
                init();
                break;
            default:
                error_id = 5;
                success = false;
        }
    }

    RLEWidthHeight get_rle_width_height(std::string line) {
        auto x = line.find("x=");
        auto y = line.find("y=");

        unsigned width = std::stoi(std::string(x+2, y));
        unsigned height = std::stoi(std::string(y+2, line.length()));

        return {width, height};
    }

    void decode_rle_body(unsigned x, unsigned y, std::string rle_body, std::vector<Pos>& cells) {
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
        if (!grid[Conf::world_y_max - 1 - y][x]) {
            if (neighbours(x, y) == 3) {
                new_birth(x, y);
            }
        }else {
            if (neighbours(x, y) == 2 or neighbours(x, y) == 3) {
                new_birth(x, y);
                --nb_born;
            }else {
                if (fade_effect_enabled) {
                    dead.push_back({x, y});
                }
                ++nb_dead;
                --nb_alive;
            }
        }
    }

    unsigned neighbours(unsigned x, unsigned y) {
        unsigned n(0);

        if (x == 0 and y == 0) {
            if (grid[Conf::world_y_max-2][y]) ++n;
            if (grid[Conf::world_y_max-2][y+1]) ++n;
            if (grid[Conf::world_y_max-1][y+1]) ++n;
            return n;
        }
        if (x == Conf::world_size-1 and y == 0) {
            if (grid[Conf::world_y_max-2][Conf::world_size-1]) ++n;
            if (grid[Conf::world_y_max-2][Conf::world_size-2]) ++n;
            if (grid[Conf::world_y_max-1][Conf::world_size-2]) ++n;
            return n;
        }
        if (x == Conf::world_size-1 and y == Conf::world_y_max-1) {
            if (grid[0][Conf::world_size-2]) ++n;
            if (grid[1][Conf::world_size-2]) ++n;
            if (grid[1][Conf::world_size-1]) ++n;
            return n;
        }
        if (x == 0 and y == Conf::world_y_max-1) {
            if (grid[1][0]) ++n;
            if (grid[1][1]) ++n;
            if (grid[0][1]) ++n;
            return n;
        }
        if (x == 0 and y != 0 and y != Conf::world_y_max-1) {
            if (grid[Conf::world_y_max-y][x]) ++n;
            if (grid[Conf::world_y_max-y-2][x]) ++n;
            if (grid[Conf::world_y_max-y][x+1]) ++n;
            if (grid[Conf::world_y_max-y-1][x+1]) ++n;
            if (grid[Conf::world_y_max-y-2][x+1]) ++n;
            return n;
        }
        if (y == 0 and x != 0 and x != Conf::world_size-1) {
            if (grid[Conf::world_y_max-1][x-1]) ++n;
            if (grid[Conf::world_y_max-1][x+1]) ++n;
            if (grid[Conf::world_y_max-2][x-1]) ++n;
            if (grid[Conf::world_y_max-2][x]) ++n;
            if (grid[Conf::world_y_max-2][x+1]) ++n;
            return n;
        }
        if (x == Conf::world_size-1 and y != 0 and y != Conf::world_y_max-1) {
            if (grid[Conf::world_y_max-y][x]) ++n;
            if (grid[Conf::world_y_max-y-2][x]) ++n;
            if (grid[Conf::world_y_max-y][x-1]) ++n;
            if (grid[Conf::world_y_max-y-1][x-1]) ++n;
            if (grid[Conf::world_y_max-y-2][x-1]) ++n;
            return n;
        }
        if (y == Conf::world_y_max-1 and x != 0 and x != Conf::world_size-1) {
            if (grid[0][x-1]) ++n;
            if (grid[0][x+1]) ++n;
            if (grid[1][x-1]) ++n;
            if (grid[1][x]) ++n;
            if (grid[1][x+1]) ++n;
            return n;
        }
        if (grid[Conf::world_y_max - y - 2][x - 1]) ++n;
        if (grid[Conf::world_y_max - y - 2][x]) ++n;
        if (grid[Conf::world_y_max - y - 2][x + 1]) ++n;
        if (grid[Conf::world_y_max - 1 - y][x - 1]) ++n;
        if (grid[Conf::world_y_max - 1 - y][x + 1]) ++n;
        if (grid[Conf::world_y_max - y][x - 1]) ++n;
        if (grid[Conf::world_y_max - y][x]) ++n;
        if (grid[Conf::world_y_max - y][x + 1]) ++n;

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
}
