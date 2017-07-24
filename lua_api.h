#ifndef LUA_API_H
#define LUA_API_H

#include <stdio.h> // For debug only

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <SDL2/SDL_keyboard.h>

void init_lua(char *script_filename);
void cleanup_lua();

// Callbacks
void lua_on_keydown(SDL_Keycode sym);

// C functions for LUA
void skier_update_velocity(double x, double y);
#endif
