#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include "graphics.h"
#include "area.h"

struct game_state {
	struct game_object *skier;
};

struct graphics *textures = NULL;

static inline SDL_Texture *skier_texture_for_velocity(vec2 velocity) {
	if (velocity.x < -3.0) {
		return textures->skier.left;
	} else if (velocity.x < 0.0) {
		return textures->skier.slightly_left;
	} else if (velocity.x > 3.0) {
		return textures->skier.right;
	} else if (velocity.x > 0.0) {
		return textures->skier.slightly_right;
	} else {
		return textures->skier.down;
	}
}

static inline SDL_Texture *texture_for_object_type(enum object_type type) {
	switch (type) {
	case TREE:
		return textures->skier.right_stopped;
	case ROCK:
		return textures->rock;
	case STUMP:
		return textures->stump;
	case SMALL_HUMP:
		return textures->small_hump;
	case LARGE_HUMP:
		return textures->large_hump;
	default:
		return NULL;
	}
}

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

void draw(SDL_Renderer *renderer, struct game_state *state) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	int width, height;
	SDL_RenderGetLogicalSize(renderer, &width, &height);

	vec2 camera = {.x = (float)width / 2.0, .y = (float)height / 2.0};
	
	// Put center of camera above skier
	camera = vec2_subtract(state->skier->position, camera);

	// Render skier
	SDL_Texture *texture = skier_texture_for_velocity(state->skier->velocity);
	int tw, th; SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
	SDL_Rect target_rect = {.x = state->skier->position.x - camera.x, .y = state->skier->position.y - camera.y, .w = tw, .h = th};
	SDL_RenderCopy(renderer, texture, NULL, &target_rect);

	// Render visible areas
	struct list_node *areas = get_visible_areas(camera.x, camera.y, width, height);
	for (struct list_node *node = areas; node != NULL; node = node->next) {
		struct game_area *area = node->object;
		for (struct game_object *object = area->objects; object != NULL; object = object->next) {
			SDL_Texture *texture = texture_for_object_type(object->type);
			int tw, th; SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
			SDL_Rect target_rect = {.x = object->position.x - camera.x, .y = object->position.y - camera.y, .w = tw, .h = th};
			SDL_RenderCopy(renderer, texture, NULL, &target_rect);
		}
	}
	free_list(areas, false);

	SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	const int width = 640;
	const int height = 380;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("FreeSki", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	if (window == NULL) {
		printf("Could not create window: %s\n", SDL_GetError());
		exit (1);
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		printf("Could not create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_RenderSetLogicalSize(renderer, width, height);

	textures = load_original_resources("ski32.exe", renderer);

	struct game_object skier = {0};
	struct game_state state = {.skier = &skier};

	// Main game loop
	const int FPS = 60;
	const int MS_PER_FRAME = 1000 / FPS;

	while (1) {
		uint32_t frame_start = SDL_GetTicks();
		if (!update(&state)) {
			goto quit;
		}
		draw(renderer, &state);
		uint32_t update_time = SDL_GetTicks() - frame_start;

		// Sleep if updating/drawing is faster than needed
		if (update_time < MS_PER_FRAME) {
			SDL_Delay(MS_PER_FRAME - update_time);
		}
	}

quit:
	cleanup_graphics(textures);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
