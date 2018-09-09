#include "physics.hpp"
#include "tsab.hpp"
#include "graphics.hpp"
#include "shaders.hpp"

#include <vector>
#include <iostream>

b2World *world;
std::vector<b2Body *> body_list;
DebugView debug;

void tsab_physics_register_api(lua_State *L) {
	lua_register(L, "tsab_physics_new_world", tsab_physics_new_world);
	lua_register(L, "tsab_physics_update", tsab_physics_update);
	lua_register(L, "tsab_physics_draw", tsab_physics_draw);
	lua_register(L, "tsab_physics_destroy_world", tsab_physics_destroy_world);
	lua_register(L, "tsab_physics_new_body", tsab_physics_new_body);
	lua_register(L, "tsab_physics_destroy_body", tsab_physics_destroy_body);
	lua_register(L, "tsab_physics_new_world", tsab_physics_new_world);
	lua_register(L, "tsab_physics_set_body_transform", tsab_physics_set_body_transform);
	lua_register(L, "tsab_physics_get_body_transform", tsab_physics_get_body_transform);
	lua_register(L, "tsab_physics_set_body_velocity", tsab_physics_set_body_velocity);
	lua_register(L, "tsab_physics_get_body_velocity", tsab_physics_get_body_velocity);
}

int tsab_physics_new_world(lua_State *L) {
	if (world != nullptr) {
		std::cerr << "World is already created!\n";
		return 0;
	}

	world = new b2World(b2Vec2(check_number(L, 1, 0), check_number(L, 2, 40)));
	world->SetAllowSleeping(check_bool(L, 3, true));
	world->SetContinuousPhysics(check_bool(L, 3, true));
	debug.SetFlags(b2Draw::e_shapeBit);
	world->SetDebugDraw(&debug);

	// world->SetContactListener(this);

	return 0;
}

int tsab_physics_update(lua_State *L) {
	if (world != nullptr) {
		world->Step(timer_fixed_dt, check_number(L, 1, 8), check_number(L, 2, 3));
	}

	return 0;
}

int tsab_physics_draw(lua_State *L) {
	if (world != nullptr) {
		world->DrawDebugData();
	}

	return 0;
}

int tsab_physics_destroy_world(lua_State *L) {
	delete world;
	world = nullptr;

	return 0;
}

void no_world() {
	std::cerr << "World was not created yet\n";
}

int tsab_physics_new_body(lua_State *L) {
	if (world == nullptr) {
		no_world();
		return 0;
	}

	b2BodyDef def;

	const char *type = luaL_checkstring(L, 1);

	if (strcmp(type, "dynamic") == 0) {
		def.type = b2_dynamicBody;
	} else if (strcmp(type, "kinematic") == 0) {
		def.type = b2_kinematicBody;
	} else {
		if (strcmp(type, "kinematic") != 0) {
			std::cerr << "Unknown body type " << type << ", setting to static. Should be one of: dynamic, kinematic, static\n";
		}

		def.type = b2_staticBody;
	}

	b2Body* body = world->CreateBody(&def);
	b2FixtureDef fixture;
	b2PolygonShape polygonShape;
	b2CircleShape circleShape;

	if (lua_istable(L, 2)) {
		lua_getfield(L, 2, "shape");
		lua_getfield(L, 2, "x");
		lua_getfield(L, 2, "y");
		lua_getfield(L, 2, "centred");

		const char *shape = check_string(L, -4, "rect");
		double x = check_number(L, -3, 0);
		double y = check_number(L, -2, 0);
		bool centered = check_bool(L, -1, false);
		lua_pop(L, 4);

		if (strcmp(shape, "rect") == 0) {
			lua_getfield(L, 2, "w");
			lua_getfield(L, 2, "h");
			double w = check_number(L, -2, 1);
			double h = check_number(L, -1, 1);

			b2Vec2 vertices[4];

			if (centered) {
				vertices[0].Set(-w / 2, -h / 2);
				vertices[1].Set(-w / 2,  h / 2);
				vertices[2].Set(w / 2, h / 2);
				vertices[3].Set(w / 2,  -h / 2);
			} else {
				vertices[0].Set(0,  0);
				vertices[1].Set(0,  h);
				vertices[2].Set(w, h);
				vertices[3].Set(w,  0);
			}

			polygonShape.Set(vertices, 4);
			fixture.shape = &polygonShape;

			lua_pop(L, 2);
		} else if (strcmp(shape, "circle") == 0) {
			lua_getfield(L, 2, "h");
			double r = check_number(L, -1, 1);

			circleShape.m_radius = r;
			circleShape.m_p.x = x;
			circleShape.m_p.y = y;

			if (!centered) {
				circleShape.m_p.x += r;
				circleShape.m_p.y += r;
			}

			fixture.shape = &circleShape;

			lua_pop(L, 1);
		} else {
			std::cerr << "Wrong shape type!\n";
			polygonShape.SetAsBox(1, 1);
			fixture.shape = &polygonShape;
		}
	} else {
		polygonShape.SetAsBox(1, 1);
		fixture.shape = &polygonShape;
	}

	body->CreateFixture(&fixture);

	body_list.push_back(body);
	lua_pushnumber(L, body_list.size() - 1);

	fixture.isSensor = check_bool(L, 3, false);

	return 1;
}

