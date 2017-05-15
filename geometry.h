#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <stdbool.h>

typedef struct {
	float x;
	float y;
} vec2;

typedef struct {
	float x;
	float y;
	float width;
	float height;
} rect;

static inline vec2 vec2_add(vec2 a, vec2 b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

static inline vec2 vec2_subtract(vec2 a, vec2 b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

static inline bool rect_intersect(rect a, rect b) {
	return !(b.x > a.x + a.width || b.x + b.width < a.x || b.y > a.y + a.height || b.y + b.height < a.y);
}

#endif
