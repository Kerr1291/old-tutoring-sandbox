#ifndef SANDBOXHUD_H
#define SANDBOXHUD_H

#include <HUD.h>

class SandboxHUD : public HUD
{
public:
	SandboxHUD(Sandbox& _sandbox);
	virtual ~SandboxHUD();
	virtual void Load();
	virtual void Update();
	virtual void Draw();
	Sandbox& sandbox;
private:
};

#endif