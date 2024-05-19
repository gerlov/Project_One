#include "escape_portal.h"

#define PORTAL_WIDTH 40
#define PORTAL_HEIGHT 95

Portal portal;
SDL_Texture* portalTexture;

void initPortal(SDL_Renderer* renderer, TileMap* tilemap) {
    if (!portalTexture) {
        create_texture(&portalTexture, renderer, "../lib/assets/exit/portal.png");
        if (!portalTexture) {
            printf("Failed to load portal texture: %s\n", SDL_GetError());
            return;
        }
    }

    if (tilemap->portal_spawn.num_points == 0) {
        printf("Error: No hunter spawn points available for placing the portal.\n");
        return;
    }

    int index = our_rand() % tilemap->portal_spawn.num_points;
    SDL_Point portalPosition = tilemap->portal_spawn.points[index];

    // Used for teleport
    portal.position.x = (float)portalPosition.x;
    portal.position.y = (float)portalPosition.y;

    // Used for SDL_HasIntersection
    portal.rect.x = portalPosition.x;
    portal.rect.y = portalPosition.y;
    portal.rect.w = PORTAL_WIDTH;
    portal.rect.h = PORTAL_HEIGHT;
}

void drawPortal(SDL_Renderer* renderer, TileMap* tilemap) {
    if (!renderer || !portalTexture) {
        return;
    }
    SDL_Rect srcRect;
    SDL_Rect dstRect;

    srcRect.w = PORTAL_WIDTH;
    srcRect.h = PORTAL_HEIGHT;
    srcRect.x = (SDL_GetTicks() / 100 % 4) * srcRect.w;
    srcRect.y = 0;

    dstRect.w = 50;
    dstRect.h = 48;

    dstRect.x = (int)(portal.position.x - tilemap->camera.x);
    dstRect.y = (int)(portal.position.y - tilemap->camera.y);

    SDL_RenderCopy(renderer, portalTexture, &srcRect, &dstRect);
}

void cleanUpPortalResources() {
    SDL_DestroyTexture(portalTexture);
}