#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <chrono>
#include <iostream>

#include "Simulation.h"

#define WIDTH 1000
#define HEIGHT 1000

typedef std::chrono::high_resolution_clock Clock;

int main(int ArgCount, char **Args)
{

    uint32_t WindowFlags = SDL_WINDOW_OPENGL;
    SDL_Window *Window = SDL_CreateWindow("AntsGL", 0, 0, WIDTH, HEIGHT, WindowFlags);
    assert(Window);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GLContext Context = SDL_GL_CreateContext(Window);

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    Simulation sim = Simulation(1024, 1024);

    auto start = Clock::now();

    bool Running = true;
    bool FullScreen = false;
    while (Running)
    {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_KEYDOWN)
            {
                switch (Event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    Running = false;
                    break;
                case 'd':
                    break;
                case 'f':
                    std::cout << "Adding food pack" << std::endl;
                    sim.addFood(50, 50);
                    break;
                case 'a':
                    std::cout << "Adding anthill pack" << std::endl;
                    sim.addAnthill(50, 50);
                    break;
                default:
                    break;
                }
            }
            else if (Event.type == SDL_QUIT)
            {
                Running = false;
            }
        }

        sim.update(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start).count() * 1e-9);
        sim.render();
        start = Clock::now();
        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(Window);
    }
    SDL_GL_DeleteContext(Context);
    return 0;
}