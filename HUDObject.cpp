#include <HUDObject.h>

#include <Sandbox.h>

#include <freeglut.h>

HUDObject::HUDObject(Sandbox* _sandbox)
:sandbox(_sandbox)
,hidden(0)
,solid(1)
,color(vec4(0,1,0,1))
,textColor(vec4(0,0,1,1))
,bottomLeft(vec2(0,0))
,size(vec2(10,10))
,brightness(1.0f)
{
}

HUDObject::~HUDObject()
{
}

void HUDObject::Update()
{
	truth mouseOver = MouseOver();
}

void HUDObject::Draw()
{
	if( hidden )
		return;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if( !vertices.empty() )
	{

	if( !text.empty() )
	{
		float offset = ( vertices.empty() ? 0.0f : 5.0f );

		//glColor3f(0.9f, 0.6f, 0.6f);
		glColor3f(textColor.r, textColor.g, textColor.b);

		for(unsigned i = 0; i < text.size(); ++i)
		{
			glRasterPos2i(bottomLeft.x + offset, i*12 + bottomLeft.y + 14);
			
			std::string::iterator sIter = text[i].begin();
			for (; sIter != text[i].end(); ++sIter)
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *sIter);
		}
	}

	if( solid )
	{
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f * color.r * brightness, 0.5f * color.g * brightness, 0.5f * color.b * brightness, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (int i = vertices.size()-1; i >= 0 ; --i)
		{
			glVertex2f(bottomLeft.x + size.x*(vertices[i].x), bottomLeft.y + size.y*(vertices[i].y));
		}
		glEnd();
		glDisable(GL_BLEND);
	}

	glColor4f(color.r * brightness, color.g * brightness, color.b * brightness, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (unsigned i = 0; i < vertices.size(); ++i)
	{
		glVertex2f(bottomLeft.x + size.x*(vertices[i].x), bottomLeft.y + size.y*(vertices[i].y));
	}
	glEnd();

	}

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

truth HUDObject::MouseOver()
{
	if( Global::Mouse::ScreenPos.x < bottomLeft.x )
		return 0;
	if( Global::Mouse::ScreenPos.y < bottomLeft.y )
		return 0;
	if( Global::Mouse::ScreenPos.x > bottomLeft.x+size.x )
		return 0;
	if( Global::Mouse::ScreenPos.y > bottomLeft.y+size.y )
		return 0;
	return 1;
}