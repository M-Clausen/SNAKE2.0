#version 330 core

layout(location = 0) in vec3 vertexpos_vec3;
uniform mat4 translation;
uniform mat4 projection;

void main()
{
	vec4 vertexpos;
	vertexpos.xyz = vertexpos_vec3;
	vertexpos.w = 1.0;

	vertexpos = projection * translation * vertexpos;

	gl_Position = projection * translation * vertexpos_vec4;
}
