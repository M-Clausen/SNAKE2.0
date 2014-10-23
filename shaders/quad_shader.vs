#version 330 core

layout(location = 0) in vec3 vertexpos_vec3;
uniform vec3 lightpositions[20];
uniform vec3 lightcolors[20];
uniform float lightranges[20];
uniform int renderlights[20];
uniform int num_lights;

uniform vec4 scale;
uniform vec4 window_to_unit;
uniform vec4 received_color;

uniform mat4 translation;
uniform mat4 projection;

out vec4 vertex_color;

vec4 normal = vec4(0.0, 0.0, -1.0, 1.0);
vec4 camera_translation = vec4(0.0f, 0.0f, 0.0f, 0.0f);

void attenuation(out float atten, in float distance, in float range, in float a, in float b, in float c)
{
    atten = 1.0 / (a * distance * distance + b * distance + c) * range;
    if(atten < 0.1) { atten = 0.0; }
}

void main()
{
	vec4 vertexpos;
	vertexpos.xyz = vertexpos_vec3;
	vertexpos.w = 1.0;
	vertexpos = scale * vertexpos;

	vertexpos = translation * vertexpos;
	vertexpos = camera_translation + vertexpos;
	vec4 original_vertexpos = vertexpos;
	vertexpos = projection * vertexpos;

	/*
	vec4 lightpos = lightpos_u;
	lightpos = projection * lightpos;

	float atten = 0.0;
	attenuation(atten, distance(lightpos, vertexpos), 1, 0.1, 0.1, 1.0);
	vec4 vlight = normalize(lightpos - vertexpos);
	float ndotl = max(0.0, dot(normal.xyz, vlight.xyz));

	vertex_color = vec4(atten * ndotl * lightcolor * received_color.xyz, 1.0) + vertex_color;
	// vertex_color = received_color;
	*/

	
	vertex_color = vec4(0.0, 0.0, 0.0, 1.0);

	for(int i = 0; i < num_lights; i++)
	{
		if(renderlights[i] == 1)
		{
			// light is in game space([0;WINDOW_WIDTH], [0;WINDOW_HEIGHT], [-1;1]) */
			vec4 lightpos = vec4(lightpositions[i].xyz, 1.0);
			lightpos = window_to_unit * lightpos;
			lightpos.x -= 1;
			lightpos.y -= 1;
			lightpos = camera_translation + lightpos;
			vec4 original_lightpos = lightpos;
			lightpos = projection * lightpos;

			float atten = 0.0;
			attenuation(atten, distance(original_lightpos, original_vertexpos), lightranges[i], 3, 4, 1.0);
			vec4 vlight = normalize(lightpos - vertexpos);
			float ndotl = max(0.0, dot(normal.xyz, vlight.xyz));

			vertex_color += vec4(atten * ndotl * lightcolors[i] * received_color.xyz, 1.0);
		}
	}
	// vertex_color = received_color;

	gl_Position = vertexpos;
}
