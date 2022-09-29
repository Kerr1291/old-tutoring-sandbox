#include <RenderManager.h>


#include <glew.h>
#include <freeglut.h>

#include <memory.h>
#include <fstream>
#include <sstream>

#include <GraphicData.h>
#include <RenderObject.h>
#include <Camera.h>

#include <stdio.h>

//Helper function for this file only
void PrintGLError(const std::string& label)
{
	std::cerr<<"checking for errors at: "<<label <<" ";
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
	   fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
	else
		std::cerr<<" ...no errors found\n";
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Public Methods // These are the functions you will use
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


//Simple ctor. Sets up the extenal system pointer, allocates default space
// for our graphics data and creates shaders and render buffers.
RenderManager::RenderManager()
:maxStorage(0)
,currentStorage(0)
,graphics(0)
,nearby_scalar(0.1f)
,vertexShader(0)
,fragmentShader(0)
,basicProgram(0)
,normalRenderbuffer(0)
,normalTexture(0)
,normalFramebuffer(0)
,drawWireframes(1)
{
	External::renderer = this;

	AllocateGraphicSpace( DEFAULT_RESERVED_GRAPHIC_SPACE );

	CreateShaderObjects();

	glEnable(GL_TEXTURE_2D); // Enable texturing so we can bind our frame buffer texture  
	glEnable(GL_DEPTH_TEST); // Enable depth testing  

	//Have it cull faces that are back faces
	//Set back faces to use the clockwise winding order
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	
	//first pass will render normal things to this texture, and glow things to the glow texture
	CreateTexture(normalTexture);
	CreateRenderbuffer(normalRenderbuffer, normalFramebuffer, normalTexture);	
}



//Free up any resources we use
RenderManager::~RenderManager()
{
	delete[] graphics;

	for( unsigned i = 0; i < renderObjects.size(); ++i )
		delete renderObjects[i];

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteProgram(basicProgram);
	
	glDeleteRenderbuffers(1, &normalRenderbuffer);
	glDeleteTextures(1, &normalTexture);
	glDeleteFramebuffers(1, &normalFramebuffer);
}

//TODO: set this up for use with 3D
void RenderManager::SetViewCullingScalar(float cullingScalar)
{
	if( cullingScalar > 0 )
		nearby_scalar = cullingScalar;
}

//Note: You shouldn't need to change this unless you decide
//		to alter the shaders used by the system in some way.
truth RenderManager::CreateShaderObjects()
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	basicProgram = glCreateProgram();

	if(!CreateShader("./Shaders/basic.vert", vertexShader))
		return 0;

	if(!CreateShader("./Shaders/basic.frag", fragmentShader))
		return 0;

	if(!LinkShaders(basicProgram, vertexShader, fragmentShader))
		return 0;

	return 1;
}

// Call once per frame, draws all the render objects in the system
void RenderManager::Draw()
{
	Render(renderObjects);
}
		
// Just used to turn wireframes on and off
void RenderManager::SetWireframes(truth enabled)
{
	drawWireframes = enabled;
}

// Creates a 1x1x1 cube.
// Must pass a pointer to:
//  vec3 for position
//  mat4 for rotation
//  float for scaling
// Optional:
//  a custom name to use to look up this object later.
RenderObject* RenderManager::CreateCube(vec3* pos, mat4* rot, float* scale, const std::string& name)
{
	GraphicData* g = 0;
	if( HasGfx("__cube") )
	{
		g = graphics[ loadedGraphics["__cube"] ];
	}
	else
	{
		g = new GraphicData();
		g->CreateBox();
		AddGfx(g, "__cube");
	}

	return MakeRenderObject(g, pos, rot, scale, name);
}


