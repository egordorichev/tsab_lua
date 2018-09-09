#ifndef TSAB_PHYSICS
#define TSAB_PHYSICS

#include <Box2D/Box2D.h>

#include "lua.hpp"

// TODO: implement those in lua
#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

void tsab_physics_register_api(lua_State *L);

int tsab_physics_new_world(lua_State *L);
int tsab_physics_destroy_world(lua_State *L);
int tsab_physics_update(lua_State *L);
int tsab_physics_new_body(lua_State *L);
int tsab_physics_destroy_body(lua_State *L);
int tsab_physics_set_body_transform(lua_State *L);
int tsab_physics_get_body_transform(lua_State *L);
int tsab_physics_set_body_velocity(lua_State *L);
int tsab_physics_get_body_velocity(lua_State *L);

#endif