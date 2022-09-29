#include <Camera.h>
#include <freeglut.h>

#include <SandboxGlobals.h>
#include <RenderObject.h>
#include <glm/gtc/type_ptr.hpp> // for the value_ptr function
#include <glm/gtc/matrix_transform.hpp> // for the lookAt function
#include <glm/gtx/rotate_vector.hpp> // so we can use glm::rotate on a vector

Camera::Camera()
:scrollSpeed( 10.0f )
,position( vec3(10,8,-40) )
,forward( vec3(0,0,1) )
,up( vec3(0,1,0) )
,width( Global::Window::Width )
,height( Global::Window::Height )
,debugMode( Global::DebugState )
{
	target = 0;
	debug_savedTarget = 0;
}

Camera::~Camera()
{
}

void Camera::SetPos(float x, float y, float z)
{
	position = vec3(x,y,z);
	ResizeWindow(width,height);
}

void Camera::MovePos(float dx, float dy, float dz)
{
	position += vec3(dx,dy,dz);
	ResizeWindow(width,height);
}

void Camera::ChangeMoveSpeed(float delta)
{
	scrollSpeed += delta;
	scrollSpeed = std::max<float>( scrollSpeed, Global::TINY );
	std::cerr<<"scrollSpeed: "<<scrollSpeed <<"\n";
}

void Camera::SetMoveSpeed(float speed)
{
	scrollSpeed = speed;
	scrollSpeed = std::max<float>( scrollSpeed, Global::TINY );
	std::cerr<<"scrollSpeed: "<<scrollSpeed <<"\n";
}

void Camera::MoveForward(float speed)
{
	position += forward * speed;
	ResizeWindow(width,height);
}

void Camera::MoveSideway(float speed)
{
    position += glm::cross(forward, up) * speed;
	ResizeWindow(width,height);
}

void Camera::SetForward(float x, float y, float z)
{
	forward = vec3(x,y,z);
	ResizeWindow(width,height);
}

void Camera::SetForward(vec3 fwd)
{
	forward = fwd;
	ResizeWindow(width,height);
}

void Camera::Rotate(float delta, vec3 axis)
{		
	forward = glm::rotate(forward, delta * 0.1f, axis);
	forward = glm::normalize( forward );
	ResizeWindow(width,height);
}
		
void Camera::Input(const bool* keys, float fps )
{
	if( debugMode )
	{
		float moveSpeed = 5 * fps * scrollSpeed;
		if( keys[ 'w' ] )
			MoveForward(moveSpeed);
		if( keys[ 's' ] )
			MoveForward(-moveSpeed);	
		if( keys[ 'd' ] )
			MoveSideway(moveSpeed);
		if( keys[ 'a' ] )
			MoveSideway(-moveSpeed);
		if( keys[ 'e' ] )
			MovePos(0,-moveSpeed,0);
		if( keys[ 'q' ] )
			MovePos(0,moveSpeed,0);

		if( keys[ '-' ] )
			ChangeMoveSpeed( -0.1f );
		if( keys[ '=' ] )
			ChangeMoveSpeed( 0.1f );

		if( Global::Mouse::ButtonStates & Button::RIGHT_HELD )
		{
			if( fabs( Global::Mouse::FrameDelta.x ) > Global::ZERO )
				Rotate( -Global::Mouse::FrameDelta.x*2, up );
			if( fabs( Global::Mouse::FrameDelta.y ) > Global::ZERO )
				Rotate( Global::Mouse::FrameDelta.y*2, glm::cross(forward,up) );
		}
	}
}
		
void Camera::ResizeWindow(int w, int h)
{	
	glutReshapeWindow ( w, h );
	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	vec3 targetToLookAt = position + forward;
	viewMatrix = glm::lookAt(position, targetToLookAt, up);
	projectionMatrix = glm::perspective(70.0f, (float)(w/h), 0.1f, 1000.0f);
	viewProjectionMatrix = projectionMatrix * viewMatrix;

	glMultMatrixf( glm::value_ptr( viewProjectionMatrix ) );
}

void Camera::LookAtTarget( RenderObject* t )
{
	forward = *t->pos - position;
	forward = glm::normalize(forward);
}

void Camera::SetTarget( RenderObject* t )
{
	target = t;
}

truth Camera::HasTarget()
{
	if( target )
		return 1;
	return 0;
}

void Camera::ClearTarget()
{
	target = 0;
}

void Camera::SyncWithTarget(float dt)
{
	if(debugMode || !target)
		return;

	const float MIN_FOLLOW_DISTANCE = 2.0f;//15.0f;
	const float CATCHUP_SPEED		= 0.01f;
	const float MIN_SPEED			= 0.01f;

	float distance_x = target->pos->x - position.x;
	float distance_y = target->pos->y - position.y;
	float distance_z = target->pos->z - position.z;

	float xsgn = static_cast<float>(distance_x > 0 ? 1 : -1 );
	float ysgn = static_cast<float>(distance_y > 0 ? 1 : -1 );
	float zsgn = static_cast<float>(distance_z > 0 ? 1 : -1 );

	distance_x = fabs(distance_x);
	distance_y = fabs(distance_y);
	distance_z = fabs(distance_z);

	if( distance_x > MIN_FOLLOW_DISTANCE )
		distance_x = std::max<float>( distance_x * CATCHUP_SPEED, MIN_SPEED );
	else
		distance_x = 0;
	
	if( distance_y > MIN_FOLLOW_DISTANCE )
		distance_y = std::max<float>( distance_y * CATCHUP_SPEED, MIN_SPEED );
	else
		distance_y = 0;
	
	if( distance_z > MIN_FOLLOW_DISTANCE )
		distance_z = std::max<float>( distance_z * CATCHUP_SPEED, MIN_SPEED );
	else
		distance_z = 0;

	position += vec3(xsgn * distance_x, ysgn * distance_y, zsgn * distance_z);
	LookAtTarget( target );
	ResizeWindow(width,height);
}
		
void Camera::SetDebugMode(truth state)
{
	if(!debugMode && target && state)
	{
		debug_savedTarget = target;
		target = 0;
	}
	if(debugMode && debug_savedTarget && !state)
	{
		target = debug_savedTarget;
		debug_savedTarget = 0;
	}

	debugMode = state;
}