// Creates a box of specified dimensions.
// Must pass a pointer to:
//  vec3 for position
//  mat4 for rotation
//  float for scaling
// The values sx, sy, and sz define the box dimensions.
// Optional:
//  a custom name to use to look up this object later.
RenderObject* RenderManager::CreateBox(vec3* pos, mat4* rot, float* scale, float sx, float sy, float sz, const std::string& name)
{
	GraphicData* g = 0;
	std::stringstream tostr;
	tostr <<"_" << sx <<"_" << sy <<"_" << sz;
	std::string boxname = "__box" + tostr.str();

	if( HasGfx(boxname) )
	{
		g = graphics[ loadedGraphics[boxname] ];
	}
	else
	{
		g = new GraphicData();
		g->CreateBox(sx,sy,sz);
		AddGfx(g, boxname);
	}

	return MakeRenderObject(g, pos, rot, scale, name);
}
	

// Creates a 1x1x1 sphere.
// Must pass a pointer to:
//  vec3 for position
//  mat4 for rotation
//  float for scaling
// Optional:
//  a custom name to use to look up this object later.	
RenderObject* RenderManager::CreateSphere(vec3* pos, mat4* rot, float* scale, const std::string& name)
{
	GraphicData* g = 0;
	if( HasGfx("__sphere") )
	{
		g = graphics[ loadedGraphics["__sphere"] ];
	}
	else
	{
		g = new GraphicData();
		g->CreateSphere();
		AddGfx(g, "__sphere");
	}

	return MakeRenderObject(g, pos, rot, scale, name);
}
	
// Creates a render object using the custom mesh data you provide
// Must pass in:
// An array of vertices, and the number
// An array of indices, and the number
// The name of the custom mesh
// Also pointers to:
//  vec3 for position
//  mat4 for rotation
//  float for scaling
// Optional:
//  a custom name to use to look up this object later.		
RenderObject* RenderManager::CreateRenderObject(vec3* verts, unsigned _numVerts, unsigned* indices, unsigned _numIndices, const std::string& meshname,
				        vec3* pos, mat4* rot, float* scale, const std::string& name)
{
	GraphicData* g = 0;
	if( HasGfx(meshname) )
	{
		g = graphics[ loadedGraphics[meshname] ];
	}
	else
	{
		g = new GraphicData();
		g->CreateRenderData(verts, _numVerts, indices, _numIndices);
		AddGfx(g, meshname);
	}

	return MakeRenderObject(g, pos, rot, scale, name);
}


// Search the render manager for a RenderObject with the name given.
// If the object is found a pointer to that object is returned.
// If the object is not found then 0 is returned.
RenderObject* RenderManager::FindRenderObject(const std::string& nameToFind)
{
	if( nameToFind.empty() )
		return 0;

	for( unsigned i = 0; i < renderObjects.size(); ++i )
	{
		if( nameToFind == renderObjects[i]->name )
			return renderObjects[i];
	}

	return 0;
}

