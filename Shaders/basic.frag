#version 330
in vec4 color;

void main() 
{	
	gl_FragData[0] = color;
}