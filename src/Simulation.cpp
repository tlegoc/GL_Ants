#include "Simulation.h"

#include "config.h"

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

std::string readFile(const char *filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open())
    {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

unsigned int LoadShader(const char *path)
{
    unsigned int shader = glCreateShader(GL_COMPUTE_SHADER);

    // Read shader
    std::string shader_str_src = readFile(path);
    const char *shader_src = shader_str_src.c_str();

    int result = GL_FALSE;
    int log_length;

    std::cout << "Compiling compute shader... " << path << std::endl;
    glShaderSource(shader, 1, &shader_src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> shader_error((log_length > 1) ? log_length : 1);
    glGetShaderInfoLog(shader, log_length, NULL, &shader_error[0]);
    std::cout << "Shader Errors?:" << std::endl
              << &shader_error[0];
    if (log_length > 0)
        std::cout << std::endl;

    std::cout << "Linking program" << std::endl;
    unsigned int program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<char> program_error((log_length > 1) ? log_length : 1);
    glGetProgramInfoLog(program, log_length, NULL, &program_error[0]);
    std::cout << "Program Errors?:" << std::endl
              << &program_error[0];
    if (log_length > 0)
        std::cout << std::endl;

    glDeleteShader(shader);

    return program;
}

float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}

void displayErrors()
{
    GLenum code;

    const char *string;
    while ((code = glGetError()) != GL_NO_ERROR)
    {
        switch (code)
        {
        // opengl 2 errors (8)
        case GL_NO_ERROR:
            string = "GL_NO_ERROR";

        case GL_INVALID_ENUM:
            string = "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            string = "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            string = "GL_INVALID_OPERATION";

        case GL_STACK_OVERFLOW:
            string = "GL_STACK_OVERFLOW";

        case GL_STACK_UNDERFLOW:
            string = "GL_STACK_UNDERFLOW";

        case GL_OUT_OF_MEMORY:
            string = "GL_OUT_OF_MEMORY";

        case GL_TABLE_TOO_LARGE:
            string = "GL_TABLE_TOO_LARGE";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            string = "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:
            break;
        }

        std::cout << "OpenGL error: " << string << std::endl;
    }
}

Simulation::Simulation(unsigned int width, unsigned int height, unsigned int max_anthill_count) :
m_width(width),
m_height(height),
m_max_anthill_count(max_anthill_count)
{
    // Texture creation
    std::cout << "Creating render texture..." << std::endl;
    m_render_tx_id = 0;
    glGenTextures(1, &m_render_tx_id);
    glBindTexture(GL_TEXTURE_2D, m_render_tx_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, RENDER_TEXTURE_FORMAT, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    displayErrors();

    std::cout << "Creating af texture..." << std::endl;
    m_af_tx_id = 0;
    glGenTextures(1, &m_af_tx_id);
    glBindTexture(GL_TEXTURE_2D, m_af_tx_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, AF_TEXTURE_FORMAT, m_width, m_height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
    displayErrors();

    std::cout << "Creating pheromone texture..." << std::endl;
    m_pheromone_tx_id = 0;
    glGenTextures(1, &m_pheromone_tx_id);
    glBindTexture(GL_TEXTURE_2D, m_pheromone_tx_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, PHEROMONES_TEXTURE_FORMAT, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    displayErrors();

    // SSBO creation
    m_ant_ssbo_id = 0;
    glGenBuffers(1, &m_ant_ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ant_ssbo_id);
    std::cout << "Allocating " << m_max_anthill_count * 50 << " blocks of " << sizeof(Ant) << " bytes, total:" << sizeof(Ant) * 50 * m_max_anthill_count << std::endl;
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Ant) * 50 * m_max_anthill_count, 0, GL_DYNAMIC_COPY);

    // Shader creation
    m_ants_compute_id = LoadShader("ants.comp");
    displayErrors();
    m_input_compute_id = LoadShader("input.comp");
    displayErrors();
    m_render_compute_id = LoadShader("render.comp");
    displayErrors();

    m_anthill_count = 0;
    displayErrors();
}

void Simulation::update(float delta)
{
    std::cout << "Updating: " << delta << std::endl;
    if (m_anthill_count <= 0)
        return;

    glUseProgram(m_ants_compute_id);

    // Bind SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ant_ssbo_id);

    // Bind textures
    glBindImageTexture(1, m_af_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, AF_TEXTURE_FORMAT);

    glBindImageTexture(2, m_pheromone_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, PHEROMONES_TEXTURE_FORMAT);

    glBindImageTexture(3, m_render_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, RENDER_TEXTURE_FORMAT);

    glDispatchCompute(m_anthill_count, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    std::cout << "Running " << m_anthill_count << " groups." << std::endl;
    displayErrors();
}

void Simulation::addAnthill(int x, int y)
{
    if (m_anthill_count >= m_max_anthill_count)
        return;
        
    m_anthill_count += 1;

    Ant ants[50];

    for (int i = 0; i < 50; i++)
    {
        ants[i].anthill_position_x = x;
        ants[i].anthill_position_y = y;
        ants[i].position_x = x;
        ants[i].position_y = y;

        float dir_x, dir_y, length;
        dir_x = randomFloat() * 2.f - 1.f;
        dir_y = randomFloat() * 2.f - 1.f;
        length = sqrt(dir_x * dir_x + dir_y * dir_y);
        dir_x /= length;
        dir_y /= length;
        ants[i].direction_x = dir_x;
        ants[i].direction_y = dir_y;

        ants[i].has_food = 0;
        ants[i].is_alive = 1;
    }
    // Anthill is stored on the 8th bit of the AF texture
    // 0x80 = 10000000
    // 0 if none, 1 if anthill
    writeDataToAf(x, y, 128);
    glNamedBufferSubData(m_ant_ssbo_id, m_anthill_count * 50 * sizeof(Ant), 50 * sizeof(Ant), ants);
    displayErrors();
}

void Simulation::addFood(int x, int y)
{
    // Food is stored on the first 7 bits of the AF texture
    // 0x7F = 01111111
    // We can have up to 127 food sources on one pixel
    writeDataToAf(x, y, 0x7F);
}

void Simulation::render()
{
    std::cout << "Rendering..." << std::endl;
    glUseProgram(m_render_compute_id);

    // Bind SSBO
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ant_ssbo_id);

    // Bind textures
    glBindImageTexture(1, m_af_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, AF_TEXTURE_FORMAT);

    glBindImageTexture(2, m_pheromone_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, PHEROMONES_TEXTURE_FORMAT);

    glBindImageTexture(3, m_render_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, RENDER_TEXTURE_FORMAT);

    glDispatchCompute(m_width, m_height, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    displayErrors();
}

unsigned int Simulation::getRenderTexture()
{
    return m_render_tx_id;
}

void Simulation::writeDataToAf(unsigned int x, unsigned int y, unsigned int val)
{
    std::cout << "Adding data to ant/food texture..." << std::endl;
    glUseProgram(m_input_compute_id);

    glBindImageTexture(0, m_af_tx_id, 0, GL_FALSE, 0, GL_READ_WRITE, AF_TEXTURE_FORMAT);

    unsigned int input_compute_write_x_un = glGetUniformLocation(m_input_compute_id, "write_x");
    unsigned int input_compute_write_y_un = glGetUniformLocation(m_input_compute_id, "write_y");
    unsigned int input_compute_write_val_un = glGetUniformLocation(m_input_compute_id, "write_val");

    // Print uniforms
    glUniform1ui(input_compute_write_x_un, x);
    glUniform1ui(input_compute_write_y_un, y);
    glUniform1ui(input_compute_write_val_un, val);

    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}