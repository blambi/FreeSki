#include "graphics.h"
#include <stdbool.h>

/**
 Simple checksum to verify that we have the correct version of the
 original SkiFree-binary
 */
unsigned long calculate_checksum(unsigned char *data, size_t size) {
	unsigned long checksum = 0;
	for(size_t offset = 0; offset < size; offset++)
		checksum += data[offset];
	return checksum;
}

/**
 Struct that contains the address and size of bitmaps that can be
 extracted from the original .exe-file
 */
struct graphics_info {
	size_t address;
	int width;
	int height;
	int bpp;
	bool reversed;
};

/**
 Extract single image from data blob, used to get graphics from original .exe file
 */
SDL_Surface *extract_bitmap(unsigned char *data, struct graphics_info info) {
	if (info.bpp != 4)
		return NULL; // Not supported

	// For some reason palette + pixels are offsetted by 40 bytes?
	data += info.address + 40;
	const int stride = (info.width * 4) / 8;

	SDL_Surface *surface = SDL_CreateRGBSurface(0, info.width, info.height, 32, 0xFF0000, 0xFF00, 0xFF, 0);
	SDL_LockSurface(surface);

	for (int x = 0; x < info.width; x++) {
		for (int y = 0; y < info.height; y++) {

			// Find pixel
			int index = data[64 + ((info.reversed ? info.height - (y + 1) : y)) * stride + x / 2];

			// Extract nibble
			if (x % 2 == 0)
				index = (index >> 4) & 0xF;
			else
				index &= 0xF;

			// Color lookup
			unsigned int color = ((unsigned int *)data)[index];

			// Set color in surface (format in palette matches format set in SDL_CreateRGBSurface masks)
			((unsigned int *)surface->pixels)[surface->pitch / 4 * y + x] = color;
		}
	}

	SDL_UnlockSurface(surface);
	return surface;
}

SDL_Texture *load_texture(SDL_Renderer *renderer, struct graphics_info info, unsigned char *data) {
	SDL_Surface *surface = extract_bitmap(data, info);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

struct graphics *load_original_resources(char *path, SDL_Renderer *renderer) {
	struct graphics *graphics = NULL;
	unsigned char *data = NULL;
	FILE *exe = fopen(path, "rb");
	if (!exe) {
		puts("ski32.exe not found, unable to load original resources");
		goto cleanup;
	}

	// Get file size
	fseek(exe, 0L, SEEK_END);
	size_t file_size = ftell(exe);
	rewind(exe);

	// Empty file? Bail.
	if (!file_size)
		goto cleanup;

	data = (unsigned char *)malloc(file_size);

	fread(data, 1, file_size, exe);

	if (calculate_checksum(data,file_size) != 13084844L) {
		puts("Incorrect checksum for ski32.exe. Make sure you have the original 32-bit binary created 2005.");
		goto cleanup;
	}

	graphics = malloc(sizeof(struct graphics));
	graphics->skier.down = load_texture(renderer, (struct graphics_info){0xE330, 16, 32, 4, true}, data);
	graphics->skier.slightly_left = load_texture(renderer, (struct graphics_info){0xE498, 16, 32, 4, true}, data);
	graphics->skier.left = load_texture(renderer, (struct graphics_info){0xE600, 24, 28, 4, true}, data);
	graphics->skier.left_stopped = load_texture(renderer, (struct graphics_info){0xE7B8, 24, 28, 4, true}, data);
	graphics->skier.slightly_right = load_texture(renderer, (struct graphics_info){0xE970, 16, 32, 4, true}, data);
	graphics->skier.right = load_texture(renderer, (struct graphics_info){0xEAD8, 24, 28, 4, true}, data);
	graphics->skier.right_stopped = load_texture(renderer, (struct graphics_info){0xEC90, 24, 28, 4, true}, data);
	
cleanup:
	if (exe)
		fclose(exe);
	if (data)
		free(data);
	return graphics;
}

void cleanup_graphics(struct graphics *graphics) {
	SDL_DestroyTexture(graphics->skier.down);
	SDL_DestroyTexture(graphics->skier.slightly_left);
	SDL_DestroyTexture(graphics->skier.left);
	SDL_DestroyTexture(graphics->skier.left_stopped);
	SDL_DestroyTexture(graphics->skier.slightly_right);
	SDL_DestroyTexture(graphics->skier.right);
	SDL_DestroyTexture(graphics->skier.right_stopped);
	SDL_DestroyTexture(graphics->tree);
}
