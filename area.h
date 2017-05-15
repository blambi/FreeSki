#ifndef AREA_H
#define AREA_H

#include <stdbool.h>
#include "geometry.h"

enum object_type {
	SKIER,
	TREE,
	STUMP,
	ROCK,
	SMALL_HUMP,
	LARGE_HUMP,
	OBJECT_TYPE_COUNT
};

struct game_object {
	enum object_type type;
	vec2 position;
	vec2 velocity;
	struct game_object *next;
};

#define AREA_OBJECT_COUNT 20

struct game_area {
	int x;
	int y;
	struct game_object objects[AREA_OBJECT_COUNT];
};

struct list_node {
	void *object;
	struct list_node *next;
};

void add_object(struct list_node *list, void *object);
void free_list(struct list_node *list, bool free_objects);

/**
 For a visible camera rect, get all areas that are visible in that rect. Any
 area not currently in memory will be generated.
 */
struct list_node *get_visible_areas(float x, float y, float width, float height);

/**
 Clears all generated areas that are not inside the supplied rect.
 */
struct game_area *clear_areas_outside(float x, float y, float width, float height);

#endif
