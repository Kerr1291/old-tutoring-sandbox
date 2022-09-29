

#include <glew.h>
#include <wglew.h>
#include <freeglut.h> 
#include <iostream>
#include <memory.h>
#include <string>
#include <sstream>
#include <vector>

#include <Sandbox.h>
#include <Camera.h>
#include <Parser.h>
#include <SDL.h>

unsigned loadingCounter = 0;
unsigned RANDOM_SEED = 2400050;
float systemfps = 0.016f;

//bool KeyState[256];
//bool KeyStateChange[256];
using namespace Global::Keyboard;

std::string appFPS = "0";
std::string title = "Sandbox";

Sandbox* sandbox = 0;


void Update();
void Draw();
void KeyboardDown(unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);
void MouseFunc(int button, int state, int x, int y);
void MouseMotionFunc(int x, int y);
void ResizeWindow(int w, int h);
float calculateFPS();


int main(int argc, char* argv[])
{
	////////////////////////////////////////////////
	//get config file and setup window dimensions
	INIFile* config = new INIFile();
	if(!config->Load("./config.ini"))
	{
		std::cerr<<"Error loacating config.ini\n";
		delete config;
		return 0;
	}

	unsigned w = 800;
	unsigned h = 600;
	config->GetValue("ScreenWidth",w);
	config->GetValue("ScreenHeight",h);
	Global::Window::Width = w;
	Global::Window::Height = h;
	///////////////////////////////////////////////

	SDL_Init( SDL_INIT_TIMER ); 

	glutInit ( &argc, argv );
	
	glutInitDisplayMode ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE );

	srand( RANDOM_SEED );	
	
	glutInitWindowSize ( Global::Window::Width, Global::Window::Height );

	glutCreateWindow ( title.c_str() );
	glutIgnoreKeyRepeat(1);
	
	glewInit();

	if( !GLEW_VERSION_2_0 )
	{
		std::cout << "ERROR! OpenGL 2.0 Not supported.\n";
		std::cout << "VENDOR:    " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "VERSION:   " << glGetString(GL_VERSION) << std::endl;
		std::cout << "RENDERER:  " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "GLEW 2.0:  " << (GLEW_VERSION_2_0!=0) << std::endl;
		delete config;
		return 0;
	}
	

	sandbox = new Sandbox(config);
	sandbox->Initialize();

	memset( KeyState, 0, 256 );
	memset( KeyStateChange, 0, 256 );



	glutDisplayFunc ( Draw );
	glutIdleFunc ( Update );
	glutKeyboardFunc( KeyboardDown );
	glutKeyboardUpFunc( KeyboardUp );
	glutMouseFunc( MouseFunc );
	glutMotionFunc( MouseMotionFunc );
	glutPassiveMotionFunc( MouseMotionFunc );
	glutReshapeFunc( ResizeWindow );
	
	//Wait a bit for the fps to normalize (fixes a strange bug)
	while( loadingCounter < 100 )
	{
		++loadingCounter;
		 calculateFPS();
	}

	glutMainLoop();
	SDL_Quit();

	return 0;
}


void Update()
{ 
	sandbox->Input( KeyState, systemfps );
	sandbox->Update( systemfps );

	//calculate and show fps
	std::stringstream toStr;
	toStr << calculateFPS();
	appFPS = toStr.str();
	glutSetWindowTitle( (title + " - " + appFPS).c_str() );

	//Update mouse buttons
	Button::buttontype& buttonFlags = Global::Mouse::ButtonStates;

	//Clear Mouse Deltas
	Global::Mouse::FrameDelta = vec2(0,0);
	memset( KeyStateChange, 0, 256 );

	//See if left button is held
	if( buttonFlags & Button::LEFT_DOWN )
	{
		buttonFlags &= ~(Button::LEFT_DOWN);
		buttonFlags |= Button::LEFT_HELD;
	}
	else if( buttonFlags & Button::LEFT_UP )
	{
		buttonFlags &= ~(Button::LEFT_UP);
	}
	
	//See if right button is held
	if( buttonFlags & Button::RIGHT_DOWN )
	{
		buttonFlags &= ~(Button::RIGHT_DOWN);
		buttonFlags |= Button::RIGHT_HELD;
	}
	else if( buttonFlags & Button::RIGHT_UP )
	{
		buttonFlags &= ~(Button::RIGHT_UP);
	}
	
	//See clear down/up states for middle mouse
	if( buttonFlags & Button::MIDDLE_DOWN )
	{
		buttonFlags &= ~(Button::MIDDLE_DOWN);
	}
	else if( buttonFlags & Button::MIDDLE_UP )
	{
		buttonFlags &= ~(Button::MIDDLE_UP);
	}
}

void Draw()
{
	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	sandbox->Draw();
	glColor4f(1.0f,1.0f,1.0f,1.0f);

	glutSwapBuffers();
	glutPostRedisplay();
}

void KeyboardDown(unsigned char key, int x, int y)
{
	KeyStateChange[key] = !KeyState[key];
	KeyState[key] = true;
}

void KeyboardUp(unsigned char key, int x, int y)
{
	KeyStateChange[key] = KeyState[key];
	KeyState[key] = false;
}


void MouseFunc(int button, int state, int x, int y)
{
	Button::buttontype& buttonFlags = Global::Mouse::ButtonStates;

	if( button == GLUT_LEFT_BUTTON )
	{
		if(state == GLUT_DOWN)
		{
			buttonFlags |= Button::LEFT_DOWN;
		}
		else if(state == GLUT_UP)
		{
			buttonFlags |= Button::LEFT_UP;
			buttonFlags &= ~(Button::LEFT_HELD);
		}
	}
	
	if( button == GLUT_MIDDLE_BUTTON )
	{
		if(state == GLUT_DOWN)
		{
			buttonFlags |= Button::MIDDLE_DOWN;
		}
		else if(state == GLUT_UP)
		{
			buttonFlags |= Button::MIDDLE_UP;
		}
	}
	
	if( button == GLUT_RIGHT_BUTTON )
	{
		if(state == GLUT_DOWN)
		{
			buttonFlags |= Button::RIGHT_DOWN;
		}
		else if(state == GLUT_UP)
		{
			buttonFlags |= Button::RIGHT_UP;
			buttonFlags &= ~(Button::RIGHT_HELD);
		}
	}
}

void MouseMotionFunc(int x, int y)
{
	vec2 previous = Global::Mouse::ScreenPos;
	Global::Mouse::ScreenPos = vec2(x,y);
	Global::Mouse::FrameDelta = Global::Mouse::ScreenPos - previous;
	Global::Mouse::FrameDelta.y *= -1; //let's make up be positive
}

void ResizeWindow(int w, int h)
{
	Global::Window::Width = w;
	Global::Window::Height = h;
	External::camera->ResizeWindow( w, h );
}

//-------------------------------------------------------------------------
// Calculates the frames per second
//-------------------------------------------------------------------------
float calculateFPS()
{
	static int frameCount = 0;
	static int currentTime = 0;
	static int previousTime = 0;
	static float fps = 0;
    //  Increase frame count
    frameCount++;

    //  Get the number of milliseconds since last time calculation
    currentTime = SDL_GetTicks();

    //  Calculate time passed
    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);

		systemfps = (timeInterval / 1000.0f) / frameCount;

        //  Set time
        previousTime = currentTime;

        //  Reset frame count
        frameCount = 0;
    }

	return fps;
}