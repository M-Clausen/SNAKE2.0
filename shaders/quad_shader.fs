#version 330 core

in vec4 vertexpos_gamespace;
in vec4 vertexpos_pre_projection;
in vec4 vertexpos_post_projection;
in vec4 vertex_color;
flat in int docolor;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;

uniform sampler2D shadowmaps[20*4]; /* 4 shadowmaps per light, ordered up, right, down, left, so light#2 is index#4 etc. */

uniform vec3 lightpositions[20];
uniform vec3 lightcolors[20];
uniform float lightranges[20];
uniform int renderlights[20];
uniform int num_lights;
uniform int depthbuffer;
uniform int num_depthbuffer;

uniform vec4 window_to_unit;

uniform mat2 lightblock_rects[200];
uniform int num_lightblock_rects;
 
in vec4 normal;

void attenuation(out float atten, in float distance, in float range, in float a, in float b, in float c)
{
    atten = 1.0 / (a * distance * distance + b * distance + c) * range;
    if(atten < 0.1) { atten = 0.0; }
}

void main()
{
	color = vec4(0.0, 0.0, 0.0, 0.0);
	for(int i = 0; i < num_lights; i++)
	{
		if(renderlights[i] == 1)
		{
			vec4 lightpos = vec4(lightpositions[i].xyz, 1.0);
			// light is in game space([0;WINDOW_WIDTH], [0;WINDOW_HEIGHT], [-1;1])
			lightpos = window_to_unit * lightpos;
			lightpos.x -= 1; // xy - 1 because space goes from -1 to 1, not 0 to 2
			lightpos.y -= 1;

			float atten = 0.0;
			attenuation(atten, distance(lightpos, vertexpos_pre_projection), lightranges[i], 0.3, 1, 1.0);
			vec4 vlight = normalize(lightpos - vertexpos_pre_projection);
			float ndotl = max(0.0, dot(normal.xyz, vlight.xyz));

			color += vec4(atten * ndotl * lightcolors[i] * vertex_color.xyz, 1.0);
		}
	}

	if(docolor == 0)
	{
		color = vec4(0.1, 0.1, 0.1, 1.0) * vertex_color;
	}

	//*
	if(depthbuffer == 1)
	{
		color = vec4(1000.0, 1000.0, 1000.0, 1.0);
		if(num_depthbuffer == 0)
		{
			color = vec4((1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, 1.0);
		}
		else if(num_depthbuffer == 1)
		{
			color1 = vec4((1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, 1.0);
		}
		else if(num_depthbuffer == 2)
		{
			color2 = vec4((1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, 1.0);
		}
		else if(num_depthbuffer == 3)
		{
			color3 = vec4((1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, (1 - (vertexpos_post_projection.z) - 1.01) / 2, 1.0);
		}
	}
	//*/
}