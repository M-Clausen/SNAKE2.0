#version 330 core

layout(location = 0) in vec3 vertexpos_vec3;

uniform vec4 scale;
uniform vec4 window_to_unit;

uniform mat4 model_translation;
uniform mat4 model_rotation;
uniform mat4 projection;

out vec4 vertexpos_gamespace;
out vec4 vertexpos_modelspace;
out vec4 vertexpos_post_projection;

uniform mat4 camera_translation;
uniform mat4 camera_rotation;

void main()
{
	vec4 vertexpos;
	vertexpos.xyz = vertexpos_vec3;
	vertexpos.w = 1.0;

	vertexpos_gamespace = vertexpos;
	vertexpos = scale * vertexpos;
	vertexpos = model_translation * vertexpos;
	vertexpos_modelspace = vertexpos;

	vertexpos = camera_translation * vertexpos;
	vertexpos = camera_rotation * vertexpos;

	gl_Position = vertexpos;

	// gl_Position = projection * vertexpos;
	vertexpos_post_projection = vertexpos;
}