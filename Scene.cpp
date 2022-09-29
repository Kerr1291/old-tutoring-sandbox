#include <Scene.h>

/////////////////////////////////////////////////////////////////////
// This is the scene file, where you will set up your test scenes
// for your application. Feel free to add any includes you need
// and anything else you deem important.
// You can get access to pretty much anything you need in the 
// External:: or Global:: namespaces, including other systems.
// This file will have some example code to show you how to do a 
// few things. Have fun!

#include <RenderManager.h>
#include <Camera.h>
#include <RenderObject.h>
#include <glm/gtc/matrix_transform.hpp> // for using glm::rotate on a matrix


/////////////////////////////////////////////////////////////////////
// basic default ctor

Scene::Scene()
{
}

/////////////////////////////////////////////////////////////////////
// dtor, free any data we allocated

Scene::~Scene()
{
}

/////////////////////////////////////////////////////////////////////
// Setup anything your scene needs

void Scene::Initialize()
{
	//we aren't going to be scaling any of our objects
	//so we can just have them all use the same scale variable
	scale = 1.0f;

	//make some vectors and matrices for our render objects to use
	for(unsigned i = 0; i < 10; ++i)
	{
		pos.push_back( vec3(0,0,0) );
		
		rot.push_back( mat4( vec4(1,0,0,0),
				vec4(0,1,0,0),
				vec4(0,0,1,0),
				vec4(0,0,0,1) ) );
	}

	//make bob, steve, the ground, and some walls
	External::renderer->CreateSphere( &pos[ 0 ], &rot[ 0 ], &scale, "bob");
	pos[0] = vec3(0,0.5f,0);
	RenderObject* r = External::renderer->CreateSphere( &pos[ 1 ], &rot[ 1 ], &scale, "steve");
	r->SetColor(1,1,0,1);
	pos[1] = vec3(1,0.5f,1);
	External::camera->SetTarget( r );

	RenderObject* ground = External::renderer->CreateBox( &pos[ 2 ], &rot[ 2 ], &scale, 40, 2, 40, "ground");
	pos[2] = vec3(0,-1,0);
	ground->SetColor(0,1,0,1);
	
	RenderObject* wall_1 = External::renderer->CreateBox( &pos[ 3 ], &rot[ 3 ], &scale, 40, 5, 2, "wall_1");
	pos[3] = vec3(0,0,21);
	wall_1->SetColor(0,0,1,1);
	
	RenderObject* wall_2 = External::renderer->CreateBox( &pos[ 4 ], &rot[ 4 ], &scale, 40, 5, 2, "wall_2");
	pos[4] = vec3(0,0,-21);
	wall_2->SetColor(0,0,1,1);
	
	RenderObject* wall_3 = External::renderer->CreateBox( &pos[ 5 ], &rot[ 5 ], &scale, 2, 5, 40, "wall_3");
	pos[5] = vec3(21,0,0);
	wall_3->SetColor(0,0,1,1);
	
	RenderObject* wall_4 = External::renderer->CreateBox( &pos[ 6 ], &rot[ 6 ], &scale, 2, 5, 40, "wall_4");
	pos[6] = vec3(-21,0,0);
	wall_4->SetColor(0,0,1,1);
}

/////////////////////////////////////////////////////////////////////
// Process any input you need to check for

void Scene::Input()
{
		
	if( Global::Keyboard::KeyPressed('8') )
	{
		External::camera->ClearTarget();
		External::renderer->RemoveRenderObject( External::renderer->FindRenderObject("steve") );
	}
	
	if( Global::Keyboard::KeyPressed('7') )
	{
		if( !External::renderer->FindRenderObject("steve") )
		{
			RenderObject* r = 0;
			r = External::renderer->CreateSphere( &pos[ 1 ], &rot[ 1 ], &scale, "steve");
			External::camera->SetTarget( r );
		}
	}

	if( Global::Keyboard::KeyHeld('f') )
	{
		pos[1].x += 0.1f;
	}
	if( Global::Keyboard::KeyHeld('h') )
	{
		pos[1].x -= 0.1f;
	}
	if( Global::Keyboard::KeyHeld('t') )
	{
		pos[1].z += 0.1f;
	}
	if( Global::Keyboard::KeyHeld('g') )
	{
		pos[1].z -= 0.1f;
	}

}

/////////////////////////////////////////////////////////////////////
// Do any kind of logic that should happen each frame

void Scene::Update(float dt)
{
	//An example of using the debug printf
	//Debug::printf(10,10,"Debug State Enabled (Press 0 to toggle)!");

	//rotate our spheres
	rot[0] = glm::rotate(rot[0], 1.0f, glm::vec3(0,0,1));
	rot[1] = glm::rotate(rot[1], 1.0f, glm::vec3(0,0,1));

	for(unsigned i = 0; i < pos.size(); ++i)
	{
		//if( pos[i].y > 0 )
		//	pos[i].y -= 0.1f;

		//
	}
}