#include "simulation.h"
#include "graphic.h"
#include "config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <string>

typedef std::vector<std::vector<bool>> Grid;

Grid grid(reserve, std::vector<bool>(reserve));
Grid updated_grid(reserve, std::vector<bool>(reserve));

static int nb_alive(0);
static int past_alive(0);
static int past_2_alive(0);
static int past_3_alive(0);
static int past_4_alive(0);
static int past_5_alive(0);
static int nb_dead(0);
static bool stable(false);
static bool past_stable(false);

enum reading_State { WORLD_SIZE, NB_CELLS, COORDINATES, END, OK };
static int state(WORLD_SIZE);
static int i(0), total(0);
static int x(0), y(0), w_size(0);
static int error_id(0);
static bool success(true);

bool read_file(std::string filename) {
    state = WORLD_SIZE;
    i = 0;
    total = 0;
    x = 0;
    y = 0;
    w_size = 0;
    error_id = 0;

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
        }else success = true;
    }else {
        error(READING_OPENING);
        return false;
    }
}

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
            if (w_size < grid.size()) {
                unsigned len(grid.size());
                for (unsigned i(w_size); i < len; ++i) {
                    grid.pop_back();
                    updated_grid.pop_back();
                }
                for (unsigned i(0); i < grid.size(); ++i) {
                    unsigned len(grid[i].size());
                    for (unsigned j(w_size); j < len; ++j) {
                        grid[i].pop_back();
                        updated_grid[i].pop_back();
                    }
                }
            }else {
                unsigned len(grid.size());
                for (unsigned i(grid.size()); i < w_size; ++i) {
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
            Conf::set_world_size(grid.size());
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

void save_file(std::string filename) {
    std::ofstream saved_file;
    unsigned nb_alive__(0);
    
    for (auto e : updated_grid) {
        for (auto f : e) {
            if (f) ++nb_alive__;
        }
    }

    saved_file.open(filename);

    saved_file << "#------------------Configuration File";
    saved_file << "------------------#\n\n";
    saved_file << "#\tworld size:\n";
    saved_file << Conf::world_size << "\n\n";
    saved_file << "#\talive cells:\n";
    saved_file << nb_alive__ << "\n\n";
    saved_file << "#\tcoordinates:\n";
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            if (updated_grid[i][j]) {
                saved_file << j << " " << Conf::world_size-1-i << "\n";
            }
        }
    }
    saved_file.close();
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

unsigned get_error_id() {
    return error_id;
}

void adjust_zoom() {
    if (Conf::world_size < grid.size()) {
        unsigned len(grid.size());
        for (unsigned i(Conf::world_size); i < len; ++i) {
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
        for (unsigned i(grid.size()); i < Conf::world_size; ++i) {
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
    updated_grid[Conf::world_size -1 - y][x] = true;
    ++nb_alive;
}

void new_death(unsigned x, unsigned y) {
    updated_grid[Conf::world_size - 1 - y][x] = false;
}

void birth_test(unsigned x, unsigned y) {
    if (!grid[Conf::world_size - 1 - y][x]) {
        if (neighbours(x, y) == 3) {
            new_birth(x, y);
        }
    }else {
        if (neighbours(x, y) == 2 or neighbours(x, y) == 3) {
            new_birth(x, y);
            --nb_alive;
        }else {
            ++nb_dead;
        }
    }
}

unsigned neighbours(unsigned x, unsigned y) {
    unsigned n(0);

    if (x == 0 and y == 0) {
        if (grid[Conf::world_size-2][y]) ++n;
        if (grid[Conf::world_size-2][y+1]) ++n;
        if (grid[Conf::world_size-1][y+1]) ++n;
        return n;
    }
    if (x == Conf::world_size-1 and y == 0) {
        if (grid[Conf::world_size-2][Conf::world_size-1]) ++n;
        if (grid[Conf::world_size-2][Conf::world_size-2]) ++n;
        if (grid[Conf::world_size-1][Conf::world_size-2]) ++n;
        return n;
    }
    if (x == Conf::world_size-1 and y == Conf::world_size-1) {
        if (grid[0][Conf::world_size-2]) ++n;
        if (grid[1][Conf::world_size-2]) ++n;
        if (grid[1][Conf::world_size-1]) ++n;
        return n;
    }
    if (x == 0 and y == Conf::world_size-1) {
        if (grid[1][0]) ++n;
        if (grid[1][1]) ++n;
        if (grid[0][1]) ++n;
        return n;
    }
    if (x == 0 and y != 0 and y != Conf::world_size-1) {
        if (grid[Conf::world_size-y][x]) ++n;
        if (grid[Conf::world_size-y-2][x]) ++n;
        if (grid[Conf::world_size-y][x+1]) ++n;
        if (grid[Conf::world_size-y-1][x+1]) ++n;
        if (grid[Conf::world_size-y-2][x+1]) ++n;
        return n;
    }
    if (y == 0 and x != 0 and x != Conf::world_size-1) {
        if (grid[Conf::world_size-1][x-1]) ++n;
        if (grid[Conf::world_size-1][x+1]) ++n;
        if (grid[Conf::world_size-2][x-1]) ++n;
        if (grid[Conf::world_size-2][x]) ++n;
        if (grid[Conf::world_size-2][x+1]) ++n;
        return n;
    }
    if (x == Conf::world_size-1 and y != 0 and y != Conf::world_size-1) {
        if (grid[Conf::world_size-y][x]) ++n;
        if (grid[Conf::world_size-y-2][x]) ++n;
        if (grid[Conf::world_size-y][x-1]) ++n;
        if (grid[Conf::world_size-y-1][x-1]) ++n;
        if (grid[Conf::world_size-y-2][x-1]) ++n;
        return n;
    }
    if (y == Conf::world_size-1 and x != 0 and x != Conf::world_size-1) {
        if (grid[0][x-1]) ++n;
        if (grid[0][x+1]) ++n;
        if (grid[1][x-1]) ++n;
        if (grid[1][x]) ++n;
        if (grid[1][x+1]) ++n;
        return n;
    }
    if (grid[Conf::world_size - 1 - y - 1][x - 1]) ++n;
    if (grid[Conf::world_size - 1 - y - 1][x]) ++n;
    if (grid[Conf::world_size - 1 - y - 1][x + 1]) ++n;
    if (grid[Conf::world_size - 1 - y][x - 1]) ++n;
    if (grid[Conf::world_size - 1 - y][x + 1]) ++n;
    if (grid[Conf::world_size - 1 - y + 1][x - 1]) ++n;
    if (grid[Conf::world_size - 1 - y + 1][x]) ++n;
    if (grid[Conf::world_size - 1 - y + 1][x + 1]) ++n;
    return n;
}

bool update(Mode mode) {
    past_5_alive = past_4_alive;
    past_4_alive = past_3_alive;
    past_3_alive = past_2_alive;
    past_2_alive = past_alive;
    past_alive = nb_alive;
    nb_alive = 0;
    nb_dead = 0;
    past_stable = stable;
    stable = false;
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            grid[i][j] = updated_grid[i][j];
        }
    }
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            updated_grid[i][j] = false;
        }
    }
    for (unsigned i(0); i < grid.size(); ++i) {
        for (unsigned j(0); j < grid[i].size(); ++j) {
            birth_test(j, Conf::world_size - 1 - i);
        }
    }
    if (mode == EXPERIMENTAL) {
        //std::cout << nb_alive << " " << past_alive << " " << past_2_alive << " " << past_3_alive << "\n";
        if (nb_alive == past_alive && past_alive == past_2_alive && past_2_alive == past_3_alive) {
            stable = true;
        }
        if (nb_alive == past_2_alive && past_alive == past_3_alive && past_2_alive == past_4_alive) {
            stable = true;
        }
        if (nb_alive == past_3_alive && past_alive == past_4_alive && past_2_alive == past_5_alive) {
            stable = true;
        }
        if (stable && past_stable) {
            return true;
        }
    }
    return false;
}

