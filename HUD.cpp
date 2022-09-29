#include <HUD.h>

/////////////////////////////////////////////////////////////


std::vector<vec2> HUD::MakeBox(unsigned w, unsigned h)
{
	std::vector<vec2> box;
	box.push_back( vec2(0,0) );
	box.push_back( vec2(0,h) );
	box.push_back( vec2(w,h) );
	box.push_back( vec2(w,0) );	
	return box;
}