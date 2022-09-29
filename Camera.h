#ifndef CAMERA_H
#define CAMERA_H

#include <SandboxGlobals.h>

struct RenderObject;

class Camera
{
	friend class RenderManager;
	public:

		Camera();
		~Camera();

		void Input(const bool* keys, float fps );
		void ResizeWindow(int w, int h);
		void LookAtTarget( RenderObject* t );

		void SetPos(float x, float y, float z);
		void MovePos(float dx, float dy, float dz);
		void ChangeMoveSpeed(float delta);
		void SetMoveSpeed(float speed);

		void MoveForward(float speed);
		void MoveSideway(float speed);
		void SetForward(float x, float y, float z);
		void SetForward(vec3 fwd);
		void Rotate(float delta, vec3 axis);

		unsigned int GetW() const { return width; }
		unsigned int GetH() const { return height; }
		float GetX() const { return position.x; }
		float GetY() const { return position.y; }
		float GetZ() const { return position.z; }
		vec3 GetPos() const { return position; }
		vec3 GetForward() const { return forward; }
		vec3 GetUp() const { return up; }

		void SetTarget( RenderObject* t );
		truth HasTarget();
		void ClearTarget();
		void SyncWithTarget(float dt = 0.016f);

		void SetDebugMode(truth state);
	
	private:
		float scrollSpeed;

		vec3 position;
		vec3 forward;
		vec3 up;

		mat4 viewMatrix;
		mat4 projectionMatrix;
		mat4 viewProjectionMatrix;

		const unsigned& width;
		const unsigned& height;

		RenderObject* target;

		RenderObject* debug_savedTarget;
		truth debugMode;
};


#endif