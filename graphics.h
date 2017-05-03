#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

struct graphics {
	SDL_Texture *skier_front;
};

/**
 Loads image resources from the original ski32.exe-file.
 Returns NULL if file cannot be found or is not the correct one.
 */
struct graphics *load_original_resources(char *path, SDL_Renderer *renderer);
void cleanup_graphics(struct graphics *graphics);

#endif
