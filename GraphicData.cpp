#include <GraphicData.h>
#include <RenderManager.h>
#include <glew.h>
#include <freeglut.h>



GraphicData::GraphicData()
:vertexArray(0)
,indexArray(0)
,numVerts(0)
,numIndices(0)
,vao(0)
,buffer(0)
,name("")
{
}

GraphicData::~GraphicData()
{
	delete[] vertexArray;
	delete[] indexArray;
}
	
void GraphicData::CreateRenderData(vec3* verts, unsigned _numVerts, unsigned* indices, unsigned _numIndices)
{
	if( vertexArray )
	{
		std::cerr<<"Error: Cannot generate render data a 2nd time.\n";
		return;
	}

	numVerts = _numVerts;
	numIndices = _numIndices;

	vertexArray = new vec3[ numVerts ];
	indexArray = new unsigned[ numIndices ];

	memcpy(vertexArray, verts, sizeof(vec3)*numVerts);
	memcpy(indexArray, indices, sizeof(unsigned)*numIndices);
	
	//make shader rendering data
 	vao = 0;
	buffer = 0;

	const unsigned vert = glGetAttribLocation(External::renderer->basicProgram, "vert");
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &buffer);

	glBindVertexArray(vao);

	glEnableVertexAttribArray( vert );

	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glVertexAttribPointer(vert, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBufferData(GL_ARRAY_BUFFER, 
			 sizeof(vec3)*numVerts,
			 vertexArray,
			 GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void GraphicData::CreateBox()
{
	CreateBox(1,1,1);

	/*
	if( vertexArray )
	{
		std::cerr<<"Error: Cannot generate render data a 2nd time.\n";
		return;
	}

	vec3 cube[] =
	{
		vec3(0.5, 0.5, 0.5), vec3(0.5, -0.5, 0.5), vec3(-0.5, -0.5, 0.5), vec3(-0.5, 0.5, 0.5),
		vec3(0.5, 0.5, -0.5), vec3(0.5, -0.5, -0.5), vec3(-0.5, -0.5, -0.5), vec3(-0.5, 0.5, -0.5)
	};

	unsigned cubei[] =
	{
		0,1,2, 2,3,0, //top
		4,5,6, 6,7,4, //bot
		2,6,7, 7,3,2, //left
		1,0,4, 5,1,4, //right
		0,3,7, 0,7,4, //back
		2,1,5, 2,5,6, //front
	};

	CreateRenderData(cube,8,cubei,36);
	*/
}

void GraphicData::CreateBox(float sx, float sy, float sz)
{
	if( vertexArray )
	{
		std::cerr<<"Error: Cannot generate render data a 2nd time.\n";
		return;
	}

	vec3 cube[] =
	{
		vec3(0.5*sx, 0.5*sy, 0.5*sz), vec3(0.5*sx, -0.5*sy, 0.5*sz), vec3(-0.5*sx, -0.5*sy, 0.5*sz), vec3(-0.5*sx, 0.5*sy, 0.5*sz),
		vec3(0.5*sx, 0.5*sy, -0.5*sz), vec3(0.5*sx, -0.5*sy, -0.5*sz), vec3(-0.5*sx, -0.5*sy, -0.5*sz), vec3(-0.5*sx, 0.5*sy, -0.5*sz)
	};

	unsigned cubei[] =
	{
		0,1,2, 2,3,0, //back
		6,5,4, 4,7,6, //front
		2,6,7, 7,3,2, //right
		1,0,4, 5,1,4, //left
		0,3,7, 0,7,4, //top
		2,1,5, 2,5,6, //bottom
	};

	CreateRenderData(cube,8,cubei,36);
}

void GraphicData::CreateSphere()
{
	//Taken from:
	// "Connecting sphere vertices openGL" on stack overflow
	//
	//Creates a sphere to be used with a GL_QUADS draw command
	std::vector< vec3 > verts;
	std::vector< unsigned > indices;

	const float RADIUS = 0.5f;
	const unsigned int RINGS = 9;
	const unsigned int SECTORS = 13;

	const float R = 1.0f / (float)( RINGS - 1 );
	const float S = 1.0f / (float)( SECTORS - 1 );
	int r, s;

	verts.resize( RINGS * SECTORS * 3 );
	std::vector< vec3 >::iterator v = verts.begin();

	for( r = 0; r < RINGS; ++r )
	{
		for( s = 0; s < SECTORS; ++s )
		{
			float y = sin( -Global::PI * 0.5f + Global::PI * r * R );
			float x = cos( 2.0f * Global::PI * s * S ) * sin( Global::PI * r * R );
			float z = sin( 2.0f * Global::PI * s * S ) * sin( Global::PI * r * R );

			v->x = x * RADIUS;
			v->y = y * RADIUS;
			v->z = z * RADIUS;

			v++;
		}
	}

	indices.resize( RINGS * SECTORS * 4 );
	std::vector< unsigned >::iterator iter = indices.begin();
	for( r = 0; r < RINGS; ++r )
	{
		for( s = 0; s < SECTORS; ++s )
		{
			*iter++ = r * SECTORS + s;
			*iter++ = r * SECTORS + (s+1);
			*iter++ = (r+1) * SECTORS + (s+1);
			*iter++ = (r+1) * SECTORS + s;
		}
	}

	name = "__sphere";
	CreateRenderData(&verts[0], verts.size(), &indices[0], indices.size());
}