void draw_world(unsigned color_theme) {
    for (unsigned i(0); i < updated_grid.size(); ++i) {
        for (unsigned j(0); j < updated_grid[i].size(); ++j) {
            if (updated_grid[i][j]) {
                graphic_draw_cell(j, Conf::world_size - i -1, color_theme);
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
    for (unsigned i(0); i < Conf::world_size; ++i) {
        for (unsigned j(0); j < Conf::world_size; ++j) {
            grid[i][j] = false;
            updated_grid[i][j] = false;
        }
    }
}

void draw_canon_planeur(unsigned x, unsigned y) {
   new_birth(x, y + 4);
   new_birth(x, y + 5);
   new_birth(x + 1, y + 4);
   new_birth(x + 1, y + 5);
   new_birth(x + 10, y + 4);
   new_birth(x + 10, y + 5);
   new_birth(x + 10, y + 6);
   new_birth(x + 11, y + 3);
   new_birth(x + 11, y + 7);
   new_birth(x + 12, y + 2);
   new_birth(x + 12, y + 8);
   new_birth(x + 13, y + 2);
   new_birth(x + 13, y + 8);
   new_birth(x + 14, y + 5);
   new_birth(x + 15, y + 3);
   new_birth(x + 15, y + 7);
   new_birth(x + 16, y + 4);
   new_birth(x + 16, y + 5);
   new_birth(x + 16, y + 6);
   new_birth(x + 17, y + 5);
   new_birth(x + 20, y + 2);
   new_birth(x + 20, y + 3);
   new_birth(x + 20, y + 4);
   new_birth(x + 21, y + 2);
   new_birth(x + 21, y + 3);
   new_birth(x + 21, y + 4);
   new_birth(x + 22, y + 1);
   new_birth(x + 22, y + 5);
   new_birth(x + 24, y);
   new_birth(x + 24, y + 1);
   new_birth(x + 24, y + 5);
   new_birth(x + 24, y + 6);
   new_birth(x + 34, y + 2);
   new_birth(x + 34, y + 3);
   new_birth(x + 35, y + 2);
   new_birth(x + 35, y + 3);
}

void erase_canon_planeur(unsigned x, unsigned y) {
    new_death(x, y + 4);
    new_death(x, y + 5);
    new_death(x + 1, y + 4);
    new_death(x + 1, y + 5);
    new_death(x + 10, y + 4);
    new_death(x + 10, y + 5);
    new_death(x + 10, y + 6);
    new_death(x + 11, y + 3);
    new_death(x + 11, y + 7);
    new_death(x + 12, y + 2);
    new_death(x + 12, y + 8);
    new_death(x + 13, y + 2);
    new_death(x + 13, y + 8);
    new_death(x + 14, y + 5);
    new_death(x + 15, y + 3);
    new_death(x + 15, y + 7);
    new_death(x + 16, y + 4);
    new_death(x + 16, y + 5);
    new_death(x + 16, y + 6);
    new_death(x + 17, y + 5);
    new_death(x + 20, y + 2);
    new_death(x + 20, y + 3);
    new_death(x + 20, y + 4);
    new_death(x + 21, y + 2);
    new_death(x + 21, y + 3);
    new_death(x + 21, y + 4);
    new_death(x + 22, y + 1);
    new_death(x + 22, y + 5);
    new_death(x + 24, y);
    new_death(x + 24, y + 1);
    new_death(x + 24, y + 5);
    new_death(x + 24, y + 6);
    new_death(x + 34, y + 2);
    new_death(x + 34, y + 3);
    new_death(x + 35, y + 2);
    new_death(x + 35, y + 3);
}

void draw_spacefiller(unsigned x, unsigned y) {
    new_birth(x, y + 11);
    new_birth(x, y + 12);
    new_birth(x, y + 13);
    new_birth(x, y + 17);
    new_birth(x, y + 18);
    new_birth(x, y + 19);
    new_birth(x + 1, y + 8);
    new_birth(x + 1, y + 11);
    new_birth(x + 1, y + 14);
    new_birth(x + 1, y + 16);
    new_birth(x + 1, y + 19);
    new_birth(x + 2, y + 5);
    new_birth(x + 2, y + 6);
    new_birth(x + 2, y + 7);
    new_birth(x + 2, y + 8);
    new_birth(x + 2, y + 11);
    new_birth(x + 2, y + 19);
    new_birth(x + 3, y + 5);
    new_birth(x + 3, y + 6);
    new_birth(x + 3, y + 7);
    new_birth(x + 3, y + 11);
    new_birth(x + 3, y + 19);
    new_birth(x + 4, y + 3);
    new_birth(x + 4, y + 7);
    new_birth(x + 4, y + 12);
    new_birth(x + 4, y + 14);
    new_birth(x + 4, y + 16);
    new_birth(x + 4, y + 18);
    new_birth(x + 5, y + 3);
    new_birth(x + 5, y + 4);
    new_birth(x + 5, y + 5);
    new_birth(x + 5, y + 12);
    new_birth(x + 5, y + 14);
    new_birth(x + 5, y + 16);
    new_birth(x + 5, y + 18);
    new_birth(x + 6, y + 2);
    new_birth(x + 6, y + 7);
    new_birth(x + 6, y + 13);
    new_birth(x + 6, y + 17);
    new_birth(x + 7, y + 1);
    new_birth(x + 7, y + 2);
    new_birth(x + 7, y + 3);
    new_birth(x + 7, y + 4);
    new_birth(x + 7, y + 5);
    new_birth(x + 7, y + 6);
    new_birth(x + 7, y + 12);
    new_birth(x + 7, y + 13);
    new_birth(x + 7, y + 14);
    new_birth(x + 7, y + 15);
    new_birth(x + 7, y + 16);
    new_birth(x + 7, y + 17);
    new_birth(x + 7, y + 18);
    new_birth(x + 8, y);
    new_birth(x + 8, y + 1);
    new_birth(x + 9, y + 1);
    new_birth(x + 9, y + 5);
    new_birth(x + 9, y + 6);
    new_birth(x + 9, y + 11);
    new_birth(x + 9, y + 12);
    new_birth(x + 9, y + 15);
    new_birth(x + 9, y + 18);
    new_birth(x + 9, y + 19);
    new_birth(x + 10, y + 3);
    new_birth(x + 10, y + 4);
    new_birth(x + 10, y + 7);
    new_birth(x + 10, y + 10);
    new_birth(x + 10, y + 12);
    new_birth(x + 10, y + 14);
    new_birth(x + 10, y + 16);
    new_birth(x + 10, y + 18);
    new_birth(x + 10, y + 21);
    new_birth(x + 10, y + 22);
    new_birth(x + 11, y + 3);
    new_birth(x + 11, y + 5);
    new_birth(x + 11, y + 10);
    new_birth(x + 11, y + 11);
    new_birth(x + 11, y + 13);
    new_birth(x + 11, y + 15);
    new_birth(x + 11, y + 18);
    new_birth(x + 11, y + 19);
    new_birth(x + 11, y + 23);
    new_birth(x + 12, y + 2);
    new_birth(x + 12, y + 3);
    new_birth(x + 12, y + 4);
    new_birth(x + 12, y + 8);
    new_birth(x + 12, y + 20);
    new_birth(x + 12, y + 24);
    new_birth(x + 13, y + 2);
    new_birth(x + 13, y + 8);
    new_birth(x + 13, y + 9);
    new_birth(x + 13, y + 10);
    new_birth(x + 13, y + 11);
    new_birth(x + 13, y + 12);
    new_birth(x + 13, y + 13);
    new_birth(x + 13, y + 14);
    new_birth(x + 13, y + 15);
    new_birth(x + 13, y + 16);
    new_birth(x + 13, y + 17);
    new_birth(x + 13, y + 18);
    new_birth(x + 13, y + 24);
    new_birth(x + 14, y + 2);
    new_birth(x + 14, y + 6);
    new_birth(x + 14, y + 18);
    new_birth(x + 14, y + 22);
    new_birth(x + 14, y + 23);
    new_birth(x + 14, y + 24);
    new_birth(x + 15, y + 3);
    new_birth(x + 15, y + 7);
    new_birth(x + 15, y + 8);
    new_birth(x + 15, y + 11);
    new_birth(x + 15, y + 13);
    new_birth(x + 15, y + 15);
    new_birth(x + 15, y + 16);
    new_birth(x + 15, y + 21);
    new_birth(x + 15, y + 23);
    new_birth(x + 16, y + 4);
    new_birth(x + 16, y + 5);
    new_birth(x + 16, y + 8);
    new_birth(x + 16, y + 10);
    new_birth(x + 16, y + 12);
    new_birth(x + 16, y + 14);
    new_birth(x + 16, y + 16);
    new_birth(x + 16, y + 19);
    new_birth(x + 16, y + 22);
    new_birth(x + 16, y + 23);
    new_birth(x + 17, y + 7);
    new_birth(x + 17, y + 8);
    new_birth(x + 17, y + 11);
    new_birth(x + 17, y + 14);
    new_birth(x + 17, y + 15);
    new_birth(x + 17, y + 20);
    new_birth(x + 17, y + 21);
    new_birth(x + 17, y + 25);
    new_birth(x + 18, y + 25);
    new_birth(x + 18, y + 26);
    new_birth(x + 19, y + 8);
    new_birth(x + 19, y + 9);
    new_birth(x + 19, y + 10);
    new_birth(x + 19, y + 11);
    new_birth(x + 19, y + 12);
    new_birth(x + 19, y + 13);
    new_birth(x + 19, y + 14);
    new_birth(x + 19, y + 20);
    new_birth(x + 19, y + 21);
    new_birth(x + 19, y + 22);
    new_birth(x + 19, y + 23);
    new_birth(x + 19, y + 24);
    new_birth(x + 19, y + 25);
    new_birth(x + 20, y + 9);
    new_birth(x + 20, y + 13);
    new_birth(x + 20, y + 19);
    new_birth(x + 20, y + 24);
    new_birth(x + 21, y + 8);
    new_birth(x + 21, y + 10);
    new_birth(x + 21, y + 12);
    new_birth(x + 21, y + 14);
    new_birth(x + 21, y + 21);
    new_birth(x + 21, y + 22);
    new_birth(x + 21, y + 23);
    new_birth(x + 22, y + 8);
    new_birth(x + 22, y + 10);
    new_birth(x + 22, y + 12);
    new_birth(x + 22, y + 14);
    new_birth(x + 22, y + 19);
    new_birth(x + 22, y + 23);
    new_birth(x + 23, y + 7);
    new_birth(x + 23, y + 15);
    new_birth(x + 23, y + 19);
    new_birth(x + 23, y + 20);
    new_birth(x + 23, y + 21);
    new_birth(x + 24, y + 7);
    new_birth(x + 24, y + 15);
    new_birth(x + 24, y + 18);
    new_birth(x + 24, y + 19);
    new_birth(x + 24, y + 20);
    new_birth(x + 24, y + 21);
    new_birth(x + 25, y + 7);
    new_birth(x + 25, y + 10);
    new_birth(x + 25, y + 12);
    new_birth(x + 25, y + 15);
    new_birth(x + 25, y + 18);
    new_birth(x + 26, y + 7);
    new_birth(x + 26, y + 8);
    new_birth(x + 26, y + 9);
    new_birth(x + 26, y + 13);
    new_birth(x + 26, y + 14);
    new_birth(x + 26, y + 15);
}

void erase_spacefiller(unsigned x, unsigned y) {
    new_death(x, y + 11);
    new_death(x, y + 12);
    new_death(x, y + 13);
    new_death(x, y + 17);
    new_death(x, y + 18);
    new_death(x, y + 19);
    new_death(x + 1, y + 8);
    new_death(x + 1, y + 11);
    new_death(x + 1, y + 14);
    new_death(x + 1, y + 16);
    new_death(x + 1, y + 19);
    new_death(x + 2, y + 5);
    new_death(x + 2, y + 6);
    new_death(x + 2, y + 7);
    new_death(x + 2, y + 8);
    new_death(x + 2, y + 11);
    new_death(x + 2, y + 19);
    new_death(x + 3, y + 5);
    new_death(x + 3, y + 6);
    new_death(x + 3, y + 7);
    new_death(x + 3, y + 11);
    new_death(x + 3, y + 19);
    new_death(x + 4, y + 3);
    new_death(x + 4, y + 7);
    new_death(x + 4, y + 12);
    new_death(x + 4, y + 14);
    new_death(x + 4, y + 16);
    new_death(x + 4, y + 18);
    new_death(x + 5, y + 3);
    new_death(x + 5, y + 4);
    new_death(x + 5, y + 5);
    new_death(x + 5, y + 12);
    new_death(x + 5, y + 14);
    new_death(x + 5, y + 16);
    new_death(x + 5, y + 18);
    new_death(x + 6, y + 2);
    new_death(x + 6, y + 7);
    new_death(x + 6, y + 13);
    new_death(x + 6, y + 17);
    new_death(x + 7, y + 1);
    new_death(x + 7, y + 2);
    new_death(x + 7, y + 3);
    new_death(x + 7, y + 4);
    new_death(x + 7, y + 5);
    new_death(x + 7, y + 6);
    new_death(x + 7, y + 12);
    new_death(x + 7, y + 13);
    new_death(x + 7, y + 14);
    new_death(x + 7, y + 15);
    new_death(x + 7, y + 16);
    new_death(x + 7, y + 17);
    new_death(x + 7, y + 18);
    new_death(x + 8, y);
    new_death(x + 8, y + 1);
    new_death(x + 9, y + 1);
    new_death(x + 9, y + 5);
    new_death(x + 9, y + 6);
    new_death(x + 9, y + 11);
    new_death(x + 9, y + 12);
    new_death(x + 9, y + 15);
    new_death(x + 9, y + 18);
    new_death(x + 9, y + 19);
    new_death(x + 10, y + 3);
    new_death(x + 10, y + 4);
    new_death(x + 10, y + 7);
    new_death(x + 10, y + 10);
    new_death(x + 10, y + 12);
    new_death(x + 10, y + 14);
    new_death(x + 10, y + 16);
    new_death(x + 10, y + 18);
    new_death(x + 10, y + 21);
    new_death(x + 10, y + 22);
    new_death(x + 11, y + 3);
    new_death(x + 11, y + 5);
    new_death(x + 11, y + 10);
    new_death(x + 11, y + 11);
    new_death(x + 11, y + 13);
    new_death(x + 11, y + 15);
    new_death(x + 11, y + 18);
    new_death(x + 11, y + 19);
    new_death(x + 11, y + 23);
    new_death(x + 12, y + 2);
    new_death(x + 12, y + 3);
    new_death(x + 12, y + 4);
    new_death(x + 12, y + 8);
    new_death(x + 12, y + 20);
    new_death(x + 12, y + 24);
    new_death(x + 13, y + 2);
    new_death(x + 13, y + 8);
    new_death(x + 13, y + 9);
    new_death(x + 13, y + 10);
    new_death(x + 13, y + 11);
    new_death(x + 13, y + 12);
    new_death(x + 13, y + 13);
    new_death(x + 13, y + 14);
    new_death(x + 13, y + 15);
    new_death(x + 13, y + 16);
    new_death(x + 13, y + 17);
    new_death(x + 13, y + 18);
    new_death(x + 13, y + 24);
    new_death(x + 14, y + 2);
    new_death(x + 14, y + 6);
    new_death(x + 14, y + 18);
    new_death(x + 14, y + 22);
    new_death(x + 14, y + 23);
    new_death(x + 14, y + 24);
    new_death(x + 15, y + 3);
    new_death(x + 15, y + 7);
    new_death(x + 15, y + 8);
    new_death(x + 15, y + 11);
    new_death(x + 15, y + 13);
    new_death(x + 15, y + 15);
    new_death(x + 15, y + 16);
    new_death(x + 15, y + 21);
    new_death(x + 15, y + 23);
    new_death(x + 16, y + 4);
    new_death(x + 16, y + 5);
    new_death(x + 16, y + 8);
    new_death(x + 16, y + 10);
    new_death(x + 16, y + 12);
    new_death(x + 16, y + 14);
    new_death(x + 16, y + 16);
    new_death(x + 16, y + 19);
    new_death(x + 16, y + 22);
    new_death(x + 16, y + 23);
    new_death(x + 17, y + 7);
    new_death(x + 17, y + 8);
    new_death(x + 17, y + 11);
    new_death(x + 17, y + 14);
    new_death(x + 17, y + 15);
    new_death(x + 17, y + 20);
    new_death(x + 17, y + 21);
    new_death(x + 17, y + 25);
    new_death(x + 18, y + 25);
    new_death(x + 18, y + 26);
    new_death(x + 19, y + 8);
    new_death(x + 19, y + 9);
    new_death(x + 19, y + 10);
    new_death(x + 19, y + 11);
    new_death(x + 19, y + 12);
    new_death(x + 19, y + 13);
    new_death(x + 19, y + 14);
    new_death(x + 19, y + 20);
    new_death(x + 19, y + 21);
    new_death(x + 19, y + 22);
    new_death(x + 19, y + 23);
    new_death(x + 19, y + 24);
    new_death(x + 19, y + 25);
    new_death(x + 20, y + 9);
    new_death(x + 20, y + 13);
    new_death(x + 20, y + 19);
    new_death(x + 20, y + 24);
    new_death(x + 21, y + 8);
    new_death(x + 21, y + 10);
    new_death(x + 21, y + 12);
    new_death(x + 21, y + 14);
    new_death(x + 21, y + 21);
    new_death(x + 21, y + 22);
    new_death(x + 21, y + 23);
    new_death(x + 22, y + 8);
    new_death(x + 22, y + 10);
    new_death(x + 22, y + 12);
    new_death(x + 22, y + 14);
    new_death(x + 22, y + 19);
    new_death(x + 22, y + 23);
    new_death(x + 23, y + 7);
    new_death(x + 23, y + 15);
    new_death(x + 23, y + 19);
    new_death(x + 23, y + 20);
    new_death(x + 23, y + 21);
    new_death(x + 24, y + 7);
    new_death(x + 24, y + 15);
    new_death(x + 24, y + 18);
    new_death(x + 24, y + 19);
    new_death(x + 24, y + 20);
    new_death(x + 24, y + 21);
    new_death(x + 25, y + 7);
    new_death(x + 25, y + 10);
    new_death(x + 25, y + 12);
    new_death(x + 25, y + 15);
    new_death(x + 25, y + 18);
    new_death(x + 26, y + 7);
    new_death(x + 26, y + 8);
    new_death(x + 26, y + 9);
    new_death(x + 26, y + 13);
    new_death(x + 26, y + 14);
    new_death(x + 26, y + 15);
}