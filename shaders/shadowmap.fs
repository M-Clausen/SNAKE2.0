#version 330 core

uniform vec2 near_far;
uniform vec4 light_pos;

in vec4 pos_from_gs;

void main()
{
	float dist = distance(pos_from_gs, light_pos);
	dist = dist / near_far.y; // divide bar far plane distance
	gl_FragDepth = dist;
}