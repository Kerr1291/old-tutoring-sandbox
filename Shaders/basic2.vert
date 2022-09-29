in vec3 vert;

uniform float scale;
uniform mat4 rotation;
uniform vec3 pos;
uniform vec4 object_color;
uniform mat4 viewProjectionMatrix;

varying out vec4 color;

void main(void) 
{ 
	color = object_color; 
	
	vec3 v = vert * scale;
	vec4 V = rotation * vec4(v,1);
		 V = V + vec4(pos,0);
gl_Position = viewProjectionMatrix * V;
}