int tsab_physics_destroy_body(lua_State *L) {
	if (world == nullptr) {
		no_world();
		return 0;
	}

	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < body_list.size()) {
		b2Body *body = body_list[id];
		body_list.erase(body_list.begin() + id);
		world->DestroyBody(body);
	}

	return 0;
}

int tsab_physics_set_body_transform(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < body_list.size()) {
		b2Body *body = body_list[id];
		body->SetTransform(b2Vec2(luaL_checknumber(L, 2), luaL_checknumber(L, 3)), luaL_checknumber(L, 4));
	}

	return 0;
}

int tsab_physics_get_body_transform(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < body_list.size()) {
		b2Body *body = body_list[id];

		lua_pushnumber(L, body->GetPosition().x);
		lua_pushnumber(L, body->GetPosition().y);
		lua_pushnumber(L, body->GetAngle());

		return 3;
	}

	return 0;
}

int tsab_physics_set_body_velocity(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < body_list.size()) {
		b2Body *body = body_list[id];
		body->SetLinearVelocity(b2Vec2(luaL_checknumber(L, 2), luaL_checknumber(L, 3)));
	}

	return 0;
}

int tsab_physics_get_body_velocity(lua_State *L) {
	int id = luaL_checknumber(L, 1);

	if (id > -1 && id < body_list.size()) {
		b2Body *body = body_list[id];

		lua_pushnumber(L, body->GetLinearVelocity().x);
		lua_pushnumber(L, body->GetLinearVelocity().y);

		return 2;
	}

	return 0;
}

void DebugView::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	float ver[vertexCount * 2];

	for (int i = 0; i < vertexCount; i++) {
		ver[i * 2] = vertices[i].x + 0.5;
		ver[i * 2 + 1] = vertices[i].y + 0.5;
	}

	if (tsab_shaders_get_active() > -1) {
		float colors[] = {
			(float) color.r, (float) color.g, (float) color.b, (float) color.a
		};

		GPU_SetUniformfv(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "color"), 4, 1, (float *) colors);
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	GPU_Polygon(tsab_graphics_get_current_target(), vertexCount, ver, { (Uint8) (color.r * 255), (Uint8) (color.g * 255), (Uint8) (color.b * 255), (Uint8) (color.a * 255) });
}

void DebugView::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	float ver[vertexCount * 2];

	for (int i = 0; i < vertexCount; i++) {
		ver[i * 2] = vertices[i].x + 0.5;
		ver[i * 2 + 1] = vertices[i].y + 0.5;
	}

	if (tsab_shaders_get_active() > -1) {
		float colors[] = {
			(float) color.r, (float) color.g, (float) color.b, (float) color.a
		};

		GPU_SetUniformfv(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "color"), 4, 1, (float *) colors);
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	GPU_Polygon(tsab_graphics_get_current_target(), vertexCount, ver, { (Uint8) (color.r * 255), (Uint8) (color.g * 255), (Uint8) (color.b * 255), (Uint8) (color.a * 255) });
}

void DebugView::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	if (tsab_shaders_get_active() > -1) {
		float colors[] = {
			(float) color.r, (float) color.g, (float) color.b, (float) color.a
		};

		GPU_SetUniformfv(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "color"), 4, 1, (float *) colors);
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	GPU_Circle(tsab_graphics_get_current_target(), center.x, center.y, radius, { (Uint8) (color.r * 255), (Uint8) (color.g * 255), (Uint8) (color.b * 255), (Uint8) (color.a * 255) });
}

void DebugView::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	if (tsab_shaders_get_active() > -1) {
		float colors[] = {
			(float) color.r, (float) color.g, (float) color.b, (float) color.a
		};

		GPU_SetUniformfv(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "color"), 4, 1, (float *) colors);
		GPU_SetUniformf(GPU_GetUniformLocation(tsab_shaders_get_active_shader(), "textured"), 0);
	}

	GPU_CircleFilled(tsab_graphics_get_current_target(), center.x, center.y, radius, { (Uint8) (color.r * 255), (Uint8) (color.g * 255), (Uint8) (color.b * 255), (Uint8) (color.a * 255) });
}

void DebugView::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {

}

void DebugView::DrawTransform(const b2Transform& xf) {

}

void DebugView::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
	GPU_Pixel(tsab_graphics_get_current_target(), p.x, p.y, { (Uint8) (color.r * 255), (Uint8) (color.g * 255), (Uint8) (color.b * 255), (Uint8) (color.a * 255) });
}