// Remove the given render object from the system (if it exists)
// Pass in a pointer to the object to be removed.
// You may use FindRenderObject to retrieve this pointer if needed.
// 1 (or true) is returned if successful, otherwise 0 (false) is returned.
truth RenderManager::RemoveRenderObject( RenderObject* objectToRemove )
{
	if( !objectToRemove )
		return 0;

	for( unsigned i = 0; i < renderObjects.size(); ++i )
	{
		if( objectToRemove == renderObjects[i] )
		{
			RemoveRenderObject( i );
			return 1;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Private Methods // Understanding the functions below is not needed!
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////


RenderObject* RenderManager::MakeRenderObject( GraphicData* g, vec3* pos, mat4* rot, float* scale, const std::string& name )
{
	RenderObject* r = new RenderObject(g);
	r->scale = scale;
	r->pos = pos;
	r->rot = rot;
	
	if(!name.empty())
	{
		r->name = name;

		if( FindRenderObject(name) )
		{
			std::cerr<<"Warning: Render object with name " <<name <<" already exists. \nWe will add this duplicate anyway but it will interfere if you try to do a find for this name in the future.\n";
		}
	}

	renderObjects.push_back( r );
	return r;
}


void RenderManager::Render(std::vector<RenderObject*>& objects)
{		
	glBindFramebuffer(GL_FRAMEBUFFER, normalFramebuffer);
	glClearColor(0.0,0.0,0.0,1.0);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram( basicProgram );

	unsigned scale = glGetUniformLocation(basicProgram, "scale");
	unsigned rotation = glGetUniformLocation(basicProgram, "rotation");
	unsigned pos = glGetUniformLocation(basicProgram, "pos");
	unsigned color = glGetUniformLocation(basicProgram, "object_color");
	unsigned viewProj = glGetUniformLocation(basicProgram, "viewProjectionMatrix");
	
	//float projMat[16];
	float viewProjMat[16];
	memcpy( viewProjMat, glm::value_ptr(External::camera->viewProjectionMatrix), sizeof(viewProjMat) );
	//glGetFloatv(GL_PROJECTION_MATRIX, projMat);
	
	glUniformMatrix4fv(viewProj, 1, 0, viewProjMat);
	
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_COLOR, GL_ONE);
	
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	for(unsigned i = 0; i < objects.size(); ++i)
	{

		if( !objects[i]->visible )
			continue;
		
		//TODO: fix the culling for 3D objects
		//if( !IsNearCamera( objects[i]->transform->loc, 0.5f ) )
		//	continue;

		if(!objects[i]->gfx || !objects[i]->pos || !objects[i]->rot || !objects[i]->scale)
			continue;

		GraphicData* gfx = objects[i]->gfx;

		glUniform1f(scale, *objects[i]->scale);
		glUniformMatrix4fv(rotation, 1, 0, glm::value_ptr(*objects[i]->rot));
		glUniform3fv(pos, 1, glm::value_ptr(*objects[i]->pos));
		
		unsigned numVerts = gfx->numVerts;
		unsigned numIndices = gfx->numIndices;

		glBindVertexArray(gfx->vao);

		if( objects[i]->solid )
		{
			glUniform4fv(color, 1, glm::value_ptr(objects[i]->color));

			//Crappy way to do this, but it works for now
			if( gfx->name == "__sphere" )
				glDrawElements(GL_QUADS, gfx->numIndices, GL_UNSIGNED_INT, gfx->indexArray);
			else
				glDrawElements(GL_TRIANGLES, gfx->numIndices, GL_UNSIGNED_INT, gfx->indexArray);
			
		}

		if( drawWireframes && Global::DebugState )
		{
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glUniform4fv(color, 1, glm::value_ptr(vec4(0,1,1,1)));
			
			//Crappy way to do this, but it works for now
			if( gfx->name == "__sphere" )
				glDrawElements(GL_QUADS, gfx->numIndices, GL_UNSIGNED_INT, gfx->indexArray);
			else
				glDrawElements(GL_TRIANGLES, gfx->numIndices, GL_UNSIGNED_INT, gfx->indexArray);
			
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
		
	}
	
	//glDisable(GL_BLEND);
	glUseProgram( 0 );

//#if 0
	//static unsigned iter = 0;
	//iter++;
	//std::stringstream tostr;
	//tostr << iter;
	//PrintGLError("draw checkA "+tostr.str()+" ");
	glPushMatrix();
	glLoadIdentity();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, normalTexture); // Bind our frame buffer texture 
 
	//glUseProgram( glowProgram );
	
	//unsigned renderedTexture = glGetUniformLocation( glowProgram, "renderedTexture" );
	//glUniform1i(renderedTexture, normalTexture);
	//glBindVertexArray( screenQuadArray );
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(-1,-1);
	glTexCoord2f(0,1); glVertex2f(-1, 1);
	glTexCoord2f(1,1); glVertex2f( 1, 1);
	glTexCoord2f(1,0); glVertex2f( 1,-1);
	glEnd();
	//glUseProgram( 0 );
	glBindTexture(GL_TEXTURE_2D, 0 );
	glPopMatrix();
//#endif
}

#if 0
GraphicData* RenderManager::LoadGfx(std::string filename)
{
	if( HasGfx( filename ) )
		return graphics[ loadedGraphics[ filename ] ];

	std::fstream fs(filename.c_str(), std::ios::in);

	if( !fs.is_open() )
	{
		std::cerr<<"FILE NOT FOUND: "<<filename <<"\n";
		return 0;
	}

	GraphicData* data = new GraphicData();
	unsigned numSets = 0;
	unsigned readSets = 0;
	unsigned totalPoints = 0;
	unsigned readPoints = 0;
	fs >> numSets;

	while( fs.good() )
	{
		std::string currentSetName;
		unsigned numPoints = 0;
		std::vector< vec2 > setPoints;

		fs >> currentSetName;
		fs >> numPoints;
		totalPoints+=numPoints;

		for(unsigned i = 0; i < numPoints; ++i)
		{
			vec2 point;
			fs >> point.x;
			fs >> point.y;
			setPoints.push_back( point );
			readPoints++;
		}
		readSets++;
		data->gfxPoints.push_back( setPoints );
	}

	if( readSets != numSets )
	{
		std::cerr<<"Error gfx Load failed: numSets != amount of point sets read from file!\n";
		delete data;
		return 0;
	}

	if( readPoints != totalPoints )
	{
		std::cerr<<"Error gfx Load failed: totalPoints != amount of points read from file!\n";
		delete data;
		return 0;
	}

	data->CreateRenderData();
		
	if( !AddGfx( data, filename ) )
	{
		std::cerr<<"Error: failed to add graphic to system!\n";
		delete data;
		return 0;
	}

	return data;
}
#endif


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//private data
//////////////////////////////////////////////////////////////////////////////////////

truth RenderManager::IsNearCamera(const vec2& pos, const float area_size)
{
	return true;
	//TODO: rewrite for 3D
/*
	Camera* c = External::camera;
	float l = (c->GetX() - c->GetW()*nearby_scalar*area_size);
	float r = (c->GetX() + c->GetW()*nearby_scalar*area_size);
	float t = (c->GetY() + c->GetH()*nearby_scalar*area_size);
	float b = (c->GetY() - c->GetH()*nearby_scalar*area_size);
	if( pos.x < l )
		return false;
	if( pos.x > r )
		return false;
	if( pos.y < b )
		return false;
	if( pos.y > t )
		return false;
	return true;
*/
}

void RenderManager::CreateTexture(unsigned &texture)
{
	if( texture != 0 )
		glDeleteTextures(1, &texture);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, Global::Window::Width, Global::Window::Height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	//original
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, Global::Window::Width, Global::Window::Height, 0,GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, 0);
	
	//first try
	//glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, Global::Window::Width, Global::Window::Height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderManager::CreateRenderbuffer(unsigned &buffer, unsigned &framebuffer, unsigned texture0, unsigned texture1)
{
	if( framebuffer != 0 )
		glDeleteFramebuffers(1, &framebuffer);
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	if( buffer != 0 )
		glDeleteRenderbuffers(1, &buffer);
	glGenRenderbuffers(1, &buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Global::Window::Width, Global::Window::Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer);
	
	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture0, 0);
	if( texture1 != 0 )
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texture1, 0);
	
	GLenum DrawBuffers[4] = {0,0,0,0};
	unsigned numBuffers = 0;

	if(texture1 != 0)
	{
		DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
		DrawBuffers[1] = GL_COLOR_ATTACHMENT1;
		numBuffers = 2;
	}
	else
	{
		DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
		numBuffers = 1;
	}

	glDrawBuffers(numBuffers, DrawBuffers);
 
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr<<"Framebuffer status returned: " <<glCheckFramebufferStatus(GL_FRAMEBUFFER) <<"\n";
		std::cerr<<"Error generating framebuffer!\n";
	}
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

