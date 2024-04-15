#include "texture.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


void create_texture(SDL_Texture** texture, SDL_Renderer* pRenderer, const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if(!surface)
    {
        printf("Error: %s\n", SDL_GetError());
        return;
    }
    *texture = SDL_CreateTextureFromSurface(pRenderer, surface);
    SDL_FreeSurface(surface);
    if(!*texture)
    {
        printf("Error: %s\n", SDL_GetError());
        return;
    }
}