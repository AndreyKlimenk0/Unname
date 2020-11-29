#include "matrix.h"

Matrix3 mat3_indentity = Matrix3(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(0, 0, 1));
Matrix4 mat4_indentity = Matrix4(Vector4(1, 0, 0, 0), Vector4(0, 1, 0, 0), Vector4(0, 0, 1, 0), Vector4(0, 0, 0, 1));

void print_mat(Matrix4 &mat)
{
	printf("%f, %f, %f, %f\n", mat.matrix[0].x, mat.matrix[0].y, mat.matrix[0].z, mat.matrix[0].w);
	printf("%f, %f, %f, %f\n", mat.matrix[1].x, mat.matrix[1].y, mat.matrix[1].z, mat.matrix[1].w);
	printf("%f, %f, %f, %f\n", mat.matrix[2].x, mat.matrix[2].y, mat.matrix[2].z, mat.matrix[2].w);
	printf("%f, %f, %f, %f\n", mat.matrix[3].x, mat.matrix[3].y, mat.matrix[3].z, mat.matrix[3].w);
}


void print_mat(Matrix3 &mat)
{
	printf("%f, %f , %f\n", mat.matrix[0].x, mat.matrix[0].y, mat.matrix[0].z);
	printf("%f, %f , %f\n", mat.matrix[1].x, mat.matrix[1].y, mat.matrix[1].z);
	printf("%f, %f , %f\n", mat.matrix[2].x, mat.matrix[2].y, mat.matrix[2].z);
}