truth RenderManager::CreateShader(const std::string& filename, unsigned handle)
{	
	char* shader_data = 0;
	if( !LoadShaderFile(filename, &shader_data) )
		return 0;

	if(! CompileShader(handle, shader_data) )
		return 0;
	
	delete[] shader_data;
	return 1;
}

		
truth RenderManager::LoadShaderFile(const std::string& filename, char** out_shaderStrings)
{
	if(*out_shaderStrings)
	{
		std::cerr<<"Error: output destination must be null.\n";
		return 0;
	}
	
	std::string file_data;
	FILE* fp = 0;

	fp = fopen( filename.c_str(), "r" );

	if( !fp )
	{
		std::cerr<<"Error: shader file "<<filename <<" not found.\n";
		return 0;
	}
	
	while( !feof(fp) )
	{
		char c = fgetc(fp);
		file_data.push_back(c); 
	}

	const unsigned data_size = file_data.size();
	*out_shaderStrings = new char[ data_size ];
	memset( *out_shaderStrings, 0, data_size );
	memcpy( *out_shaderStrings, file_data.c_str(), sizeof(char)*data_size );
	(*out_shaderStrings)[data_size-1] = '\0';

	return 1;
}

truth RenderManager::CompileShader(unsigned shaderHandle, char* shader_data)
{
	glShaderSource(shaderHandle, 1, const_cast<const GLchar**>(&shader_data), 0);
	glCompileShader(shaderHandle);
	int status = 0;
	int logLength = 1;
	glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
	if( logLength > 1 ) // 0 log length = no errors on ATI, 1 = no errors on NVIDIA
	{
		std::cerr<<"Compile Error:\n";
		char error_log[1024];
		glGetShaderInfoLog(shaderHandle, 1024, 0, error_log );
		std::cerr<<"\n"<<error_log;

		if(status != GL_TRUE)
		{
			std::cerr<<"Compile Failed.\n";
			delete[] shader_data;
			return 0;
		}
	}
	return 1;
}
		
