#ifndef WORLD_H
#define WORLD_H

#include <stdlib.h>

#include "../libs/os/camera.h"
#include "../libs/ds/array.h"
#include "../libs/math/vector.h"
#include "../libs/math/matrix.h"
#include "../win32/win_types.h"
#include "../libs/geometry_helper.h"
#include "../collision/collision.h"


enum Entity_Type {
	ENTITY_TYPE_UNKNOWN,
	ENTITY_TYPE_COMMON,
	ENTITY_TYPE_LIGHT,
	ENTITY_TYPE_GEOMETRY,
};

struct Entity_Id {
	Entity_Id(); // @Note: may be better to delete this constructor
	Entity_Id(Entity_Type type, u32 index);

	Entity_Type type;
	u32 index;

	bool operator==(const Entity_Id &other)
	{
		if ((type == other.type) && (index == other.index)) {
			return true;
		}
		return false;
	}
};

struct Entity {
	Entity() { type = ENTITY_TYPE_COMMON; bounding_box_type = BOUNDING_BOX_TYPE_UNKNOWN; }
	u32 id;
	Entity_Type type;
	Vector3 position;
	
	Boudning_Box_Type bounding_box_type;
	AABB AABB_box;
};

enum Geometry_Type {
	GEOMETRY_TYPE_BOX,
	GEOMETRY_TYPE_GRID,
	GEOMETRY_TYPE_SPHERE
};

struct Geometry_Entity : Entity {
	Geometry_Entity() { type = ENTITY_TYPE_GEOMETRY; }
	Geometry_Type geometry_type;
	union {
		Box box;
		Grid grid;
		Sphere sphere;
	};
};

enum Light_Type : u32 {
	SPOT_LIGHT_TYPE = 0,
	POINT_LIGHT_TYPE = 1,
	DIRECTIONAL_LIGHT_TYPE = 2,
};

struct Light : Entity {
	Light() { type = ENTITY_TYPE_LIGHT; }
	
	Light_Type light_type;
	
	float range;
	float radius;

	Vector3 color;
	Vector3 direction;
};

struct Game_World {
	u32 light_hash = 0;
	u32 id_count = 0;
	
	Array<Entity> entities;
	Array<Light> lights;
	Array<Geometry_Entity> geometry_entities;

	void init();

	void set_entity_AABB(Entity_Id entity_id, AABB *bounding_box);

	Entity *get_entity(Entity_Id entity_id);

	Entity_Id make_geometry_entity(const Vector3 &position, Geometry_Type geometry_type, void *data);
	
	Entity_Id make_spot_light(const Vector3 &position, const Vector3 &diretion, const Vector3 &color, float radius);
	Entity_Id make_point_light(const Vector3 &position, const Vector3 &color, float range);
	Entity_Id make_direction_light(const Vector3 &direction, const Vector3 &color);

};
#endif