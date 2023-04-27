#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <chrono>
#include <iostream>

#include "Simulation.h"

#define WIDTH 1024
#define HEIGHT 700

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
    SDL_GL_SetSwapInterval(0);

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    Simulation sim = Simulation(WIDTH, HEIGHT, 150);

    GLuint fboId = 0;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, sim.getRenderTexture(), 0);

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
                int x, y;
                switch (Event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    Running = false;
                    break;
                case 'd':
                    break;
                case 'f':
                    std::cout << "Adding food pack" << std::endl;
                    sim.getRandomPosition(&x, &y);
                    sim.addFood(x, y);
                    break;
                case 'a':
                    std::cout << "Adding anthill pack" << std::endl;
                    sim.getRandomPosition(&x, &y);
                    sim.addAnthill(x, y);
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

        float delta_time = std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start).count() * 1e-9 * 50.0;
        start = Clock::now();
        sim.update(delta_time);
        sim.render(delta_time);
        glViewport(0, 0, WIDTH, HEIGHT);
        glClearColor(1.0f, .0f, .0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        SDL_GL_SwapWindow(Window);
    }
    SDL_GL_DeleteContext(Context);
    return 0;
}