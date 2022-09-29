#include <SandboxGlobals.h>

#include <Camera.h>
#include <Sandbox.h>

#include <freeglut.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>

namespace External
{
	//Variables that allow access to objects created other places in the game
	Camera* camera = 0;
	RenderManager* renderer = 0;
	Sandbox* sandbox = 0;
}

namespace Debug
{
	struct DebugString
	{
		vec3 pos;
		std::string data;
	};
	std::vector<DebugString> debugStrings;
	std::vector<DebugString> debugStrings3D;

	namespace Internal
	{
		void PrintStrings()
		{
			glColor3f(0.9f, 0.6f, 0.6f);
			if( !debugStrings.empty() )
			{
				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();
				int w = glutGet(GLUT_WINDOW_WIDTH);
				int h = glutGet(GLUT_WINDOW_HEIGHT);
				gluOrtho2D(0, w, h, 0);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();

				for(unsigned i = 0; i < debugStrings.size(); ++i)
				{
					glRasterPos2i(debugStrings[i].pos.x, debugStrings[i].pos.y);
					std::string::iterator iter = debugStrings[i].data.begin();
					for(; iter != debugStrings[i].data.end(); ++iter)
					{
						glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
					}

				}
				debugStrings.clear();

				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
			}
			
			if( !debugStrings3D.empty() )
			{
				for(unsigned i = 0; i < debugStrings3D.size(); ++i)
				{
					glRasterPos3f(debugStrings3D[i].pos.x, debugStrings3D[i].pos.y, debugStrings3D[i].pos.z);
					std::string::iterator iter = debugStrings3D[i].data.begin();
					for(; iter != debugStrings3D[i].data.end(); ++iter)
					{
						glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *iter);
					}

				}
				debugStrings3D.clear();
			}
		}
	}

    void printf(int x, int y, const char* string, ...)
	{
		if(!Global::DebugState)
			return;

		char buffer[256];

		va_list arg;
		va_start(arg, string);
		vsprintf_s(buffer, string, arg);
		va_end(arg);

		std::string out_buffer( buffer );
		DebugString dstr = { vec3(x,y,0), out_buffer };
		debugStrings.push_back( dstr );
	}
	
    void printf3D(float x, float y, float z, const char* string, ...)
	{
		if(!Global::DebugState)
			return;

		char buffer[256];

		va_list arg;
		va_start(arg, string);
		vsprintf_s(buffer, string, arg);
		va_end(arg);

		std::string out_buffer( buffer );
		DebugString dstr = { vec3(x,y,z), out_buffer };
		debugStrings3D.push_back( dstr );
	}
}

namespace Global
{
	///global variables
	namespace Window
	{
		unsigned Width = 800;
		unsigned Height = 600;
	}

	namespace Mouse
	{
		vec2 ScreenPos(0,0);
		vec2 FrameDelta(0,0);
		Button::buttontype ButtonStates = 0;
	}

	namespace Keyboard
	{
		bool KeyPressed(unsigned char key)
		{
			return KeyStateChange[key] & KeyState[key];
		}

		bool KeyReleased(unsigned char key)
		{
			return KeyStateChange[key] & !KeyState[key];
		}

		bool KeyHeld(unsigned char key)
		{
			return KeyState[key];
		}

		bool KeyState[256];
		bool KeyStateChange[256];
	}

	truth DebugState = 1;

	void SetDebugMode(truth state)
	{
		DebugState = state;
		External::camera->SetDebugMode(state);
	}



}