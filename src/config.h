#pragma once

#define AF_TEXTURE_FORMAT 0x8232 // GL_R8UI
#define PHEROMONES_TEXTURE_FORMAT 0x822D // GL_R16F
#define RENDER_TEXTURE_FORMAT 0x8814 // GL_RGBA32F

#define ANT_SHADER_PATH "ants.comp"
#define RENDER_SHADER_PATH "render.comp"
#define INPUT_SHADER_PATH "input.comp"

#define ANTHILL_MASK 0x80
#define FOOD_MASK 0x7F

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