#include "Display.h"

#include "Config.h"

#include <SDL/SDL.h>
#include <SDL/SDL_video.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_gfxBlitFunc.h>

namespace display
{

static u8* display_buffer = nullptr;
static u8 size_x = 0, size_y = 0;

static SDL_Surface* surface = nullptr;
static SDL_Surface* font = nullptr;

constexpr unsigned FONT_X = 4;
constexpr unsigned FONT_Y = 5;

// TODO: Find out how to scale things in SDL
constexpr unsigned FONT_SCALE = 1;

void init(u8 sx, u8 sy)
{
    info("Display") << "Initializing: " << (int)sx << "x" << (int)sy;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        error("Display") << "Failed to initialize SDL";
        exit(1);
    }

    surface = SDL_SetVideoMode(sx * FONT_SCALE, sy * FONT_SCALE, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetCaption("cx16 Display", "cx16 Display");
    if(!surface)
    {
        error("Display") << "Failed to create SDL surface";
        exit(1);
    }

    // TODO: Support loading fonts with I/O
    // TODO: Real resource path
    font = IMG_Load("ascii.png");
    if(!font)
    {
        error("Display") << "Failed to load font (ascii.png)";
        exit(1);
    }

    size_x = sx;
    size_y = sy;
    display_buffer = new u8[size_x * size_y];

    for(u8 x = 0; x < sx; x++)
    {
        for(u8 y = 0; y < sy; y++)
            set_pixel(x, y, 0);
    }

    const char str[] = "cx16 Emulator v1.0";
    size_t s = 0;
    for(auto ch: str)
    {
        set_pixel(s, 0, ch);
        s++;
    }
}

void main_loop()
{
    SDL_Event event;
    while(SDL_WaitEvent(&event))
    {
        // TODO: Handle events
        if(event.type == SDL_QUIT)
        {
            break;
        }
    }
    SDL_FreeSurface(font);
    SDL_FreeSurface(surface);
    SDL_Quit();
}

void set_pixel(u8 x, u8 y, u8 color)
{
    if(!display_buffer)
    {
        error("Display") << "set_pixel: Display not initialized";
        return;
    }

    //trace("Display") << "set_pixel " << (int)x << "," << (int)y << "=" << (int)color;

    // Display
    unsigned font_x = color & 0xF;
    unsigned font_y = (color & 0xF0) >> 4;
    SDL_Rect srcRect { .x = static_cast<Sint16>(font_x * FONT_X), .y = static_cast<Sint16>(font_y * FONT_Y), .w = FONT_X, .h = FONT_Y };
    SDL_Rect dstRect { .x = static_cast<Sint16>(x * (FONT_X + 1) * FONT_SCALE), .y = static_cast<Sint16>(y * (FONT_Y + 1) * FONT_SCALE), .w = FONT_X * FONT_SCALE, .h = FONT_Y * FONT_SCALE };
    SDL_BlitSurface(font, &srcRect, surface, &dstRect);

    // Set internal buffer
    display_buffer[x*size_y+y] = color;
}

u8 pixel(u8 x, u8 y)
{
    if(!display_buffer)
    {
        error("Display") << "pixel: Display not initialized";
        return 0;
    }
    return display_buffer[x*size_y+y];
}

}
