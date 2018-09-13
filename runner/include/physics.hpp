#ifndef TSAB_PHYSICS
#define TSAB_PHYSICS

#include <Box2D/Box2D.h>

#include "lua.hpp"

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

void tsab_physics_register_api(lua_State *L);

int tsab_physics_new_world(lua_State *L);
int tsab_physics_destroy_world(lua_State *L);
int tsab_physics_update(lua_State *L);
int tsab_physics_draw(lua_State *L);
int tsab_physics_new_body(lua_State *L);
int tsab_physics_destroy_body(lua_State *L);
int tsab_physics_set_body_transform(lua_State *L);
int tsab_physics_get_body_transform(lua_State *L);
int tsab_physics_set_body_velocity(lua_State *L);
int tsab_physics_get_body_velocity(lua_State *L);
int tsab_physics_add_fixture(lua_State *L);

class DebugView : public b2Draw
{
	public:
		void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
		void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
		void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
		void DrawTransform(const b2Transform& xf);
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
};

#endif