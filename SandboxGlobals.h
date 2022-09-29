#ifndef SANDBOXGLOBALS_H
#define SANDBOXGLOBALS_H

//3rd party stuff (Math library)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // for the value_ptr function

//standard stuff
#include <iostream>
#include <math.h>
#include <vector>

//typedefs
typedef glm::vec2 vec2;
typedef glm::mat2 mat2;
typedef glm::vec3 vec3;
typedef glm::mat3 mat3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;
typedef unsigned char truth;


class Camera;
struct GraphicData;
class RenderManager;
class UserInterface;
class Sandbox;

namespace External
{
	//Variables that allow access to other places in the sandbox
	extern Camera* camera;
	extern RenderManager* renderer;
	extern Sandbox* sandbox;
}

namespace Button
{
	typedef unsigned char buttontype;
	const buttontype LEFT_DOWN = 1;
	const buttontype RIGHT_DOWN = 2;
	const buttontype MIDDLE_DOWN = 4;
	const buttontype LEFT_HELD = 8;
	const buttontype RIGHT_HELD = 16;
	const buttontype LEFT_UP = 32;
	const buttontype RIGHT_UP = 64;
	const buttontype MIDDLE_UP = 128;
}


namespace Debug
{
	namespace Internal
	{
		void PrintStrings();
	}

	//Warning: These are *slow*
    void printf(int x, int y, const char* string, ...);
    void printf3D(float x, float y, float z, const char* string, ...);
}


namespace Global
{
	//////////////////////////////////////////////////
	//global consts
	const float PI = 3.141592f;
	const float TINY = 0.001f;
	const float ZERO = 0.000001f;

	//////////////////////////////////////////////////
	///global variables
	namespace Window
	{
		extern unsigned Width;
		extern unsigned Height;
	}

	namespace Mouse
	{
		extern vec2 ScreenPos;
		extern vec2 FrameDelta;
		extern Button::buttontype ButtonStates;
	}

	namespace Keyboard
	{
		bool KeyPressed(unsigned char key);
		bool KeyReleased(unsigned char key);
		bool KeyHeld(unsigned char key);
		extern bool KeyState[256];
		extern bool KeyStateChange[256];
	}

	extern truth DebugState;

	//////////////////////////////////////////////////
	//global functions
	void SetDebugMode(truth state);

	//Get a 0-1 float (This is a pretty mediocre way to do it, but it works fine).
	inline float GetRandomFloat() { return (float)(rand()%10001)/10000.0f; }

}

#endif