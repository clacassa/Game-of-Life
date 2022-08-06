#include "config.h"

unsigned Conf::world_size = reserve;

void Conf::define_random_exp() {
    #define RANDOM_EXP
}

void Conf::set_world_size(unsigned wsize) {
    world_size = wsize;
}
