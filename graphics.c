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
 Extract single image from data blob, used to get graphics from original .exe file
 */
SDL_Surface *extract_bitmap(unsigned char *data, size_t address, int width, int height, int bbp, bool reversed) {
	if (bbp != 4)
		return NULL; // Not supported

	// For some reason palette + pixels are offsetted by 40 bytes?
	data += address + 40;
	const int stride = (width * 4) / 8;

	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0xFF0000, 0xFF00, 0xFF, 0);
	SDL_LockSurface(surface);

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {

			// Find pixel
			int index = data[64 + ((reversed ? height - (y + 1) : y)) * stride + x / 2];

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

	SDL_Surface *skier_front = extract_bitmap(data, 0xE330, 16, 32, 4, true);
	graphics->skier_front = SDL_CreateTextureFromSurface(renderer, skier_front);
	SDL_FreeSurface(skier_front);
	
cleanup:
	if (exe)
		fclose(exe);
	if (data)
		free(data);
	return graphics;
}

void cleanup_graphics(struct graphics *graphics) {
	SDL_DestroyTexture(graphics->skier_front);
}
