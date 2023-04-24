#pragma once

#define ANT_SHADER_PATH "ants.comp"
#define ANTHILL_FOOD_RENDER_SHADER_PATH "anthill_food_render.comp"
#define INPUT_SHADER_PATH "input.comp"

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