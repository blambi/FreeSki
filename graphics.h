#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct graphics {
	struct {
		SDL_Texture *down;
		SDL_Texture *slightly_left;
		SDL_Texture *left;
		SDL_Texture *left_stopped;
		SDL_Texture *slightly_right;
		SDL_Texture *right;
		SDL_Texture *right_stopped;
	} skier;
	SDL_Texture *tree;
	SDL_Texture *rock;
	SDL_Texture *stump;
	SDL_Texture *small_hump;
	SDL_Texture *large_hump;
};

/**
 Loads image resources from the original ski32.exe-file.
 Returns NULL if file cannot be found or is not the correct one.
 */
struct graphics *load_original_resources(char *path, SDL_Renderer *renderer);
/**
 Loads image resources from an folder containing alternative graphics.
 Returns NULL if not all files could be found.
 */
struct graphics *load_alternative_resources(char *path, SDL_Renderer *renderer);
void cleanup_graphics(struct graphics *graphics);

#endif
