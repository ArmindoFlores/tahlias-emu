#ifndef GUI_H
#define GUI_H

#include <SDL2/SDL.h>
#include <string>
#include <functional>

struct Color {
    int r, g, b;
};

class GUI {
public:
    GUI(int width, int height, const std::string &title);
    ~GUI();

    void drawPoint(int x, int y, const Color &color);
    void fill(const Color& color);
    SDL_Event getEvent();
    void update();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif