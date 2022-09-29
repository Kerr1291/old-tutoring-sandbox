#ifndef SANDBOX_H
#define SANDBOX_H

#include <SandboxGlobals.h>

//Systems
#include <RenderManager.h>
#include <HUDManager.h>

class INIFile;
class Camera;
class Scene;

class Sandbox
{
	public:
		Sandbox(INIFile* configFile);
		~Sandbox();
		
		void Initialize();
		void Input(const bool* keys, float fps);
		void Update(float fps);
		void Draw();
		
		INIFile* config;

	private:
		
		void DrawWorldCenteredDebugCross();
		
		Camera* mCamera;
		HUDManager hudDisplay;
		RenderManager renderer;
		Scene* mSandboxScene;
		
		float sandboxfps;
		truth showHUD;
};



#endif