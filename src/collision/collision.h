#ifndef HADES_COLLISION_H
#define HADES_COLLISION_H

#include "../libs/math/vector.h"
#include "../render/model.h"

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

#endif
