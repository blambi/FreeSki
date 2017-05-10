#include "area.h"
#include <tgmath.h>

// Areas are rectangular. This is the length of a side of the rectangle
const float AREA_SIZE = 600.0;
const int AREA_CACHE_COUNT = 10;

struct game_area *area_cache[AREA_CACHE_COUNT];

struct list_node *new_list() {
	struct list_node *list = malloc(sizeof(struct list_node));
	list->object = NULL;
	list->next = NULL;
	return list;
}

void add_object(struct list_node *list, void *object) {
	if (list->object == NULL) {
		list->object = object;
		return;
	}

	if (list->next == NULL) {
		list->next = malloc(sizeof(struct list_node));
		list->next->next = NULL;
		list->next->object = object;
		return;
	}

	add_object(list->next, object);
}

void free_list(struct list_node *list, bool free_objects) {
	if (list->next) {
		free_list(list->next, free_objects);
	}

	if (free_objects) {
		free(list->object);
	}

	free(list);
}

void destroy_area(struct game_area *area) {
	free(area);
}

/**
 Generates a new area and puts it in the game area cache.
 */
struct game_area *generate_area(int x, int y) {
	// Find empty slot in cache, otherwise evict the area furthest away from the generated one
	int free_index = -1;
	int biggest_distance_index = -1;
	float biggest_distance = -1.0;

	for (int i = 0; i < AREA_CACHE_COUNT; i++) {
		if (area_cache[i] == NULL) {
			free_index = i;
			break;
		} else {
			struct game_area *cached_area = area_cache[i];
			float distance = sqrtf(powf(x - cached_area->x, 2) + powf(y - cached_area->y, 2));
			if (distance > biggest_distance) {
				biggest_distance_index = i;
				biggest_distance = distance;
			}
		}
	}

	if (free_index == -1) {
		// No free indexes, so discard the area with the biggest distance
		destroy_area(area_cache[biggest_distance_index]);
		area_cache[biggest_distance_index] = NULL;
		free_index = biggest_distance_index;
	}

	struct game_area *area = malloc(sizeof(struct game_area));
	area->x = x;
	area->y = y;
	area_cache[free_index] = area;

	// Seed rand with this areas coordinates
	srand(x ^ y);
	struct game_object *previous = NULL;
	for (int i = 0; i < AREA_OBJECT_COUNT; i++) {
		struct game_object *object = &area->objects[i];
		*object = (struct game_object){0};
		object->position = (vec2){(double)rand() / (double)RAND_MAX * AREA_SIZE + (x * AREA_SIZE), (double)rand() / (double)RAND_MAX * AREA_SIZE + (y * AREA_SIZE)};
		if (previous)
			previous->next = object;
		previous = object;
	}

	return area;
}

struct list_node *get_visible_areas(float x, float y, float width, float height) {
	int start_area_x = (int)floor(x / AREA_SIZE);
	int start_area_y = (int)floor(y / AREA_SIZE);
	int end_area_x = (int)floor((x + width) / AREA_SIZE);
	int end_area_y = (int)floor((y + height) / AREA_SIZE);

	struct list_node *area_list = new_list();

	for (int x = start_area_x; x <= end_area_x; x++) {
		for (int y = start_area_y; y <= end_area_y; y++) {
			struct game_area *area = NULL;

			// See if the area exists in the cache
			for (int cache_index = 0; cache_index < AREA_CACHE_COUNT; cache_index++) {
				struct game_area *cached_area = area_cache[cache_index];
				if (cached_area != NULL && cached_area->x == x && cached_area->y == y) {
					area = cached_area;
					break;
				}
			}

			// If the area wasn't cached, generate it
			if (area == NULL) {
				area = generate_area(x, y);
			}

			add_object(area_list, area);
		}
	}

	return area_list;
}
