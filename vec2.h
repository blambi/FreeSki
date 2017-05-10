#ifndef VEC2_H
#define VEC2_H

typedef struct {
	float x;
	float y;
} vec2;

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

#endif
