#ifndef VERTEX_H
#define VERTEX_H

#include <d3d11.h>
#include "../libs/math/vector.h"
#include "../libs/ds/hash_table.h"


struct Vertex_XC {
	Vertex_XC() {}
	Vertex_XC(const Vector2 &position, const Vector4 &color) : position(position), color(color) {}
	
	Vector2 position;
	Vector4 color;
};

struct Vertex_X2UV {
	Vertex_X2UV() {}
	Vertex_X2UV(const Vector2 &position, const Vector2 &uv) : position(position), uv(uv) {}
	
	Vector2 position;
	Vector2 uv;
};

struct Vertex_XNUV {
	Vertex_XNUV() {}
	Vertex_XNUV(Vector3 position, Vector3 normal, Vector2 uv) : position(position), normal(normal), uv(uv) {}
	Vertex_XNUV(float p1, float p2, float p3, float n1, float n2, float n3, float uv1, float uv2) : Vertex_XNUV(Vector3(p1, p2, p3), Vector3(n1, n2, n3), Vector2(uv1, uv2)) {}
	
	Vector3 position;
	Vector3 normal;
	Vector2 uv;
};
#endif