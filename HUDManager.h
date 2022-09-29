#ifndef HUDMANAGER_H
#define HUDMANAGER_H

#include <SandboxGlobals.h>

#include <string>
#include <map>

class HUD;

class HUDManager
{
public:
	HUDManager();
	~HUDManager();

	void AddHUD(const std::string& hudname, HUD* hud);
	void SetActiveHUD(const std::string& hudname);
	
	void Update();
	void Draw();


private:
	HUD* activeHUD;
	std::map<std::string, HUD*> loadedHUDs;
};


#endif