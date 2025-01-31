#ifndef _COLLISION_H
#define _COLLISION_H

#include "../render/mesh.h"
#include "../libs/math/vector.h"
#include "../libs/math/structures.h"

enum Boudning_Box_Type {
	BOUNDING_BOX_TYPE_UNKNOWN,
	BOUNDING_BOX_TYPE_AABB,
	BOUNDING_BOX_TYPE_OBB,
	BOUNDING_BOX_TYPE_SPHERE
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct Bounding_Sphere {
	float radious;
	Vector3 postion;
};

AABB make_AABB(Triangle_Mesh *mesh);
Bounding_Sphere make_bounding_sphere(const Vector3 &position, Triangle_Mesh *mesh);

bool detect_intersection(Ray *ray, AABB *aabb, Vector3 *intersection_point = NULL);
bool detect_intersection(float radius, const Vector2 &circle_center, const Vector2 &test_point);

#endif

