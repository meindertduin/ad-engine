#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "engine/window.h"
#include "gfx/render_pipeline.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

int main ( int argc, char* args[] ) {
    AdWindow window { math::Size { WIDTH, HEIGHT }, "Ad Render Demo" };
    window.initialize();

    auto renderPipeline = gfx::RenderPipeline::createInstance(WIDTH, HEIGHT);
    renderPipeline->initialize();

    // Poll for events and wait till user closes window
    bool quit = false;
    SDL_Event currentEvent;
    while(!quit) {
        while(SDL_PollEvent(&currentEvent) != 0) {
            if(currentEvent.type == SDL_QUIT) {
                quit = true;
            }

            renderPipeline->render();
        }
    }

    return 0;
}