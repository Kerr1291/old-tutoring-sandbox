#ifndef HUD_H
#define HUD_H

#include <SandboxGlobals.h>

#include <HUDObject.h>

class HUD
{
public:
	virtual ~HUD(){}
	virtual void Load() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
protected:
	std::vector<glm::vec2> MakeBox(unsigned w, unsigned h);
	std::vector<HUDObject> hudObjects;
};





#endif