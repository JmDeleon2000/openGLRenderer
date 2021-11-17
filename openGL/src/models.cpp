#include "models.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

Model::Model(const char* path) 
{
	fstream fileStream;
	string line;
	string fverts, fuv, fnormals, ffaces;

	fileStream.open(path, ios::in);
	if (!fileStream.is_open()) return;
	vertCount = uvCount = normCount = faceCount = 0;

	while (getline(fileStream, line))
	{
		if (line[0] == 'v' && line[1] == 'n') 
		{
			line.erase(0, 3);
			fnormals.append(line + ' '); 
			normCount+=3;
			continue;
		}
		if (line[0] == 'v' && line[1] == 't')
		{
			line.erase(0, 3);
			fuv.append(line + ' ');
			uvCount+=2;
			continue;
		}
		if (line[0] == 'v') 
		{ 
			line.erase(0, 2);
			fverts.append(line + ' '); 
			vertCount+=3;
			continue;
		}
		if (line[0] == 'f') 
		{
			line.erase(0, 2);
			ffaces.append(line + '\n');
			faceCount++;
		}
	}
	fileStream.close();

	verts = new float[vertCount];
	int i = 0, ws;
	while (i < vertCount)
	{
		ws = fverts.find(' ');
		verts[i] = atof(fverts.substr(0, ws).c_str());
		fverts.erase(0, ws + 1);
		i++;
	}
	vertCount /= 3;
	uvs = new float[uvCount];
	i = 0;
	while (i < uvCount)
	{
		ws = fuv.find(' ');
		uvs[i] = atof(fuv.substr(0, ws).c_str());
		fuv.erase(0, ws + 1);
		i++;
	}
	uvCount /= 2;
	normals = new float[normCount];
	i = 0;
	while (i < normCount)
	{
		ws = fnormals.find(' ');
		normals[i] = atof(fnormals.substr(0, ws).c_str());
		fnormals.erase(0, ws + 1);
		i++;
	}
	normCount /= 3;
	/* 24 = (3 floats/vertex 3 floats/normal 2 floats/uvs)*3 vertices/face  */
	mainBuffer = new float[faceCount * 24];
	string sub;
	int slash, bufferIndex = 0;
	int v1, vt1, vn1;
	int v2, vt2, vn2;
	int v3, vt3, vn3;
	while (ffaces.length() > 0)
	{
		ws = ffaces.find('\n');
		sub = ffaces.substr(0, ws);
		ffaces.erase(0, ws + 1);

		slash = sub.find('/');
		v1 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);

		slash = sub.find('/');
		vt1 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);

		slash = sub.find(' ');
		vn1 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);


		slash = sub.find('/');
		v2 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);

		slash = sub.find('/');
		vt2 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);

		slash = sub.find(' ');
		vn2 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);


		slash = sub.find('/');
		v3 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);

		slash = sub.find('/');
		vt3 = atoi(sub.substr(0, slash).c_str()) - 1;
		sub.erase(0, slash + 1);

		vn3 = atoi(sub.c_str()) - 1;

		memcpy(&mainBuffer[bufferIndex], &verts[v1 * 3], sizeof(float) * 3);
		bufferIndex += 3;

		memcpy(&mainBuffer[bufferIndex], &normals[vn1 * 3], sizeof(float) * 3);
		bufferIndex += 3;

		memcpy(&mainBuffer[bufferIndex], &uvs[vt1 * 2], sizeof(float) * 2);
		bufferIndex += 2;

		memcpy(&mainBuffer[bufferIndex], &verts[v2 * 3], sizeof(float) * 3);
		bufferIndex += 3;

		memcpy(&mainBuffer[bufferIndex], &normals[vn2 * 3], sizeof(float) * 3);
		bufferIndex += 3;

		memcpy(&mainBuffer[bufferIndex], &uvs[vt2 * 2], sizeof(float) * 2);
		bufferIndex += 2;

		memcpy(&mainBuffer[bufferIndex], &verts[v3 * 3], sizeof(float) * 3);
		bufferIndex += 3;

		memcpy(&mainBuffer[bufferIndex], &normals[vn3 * 3], sizeof(float) * 3);
		bufferIndex += 3;

		memcpy(&mainBuffer[bufferIndex], &uvs[vt3 * 2], sizeof(float) * 2);
		bufferIndex += 2;
	}


}

Model::~Model() 
{
	delete[] verts;
	delete[] uvs;
	delete[] normals;
	delete[] mainBuffer;
	//TODO check if this works
}