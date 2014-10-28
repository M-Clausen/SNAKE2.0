#version 330 core

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
out vec4 vertexpos_pre_projection;
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
	
	vertex_color = vec4(0.0, 0.0, 0.0, 1.0);
	
	/*
	for(int i = 0; i < num_lights; i++)
	{
		if(renderlights[i] == 1)
		{
			// light is in game space([0;WINDOW_WIDTH], [0;WINDOW_HEIGHT], [-1;1])
			vec4 lightpos = vec4(lightpositions[i].xyz, 1.0);
			lightpos = window_to_unit * lightpos;
			lightpos.x -= 1; // xy - 1 because space goes from -1 to 1, not 0 to 2
			lightpos.y -= 1;
			lightpos = camera_translation + lightpos;

			float atten = 0.0;
			attenuation(atten, distance(lightpos, vertexpos), lightranges[i], 3, 6, 1.0);
			vec4 vlight = normalize(lightpos - vertexpos);
			float ndotl = max(0.0, dot(normal.xyz, vlight.xyz));

			vertex_color += vec4(atten * ndotl * lightcolors[i] * received_color.xyz, 1.0);
		}
	}
	*/
	
	vertex_color = received_color;

	vertexpos_pre_projection = vertexpos;

	vertexpos = camera_translation * vertexpos;
	vertexpos = camera_rotation * vertexpos;
	vertexpos.z -= 0;

	gl_Position = projection * vertexpos;
	vertexpos_post_projection = vertexpos;

	docolor = 1;
}
