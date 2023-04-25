#pragma once

struct Ant {
    float position_x;
    float position_y;

    float direction_x;
    float direction_y;

    unsigned int has_food;
    unsigned int is_alive;

    unsigned int anthill_position_x;
    unsigned int anthill_position_y;
};