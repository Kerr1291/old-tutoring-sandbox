#ifndef GRAPHICDATA_H
#define GRAPHICDATA_H

#include <SandboxGlobals.h>

struct GraphicData
{
	friend class RenderManager;
	GraphicData();
	~GraphicData();

	private:

	
	void CreateRenderData(vec3* verts, unsigned _numVerts, unsigned* indices, unsigned _numIndices);
	void CreateBox();
	void CreateBox(float sx, float sy, float sz);
	void CreateSphere();

	vec3* vertexArray;
	unsigned* indexArray;
	unsigned numVerts;
	unsigned numIndices;
	unsigned vao;
	unsigned buffer;

	std::string name;
};


#endif