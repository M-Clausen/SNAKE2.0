#version 400 core

layout(location = 0) in vec3 vertexpos_vec3;
layout(location = 1) in vec3 normal_vec3;

uniform vec4 scale;
uniform vec4 window_to_unit;
uniform vec4 received_color;

uniform mat4 model_translation;
uniform mat4 model_rotation;
uniform mat4 projection;

out vec4 vertex_color;
out vec4 vertexpos_gamespace;
out vec4 vertexpos_modelspace;
out vec4 vertexpos_post_projection;

uniform mat4 camera_translation;
uniform mat4 camera_rotation;

flat out int docolor;
out vec4 normal;

void main()
{
	normal = vec4(normal_vec3.xyz, 1.0);
	vec4 vertexpos;
	vertexpos.xyz = vertexpos_vec3;
	vertexpos.w = 1.0;

	vertexpos_gamespace = vertexpos;
	vertexpos = scale * vertexpos;
	vertexpos = model_translation * vertexpos;
	vertexpos_modelspace = vertexpos;

	vertexpos = camera_translation * vertexpos;
	vertexpos = camera_rotation * vertexpos;

	gl_Position = projection * vertexpos;
	vertexpos_post_projection = vertexpos;

	vertex_color = received_color;
}
