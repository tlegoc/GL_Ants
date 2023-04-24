#pragma once

class Simulation
{

public:
    Simulation(unsigned int width, unsigned int height, unsigned int max_anthill_count = 50);
    Simulation(const Simulation &other) = delete;
    Simulation(Simulation &other) = delete;

    void update(float delta);
    void addAnthill(int x, int y);
    void addFood(int x, int y);
    void render();

    unsigned int getRenderTexture();

private:
    void writeDataToAf(unsigned int x, unsigned int y, unsigned int val);

    const unsigned int m_width;
    const unsigned int m_height;
    unsigned int m_anthill_count;
    const unsigned int m_max_anthill_count;

    // Data ids
    unsigned int m_render_tx_id;
    unsigned int m_af_tx_id;
    unsigned int m_pheromone_tx_id;
    unsigned int m_ant_ssbo_id;

    // Shader ids
    unsigned int m_ants_compute_id;
    unsigned int m_render_compute_id;
    unsigned int m_input_compute_id;
};