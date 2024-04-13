#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

///@brief Create and load a texture from an image file.
///@param texture A pointer to a pointer to SDL_Texture to store the loaded texture.
///@param pRenderer The SDL_Renderer that will be used for rendering the texture.
///@param path The file path to the image that should be loaded into the texture.
void create_texture(SDL_Texture** texture, SDL_Renderer* pRenderer, const char* path);

#endif