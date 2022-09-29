#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <SandboxGlobals.h>

struct GraphicData;
struct Transform;

struct RenderObject
{
	friend class RenderManager;
	friend class Camera;

	RenderObject(GraphicData* g);
	~RenderObject();

	void SetColor(vec4 rgba);
	void SetColor(float r, float g, float b, float a = 1.0f);

	truth visible;
	truth solid;

	private:

	float* scale;
	mat4* rot;
	vec3* pos;
	
	vec4 color;
	GraphicData* gfx;

	std::string name;
};


#endif