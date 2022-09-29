#ifndef SCENE_H
#define SCENE_H

#include <SandboxGlobals.h>

class Scene
{
	public:

	Scene();
	~Scene();

	void Initialize();

	void Input();

	void Update(float dt);

	
	std::vector<vec3> pos;
	std::vector<mat4> rot;
	float scale;
};

#endif // SCENE_H