#version 330 core

layout(location = 0) in vec3 vertexpos_vec3;
uniform vec3 lightpositions[20];
uniform vec3 lightcolors[20];
uniform float lightranges[20];
uniform int renderlights[20];
uniform int num_lights;

uniform vec4 received_color;

uniform mat4 translation;
uniform mat4 projection;

out vec4 vertex_color;

vec4 normal = vec4(0.0, 0.0, -1.0, 1.0);

void attenuation(out float atten, in float distance, in float range, in float a, in float b, in float c)
{
    atten = 1.0 / (a * distance * distance + b * distance + c) * range;
}

void main()
{
	vec4 vertexpos;
	vertexpos.xyz = vertexpos_vec3;
	vertexpos.w = 1.0;

	vertexpos = projection * translation * vertexpos;

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
			vec4 lightpos = vec4(lightpositions[i].xyz, 1.0);
			lightpos = projection * lightpos;

			float atten = 0.0;
			attenuation(atten, distance(lightpos, vertexpos), lightranges[i], 0.1, 0.1, 1.0);
			vec4 vlight = normalize(lightpos - vertexpos);
			float ndotl = max(0.0, dot(normal.xyz, vlight.xyz));

			vertex_color += vec4(atten * ndotl * lightcolors[i] * received_color.xyz, 1.0);
		}
	}

	gl_Position = vertexpos;
}
