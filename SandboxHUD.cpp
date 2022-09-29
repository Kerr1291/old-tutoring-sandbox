#include <SandboxHUD.h>
#include <Sandbox.h>

#include <Camera.h>

#include <sstream>

SandboxHUD::SandboxHUD(Sandbox& _sandbox)
:sandbox(_sandbox)
{
}

SandboxHUD::~SandboxHUD()
{
}

void SandboxHUD::Load()
{
	hudObjects.push_back( HUDObject( &sandbox ) );
	hudObjects.push_back( HUDObject( &sandbox ) );
	hudObjects.push_back( HUDObject( &sandbox ) );
	hudObjects.push_back( HUDObject( &sandbox ) );

	hudObjects[0].vertices = MakeBox(1,1);
	hudObjects[0].solid = 0;
	hudObjects[0].text.push_back( "Camera:" );
	hudObjects[0].bottomLeft = vec2(0.05f * Global::Window::Width, 0.95f * Global::Window::Height);

	//This is an example of a HUD object
	//Warning: Text is slow! (This object alone knocks off about 60 fps)
	hudObjects[1].vertices = MakeBox(1,1);
	hudObjects[1].color = vec4(0.4,0.4,0.4,1);
	hudObjects[1].textColor = vec4(1.0,0.0,0.0,1.0);
	hudObjects[1].solid = 1;
	hudObjects[1].text.push_back( "Hello! (Press 9 to hide/show this stuff)" );
	hudObjects[1].text.push_back( "W/S = Move Forward/Backward" );
	hudObjects[1].text.push_back( "A/D = Move Sideways" );
	hudObjects[1].text.push_back( "Q/E = Move Up/Down" );
	hudObjects[1].text.push_back( "+/- = Change Move Speed" );
	hudObjects[1].text.push_back( "Hold Mouse2 = Turn Camera" );
	hudObjects[1].size = vec2( hudObjects[1].text[0].size() * 10, 12*hudObjects[1].text.size() + 10 );
	hudObjects[1].bottomLeft = vec2(0.05f * Global::Window::Width, 0.80f * Global::Window::Height);

}

void SandboxHUD::Update()
{
	//Here is an example of making a HUD object with some text that updates based on a variable
	{ //Placing this in its own scope to reuse the name tostr if we want

		//Here we use a stringstream to turn some numbers into text
		std::stringstream tostr;
		tostr << External::camera->GetPos().x;
		tostr << ", ";
		tostr << External::camera->GetPos().y;
		tostr << ", ";
		tostr << External::camera->GetPos().z;

		//Set the text to be displayed (for this frame)
		hudObjects[0].text[0] = "Camera: " + tostr.str();
		
		//Fineally, set how big it is (based on the text size)
		float newSize = hudObjects[0].text[0].size() * 9.0f;
		hudObjects[0].size = vec2( ( newSize < hudObjects[0].size.x ? hudObjects[0].size.x : newSize ), 20 );
	}

	for(unsigned i = 0; i < hudObjects.size(); ++i)
		hudObjects[i].Update();
}

void SandboxHUD::Draw()
{
	for(unsigned i = 0; i < hudObjects.size(); ++i)
		hudObjects[i].Draw();
}