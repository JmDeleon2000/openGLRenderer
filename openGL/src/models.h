#pragma once

class Model 
{
public:
	float* mainBuffer;
	int vertCount, uvCount, normCount, faceCount;
private:
	float* verts;
	float* uvs;
	float* normals;
public:
	Model(const char* path);
	~Model();
};