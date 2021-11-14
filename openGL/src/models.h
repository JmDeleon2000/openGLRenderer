#pragma once


class Model 
{
public:
	float* verts;
	float* uvs;
	float* normals;
	int* faces;
	//int* faceSizes;

public:
	Model() {};
	Model(float* verts, float* uvs, float* normals, int* faces) {};
};