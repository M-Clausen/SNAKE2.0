#version 330 core

layout(location = 0) out vec4 color0;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;

uniform vec2 near_far;
uniform vec4 light_pos;
uniform int num_buffer;

in vec4 vertexpos_post_projection;


void main()
{
	float dist = vertexpos_post_projection.z;
	// dist = dist / near_far.y; // divide by far plane distance
	
	if(num_buffer == 0)
	{
		color0 = vec4(dist, dist, dist, 1.0);
	}
	else if(num_buffer == 1)
	{
		color1 = vec4(dist, dist, dist, 1.0);
	}
	else if(num_buffer == 2)
	{
		color2 = vec4(dist, dist, dist, 1.0);
	}
	else if(num_buffer == 3)
	{
		color3 = vec4(dist, dist, dist, 1.0);
	}
}