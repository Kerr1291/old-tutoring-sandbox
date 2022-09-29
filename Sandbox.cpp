#include <Sandbox.h>

#include <freeglut.h>

#include <Parser.h>
#include <Camera.h>
#include <SandboxHUD.h>
#include <RenderManager.h>
		
#include <Scene.h>

/////////////////////////////////////////////////////////
//Public Methods	

/////////////////////////////////////////////////////////////////////
// Basic default ctor. Takes a configfile to allow you to read in
// data from a file easily and use it as needed.
Sandbox::Sandbox(INIFile* configFile)
:config(configFile)
,mCamera(0)
,mSandboxScene(0)
,sandboxfps(0.016f)
,showHUD(1)
{
	External::sandbox = this;
}

/////////////////////////////////////////////////////////////////////
// Free up anything allocated by the sandbox
Sandbox::~Sandbox()
{
	delete mCamera;
	delete mSandboxScene;
}


/////////////////////////////////////////////////////////////////////
// Here we initialize all the systems the sandbox uses.
void Sandbox::Initialize()
{
	/////////////////////////////////////////////////////////////////////
	//Make sure these happen first in initialize!
	//This creates a camera and assigns the global pointer to this camera
	mCamera = new Camera();
	External::camera = mCamera;
	//
	/////////////////////////////////////////////////////////////////////

	
	/////////////////////////////////////////////////////////////////////
	//TODO: Init physics system here
	//
	/////////////////////////////////////////////////////////////////////
	
	
	/////////////////////////////////////////////////////////////////////
	//Init the scene here
	mSandboxScene = new Scene();
	mSandboxScene->Initialize();
	//
	/////////////////////////////////////////////////////////////////////
	

	/////////////////////////////////////////////////////////////////////
	//Create a HUD for us and set it as active
	hudDisplay.AddHUD( "SandboxHUD", new SandboxHUD(*this) );
	hudDisplay.SetActiveHUD( "SandboxHUD" );
	//
	/////////////////////////////////////////////////////////////////////
}

void Sandbox::Input(const bool* keys, float fps)
{
	//special debug keys go here, these should activate before any others
	if( Global::Keyboard::KeyPressed('0') )
	{
		Global::SetDebugMode(( Global::DebugState ? 0 : 1 ));
	}
	
	if( Global::Keyboard::KeyPressed('9') )
	{
		showHUD = (showHUD ? 0 : 1);
	}

	/////////////////////////////////////////////////////////////////////
	//Do any input checking for our scene
	mSandboxScene->Input();
	//
	/////////////////////////////////////////////////////////////////////

	mCamera->Input( keys, fps );
}

void Sandbox::Update(float fps)
{
	/////////////////////////////////////////////////////////////////////
	//TODO: update physics system here
	//
	/////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////
	//Update the scene
	mSandboxScene->Update(fps);
	//
	/////////////////////////////////////////////////////////////////////

	mCamera->SyncWithTarget(fps);

	if(showHUD)
		hudDisplay.Update();
}

/////////////////////////////////////////////////////////////////////
// You shouldn't ever need to touch this function.
// This function does the drawing of things in a specific order.
//
// If you want to disable the coordinate system cross just comment
//   out the specific function below.
void Sandbox::Draw()
{
	if(showHUD)
		hudDisplay.Draw();
	
	if(Global::DebugState)
		Debug::Internal::PrintStrings();

	DrawWorldCenteredDebugCross(); //Draws the world centered cross
	renderer.Draw();
}
		
/////////////////////////////////////////////////////////
//Private Methods		


/////////////////////////////////////////////////////////////////////////////
// This function draws a coordinate system cross in the center of the world
// If you want it bigger/smaller just change the 'num' value.
void Sandbox::DrawWorldCenteredDebugCross()
{
	const unsigned num = 20;
	const float offset = num;

	glBegin(GL_LINES);
	
	glColor3f(0,0,1);
	glVertex3f( -1, 0, 0 );
	glVertex3f( 1, 0, 0 );

	for(unsigned i = 0; i < num; ++i)
	{
		glVertex3f( (i*2.0f) - offset, 0, 0 );
		glVertex3f( (i*2.0f+1.0f) - offset, 0, 0 );
	}

	glColor3f(0,1,0);
	glVertex3f( 0, -1, 0 );
	glVertex3f( 0, 1, 0 );
	
	for(unsigned i = 0; i < num; ++i)
	{
		glVertex3f( 0, (i*2.0f) - offset, 0 );
		glVertex3f( 0, (i*2.0f+1.0f) - offset, 0 );
	}
	
	glColor3f(1,0,0);
	glVertex3f( 0, 0, -1 );
	glVertex3f( 0, 0, 1 );
	
	for(unsigned i = 0; i < num; ++i)
	{
		glVertex3f( 0, 0, (i*2.0f) - offset );
		glVertex3f( 0, 0, (i*2.0f+1.0f) - offset );
	}


	glEnd();
}