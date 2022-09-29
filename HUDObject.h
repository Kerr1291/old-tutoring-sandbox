#ifndef HUDOBJECT_H
#define HUDOBJECT_H

#include <SandboxGlobals.h>

class Sandbox;

struct HUDObject
{
	HUDObject(Sandbox* sandbox);
	~HUDObject();
	Sandbox* sandbox;

	truth hidden;
	truth solid;
	vec4 color;
	vec4 textColor;
	vec2 bottomLeft;
	vec2 size;
	std::vector<std::string> text;
	float brightness;
	std::vector<vec2> vertices;

	void Update();
	void Draw();
	truth MouseOver();
};

#endif