truth RenderManager::LinkShaders(unsigned program, unsigned vertex, unsigned fragment)
{
	if( vertex != 0 )
		glAttachShader( program, vertex );
	glAttachShader( program, fragment );

	glLinkProgram( program );
	
	int status = 0;
	int logLength = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if( logLength > 1 )
	{
		std::cerr<<"Linker Error:\n";
		char error_log[1024];
		glGetProgramInfoLog(program, 1024, 0, error_log );
		std::cerr<<"\n"<<error_log;
		if(status != GL_TRUE)
		{
			std::cerr<<"Link Failed.\n";
			return 0;
		}
	}
	return 1;
}


truth RenderManager::AddGfx( GraphicData* g, std::string filename )
{
	if( !g )
	{
		std::cerr<<"error cannot add null graphic object\n";
		return 0;
	}

	if( maxStorage == currentStorage )
	{
		AllocateGraphicSpace( maxStorage * 2 );
	}

	graphics[ currentStorage ] = g;
	loadedGraphics[ filename ] = currentStorage;

	currentStorage++;
	return 1;
}
		
truth RenderManager::HasGfx( std::string filename )
{
	if( loadedGraphics.find( filename ) != loadedGraphics.end() )
	{
		return 1;
	}
	return 0;
}

void RenderManager::AllocateGraphicSpace( unsigned size )
{
	if( size <= maxStorage )
	{
		std::cerr<<"ERROR: cannot allocate a smaller buffer than the current max size!\n";
		return;
	}

	if( graphics )
	{
		GraphicData** old_graphics = new GraphicData*[maxStorage];
		memcpy( old_graphics, graphics, sizeof( GraphicData* ) * maxStorage  );
		delete[] graphics;
		graphics = new GraphicData*[size];
		memset( graphics, 0, sizeof( GraphicData* ) * size );
		memcpy( graphics, old_graphics, sizeof( GraphicData* ) * maxStorage  );
		delete[] old_graphics;
	}
	else
	{
		graphics = new GraphicData*[size];
		memset( graphics, 0, sizeof( GraphicData* ) * size );
		currentStorage = 0;
	}
		
	maxStorage = size;
}
		


void RenderManager::RemoveRenderObject(unsigned i)
{
	if(i >= renderObjects.size())
		return;

	delete renderObjects[i]; 
	renderObjects[i] = 0;
	renderObjects.erase( renderObjects.begin() +  i );
}
