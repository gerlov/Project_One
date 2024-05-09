#include "mazeview.h"

void init_maze_view(MazeView* mazeView, SDL_Renderer* renderer, TileMap* tilemap, int window_width, int window_height) {
    // calc scale down factor based on window size 
    float scale_factor_w = (float)window_width / (tilemap->width * tilemap->tile_size);
    float scale_factor_h = (float)window_height / (tilemap->height * tilemap->tile_size);
    float scale_factor = (scale_factor_w < scale_factor_h) ? scale_factor_w : scale_factor_h;

    int scaled_tile_size = (int)(tilemap->tile_size * scale_factor);
    int new_texture_width = tilemap->width * scaled_tile_size;
    int new_texture_height = tilemap->height * scaled_tile_size;

    //  texture to render the scaled tilemap 
    SDL_Texture* scaledTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, new_texture_width, new_texture_height);
    if (!scaledTexture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return;
    }

    // set tetxure as the target for rendering
    SDL_SetRenderTarget(renderer, scaledTexture);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

   // render each tile the tilemap on the texture
    SDL_Rect position;
    for (int y = 0; y < tilemap->height; y++) {
        for (int x = 0; x < tilemap->width; x++) {
            Tile *tile = get_tile(tilemap, x, y);
            if (tile == NULL || tile->type == TILE_EMPTY)
                continue;

            position.x = x * scaled_tile_size;
            position.y = y * scaled_tile_size;
            position.w = scaled_tile_size;
            position.h = scaled_tile_size;

            SDL_Texture* src_texture; // Variable to hold the texture based on the tile type
            switch (tile->type) {
                case TILE_FLOOR:
                    src_texture = tilemap->pFloorTexture;
                    break;
                case TILE_WALL:
                    src_texture = tilemap->pWallTexture;
                    break;
                case TILE_PIT:
                    src_texture = tilemap->pPitTexture;
                    break;
                default:
                    continue; // Skip rendering if it's an unknown type
            }

            SDL_SetTextureBlendMode(src_texture, SDL_BLENDMODE_BLEND);
            SDL_RenderCopyEx(renderer, src_texture, &tile->src_rect, &position, 0, NULL, SDL_FLIP_NONE);
        }
    }

   
    SDL_SetRenderTarget(renderer, NULL);
    SDL_SetTextureBlendMode(scaledTexture, SDL_BLENDMODE_BLEND);

    // load background texture 
    mazeView->bgTexture = IMG_LoadTexture(renderer, "../lib/assets/powerup_pack/bkgr.png");
    if (!mazeView->bgTexture) {
        SDL_Log("Failed to load background texture: %s", SDL_GetError());
        SDL_DestroyTexture(scaledTexture);
        return;
    }

    mazeView->texture = scaledTexture;
    mazeView->viewRect = (SDL_Rect){0, 0, new_texture_width, new_texture_height};
    mazeView->visible = 0;
    mazeView->displayTime = 0;
}

void render_maze_view(MazeView* mazeView, SDL_Renderer* renderer) {
    if (mazeView->visible && SDL_GetTicks() < mazeView->displayTime) {
        if (mazeView->bgTexture) {
            SDL_RenderCopy(renderer, mazeView->bgTexture, NULL, NULL);
        }

        int renderer_width, renderer_height;
        SDL_GetRendererOutputSize(renderer, &renderer_width, &renderer_height);
        SDL_Rect drawRect = {
            (renderer_width - mazeView->viewRect.w) / 2,
            (renderer_height - mazeView->viewRect.h) / 2,
            mazeView->viewRect.w,
            mazeView->viewRect.h
        };

        SDL_RenderCopy(renderer, mazeView->texture, NULL, &drawRect);
    } else {
        mazeView->visible = 0;
    }
}

void free_maze_view(MazeView* mazeView) {
    if (mazeView->texture != NULL) {
        SDL_DestroyTexture(mazeView->texture);
        mazeView->texture = NULL;
    }
    if (mazeView->bgTexture != NULL) {
        SDL_DestroyTexture(mazeView->bgTexture);
        mazeView->bgTexture = NULL;
    }
}
