#include <SDL2/SDL.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>

unsigned long calculate_checksum(FILE *file) {
	unsigned long checksum = 0;
	while (!feof(file) && !ferror(file)) {
		checksum += fgetc(file);
	}
	return checksum;
}

void load_original_resources() {
	FILE *exe = fopen("ski32.exe", "rb");
	if (!exe) {
		puts("ski32.exe not found, unable to load original resources");
		return;
	}

	if (calculate_checksum(exe) != 13084843) {
		puts("Incorrect checksum for ski32.exe. Make sure you have the original 32-bit binary created 2005.");
	}

	// Load image files here
	
	fclose(exe);
}

typedef struct {
	float x;
	float y;
} vec2;

vec2 vec2_add(vec2 a, vec2 b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

vec2 vec2_subtract(vec2 a, vec2 b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

struct game_object {
	SDL_Surface *image;
	vec2 position;
	vec2 velocity;
	struct game_object *next;
	struct game_object *previous;
};

void add_object(struct game_object *list, struct game_object *object) {
	if (list->next == NULL) {
		list->next = object;
		return;
	}
	add_object(list->next, object);
}

void remove_object(struct game_object *list, struct game_object *object) {
	if (list->next == object) {
		list->next = object->next;
		return;
	}
	remove_object(list->next, object);
}

struct game_state {
	struct game_object *skier;
};

bool update(struct game_state *state) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			return false;
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case SDLK_RIGHT:
				state->skier->velocity.x += 2.0;
				break;
			case SDLK_LEFT:
				state->skier->velocity.x -= 2.0;
				break;
			case SDLK_UP:
				state->skier->velocity.y -= 2.0;
				break;
			case SDLK_DOWN:
				state->skier->velocity.y += 2.0;
				break;
			default:
				break;
			}
		}
	}

	state->skier->position = vec2_add(state->skier->position, state->skier->velocity);	
	return true;
}

void draw(SDL_Window *window, struct game_state *state) {
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, 0xFFFFFFFF);
 
	vec2 camera = {.x = surface->w / 2.0, .y = surface->h / 2.0};
	
	// Center camera above skier
	camera = vec2_subtract(state->skier->position, camera);
	
	for (struct game_object *object = state->skier; object != NULL; object = object->next) {
		SDL_Rect target_rect = {.x = object->position.x - camera.x, .y = object->position.y - camera.y};
		SDL_BlitSurface(object->image, NULL, SDL_GetWindowSurface(window), &target_rect);
	}

	SDL_UpdateWindowSurface(window);
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("FreeSki", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 380, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (window == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		exit (1);
	}

	SDL_Surface *window_surface = SDL_GetWindowSurface(window);

	struct game_object skier;
	skier.position.x = 0;
	skier.position.y = 0;
	skier.next = NULL;
	skier.image = SDL_CreateRGBSurfaceWithFormat(0, 16, 32, window_surface->format->BytesPerPixel, window_surface->format->format);

	// Just fill skier with red for now
	SDL_FillRect(skier.image, NULL, 0xFFFF0000);

	struct game_state state = {.skier = &skier};

	// Main game loop
	const int FPS = 60;
	const int MS_PER_FRAME = 1000 / FPS;
	
	SDL_Event event;
	while (1) {
		uint32_t frame_start = SDL_GetTicks();
		if (!update(&state)) {
			goto quit;
		}
		draw(window, &state);
		uint32_t update_time = SDL_GetTicks() - frame_start;

		// Sleep if updating/drawing is faster than needed
		if (update_time < MS_PER_FRAME) {
			SDL_Delay(MS_PER_FRAME - update_time);
		}
	}

 quit:
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
