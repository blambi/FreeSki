/*
	First of all just at test, later try to move game logic to lua

	Compile:
	gcc -o luatest -Wall `pkg-config --libs --cflags lua5.3` lua_api.c -lm
*/
#include "lua_api.h"

//static int c_sin(lua_State *state);
//void test_lua(int a, int b);
static int c_update_skier_velocity(lua_State *state);

lua_State *lua_state;

/*int main (int argc, char *argv[]) {
		// Test code, remove later
		init_lua("freeski.lua");
		test_lua(5, 7);
		cleanup_lua();
		}*/

void init_lua(char *script_filename) {
		lua_state = luaL_newstate();
		luaL_openlibs(lua_state); // Maybe just open the ones we really need?

		// push a c functions to lua
		lua_pushcfunction(lua_state, c_update_skier_velocity);
		lua_setglobal(lua_state, "update_skier_velocity");

		luaL_dofile(lua_state, script_filename);
}

void cleanup_lua() {
		lua_close(lua_state);
}

void lua_on_keydown(SDL_Keycode sym) {
		lua_getglobal(lua_state, "on_keydown");
		lua_pushnumber(lua_state, sym); // Add arguments
		lua_call(lua_state, 1, 0); // Call the function with two arguments and one return
}

static int c_update_skier_velocity(lua_State *state) {
 	  /* Takes one argument (a float) and returns another */
 		double x = luaL_checknumber(state, 1);
		double y = luaL_checknumber(state, 1);

		skier_update_velocity(x, y);

 		return 0; // Number of results/returns */
}

/* void test_lua(int a, int b) { */
/* 		// Test talking to lua and getting something back */
/* 		int result; */
		
/* 		lua_getglobal(lua_state, "test"); // lookup function */
/* 		lua_pushnumber(lua_state, a); // Add arguments */
/* 		lua_pushnumber(lua_state, b); */
/* 		lua_call(lua_state, 2, 1); // Call the function with two arguments and one return */
		
/* 		result = (int)lua_tonumber(lua_state, -1); // -1? no idea, index? */
/* 		lua_pop(lua_state, 1); */

/* 		printf("GOT: %d\n", result); */
/* } */

/* static int c_sin(lua_State *state) { */
/* 		/\* Takes one argument (a float) and returns another *\/ */
/* 		double d = luaL_checknumber(state, 1); */
/* 		lua_pushnumber(state, sin(d)); */
/* 		return 1; // Number of results/returns */
/* } */
