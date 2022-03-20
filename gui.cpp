#include "gui.h"
#include <iostream>

GUI::GUI(int width, int height, const std::string &title)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow(title.c_str(), 
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED,
                              width, height, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    fill((Color){255, 255, 255});
    update();
}

void GUI::update()
{
    SDL_RenderPresent(renderer);  
}

void GUI::fill(const Color &c)
{
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    SDL_RenderClear(renderer);
}

void GUI::drawPoint(int x, int y, const Color &c)
{
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    SDL_Rect r;
    r.x = x*8;
    r.y = y*8;
    r.w = 8;
    r.h = 8;
    SDL_RenderFillRect(renderer, &r);  
}

SDL_Event GUI::getEvent()
{
    SDL_Event e;
    SDL_PollEvent(&e);
    return e;
}

GUI::~GUI()
{
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}