#include <RenderObject.h>
#include <memory.h>

#include <GraphicData.h>

RenderObject::RenderObject(GraphicData* g)
:scale(0)
,rot(0)
,pos(0)
,visible(1)
,solid(1)
,color(vec4(1,0,0,1)) // Default render color (currently red), feel free to change this
,gfx(g)
,name("")
{	
	if( !g )
		throw 0;
}

RenderObject::~RenderObject()
{}

void RenderObject::SetColor(vec4 rgba)
{
	color = rgba;
}

void RenderObject::SetColor(float r, float g, float b, float a)
{ 
	SetColor(vec4(r,g,b,a));
}

