#include "escape_portal.h"

SDL_Texture* portalTexture;
SDL_Point portal_position = {0, 0};


void initPortal(SDL_Renderer* renderer, TileMap* tilemap) {
    if (!portalTexture) {
        create_texture(&portalTexture, renderer, "../lib/assets/exit/portal.png");
        if (!portalTexture) {
            printf("Failed to load portal texture: %s\n", SDL_GetError());
            return;
        }
    }

    if (tilemap->hunter_spawn.num_points == 0) {
        printf("Error: No hunter spawn points available for placing the portal.\n");
        return;
    }

    int index = our_rand() % tilemap->hunter_spawn.num_points;
    portal_position = tilemap->hunter_spawn.points[index];
}
void drawPortal(SDL_Renderer* renderer, TileMap* tilemap) {
    if (!renderer || !portalTexture) {
        return;
    }
    SDL_Rect srcRect;
    srcRect.w = 32;  
    srcRect.h = 64;  
    srcRect.x = (SDL_GetTicks() / 100 % 5) * srcRect.w;
    srcRect.y = 0;

    SDL_Rect dstRect;
    dstRect.w = 68;
    dstRect.h = 76;

    dstRect.x = portal_position.x - tilemap->camera.x; 
    dstRect.y = portal_position.y - tilemap->camera.y; 
    SDL_RenderCopy(renderer, portalTexture, &srcRect, &dstRect);
}

void cleanUpPortalResources() {
    SDL_DestroyTexture(portalTexture);
}