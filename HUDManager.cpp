#include <HUDManager.h>

#include <HUD.h>

HUDManager::HUDManager()
:activeHUD(0)
{}

HUDManager::~HUDManager()
{
	std::map<std::string, HUD*>::iterator iter = loadedHUDs.begin();
	for(; iter != loadedHUDs.end(); ++iter)
		delete iter->second;
}

void HUDManager::AddHUD(const std::string& hudname, HUD* hud)
{
	if( loadedHUDs.find( hudname ) != loadedHUDs.end() )
		return;

	hud->Load();
	loadedHUDs[ hudname ] = hud;
}

void HUDManager::SetActiveHUD(const std::string& hudname)
{
	if( loadedHUDs.find( hudname ) == loadedHUDs.end() )
		return;

	activeHUD = loadedHUDs[ hudname ];
}

void HUDManager::Update()
{
	if( activeHUD )
	{
		activeHUD->Update();
	}
}

void HUDManager::Draw()
{
	if( activeHUD )
	{
		activeHUD->Draw();
	}
}