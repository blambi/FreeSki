#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "graphics.h"

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
	SDL_Texture *texture;
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

struct graphics *textures = NULL;

SDL_Texture *skier_texture_for_velocity(vec2 velocity) {
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
			state->skier->texture = skier_texture_for_velocity(state->skier->velocity);
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
	
	for (struct game_object *object = state->skier; object != NULL; object = object->next) {
		int tw, th; SDL_QueryTexture(object->texture, NULL, NULL, &tw, &th);
		SDL_Rect target_rect = {.x = object->position.x - camera.x, .y = object->position.y - camera.y, .w = tw, .h = th};
		SDL_RenderCopy(renderer, object->texture, NULL, &target_rect);
	}

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
	skier.texture = textures->skier.down;

	struct game_object tree = {0};
	tree.position = (vec2){100.0, 100.0};
	tree.texture = textures->skier.slightly_left;
	skier.next = &tree;

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
