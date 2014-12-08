#version 400 core

in vec4 vertexpos_gamespace;
in vec4 vertexpos_modelspace;
in vec4 vertexpos_post_projection;
in vec4 vertex_color;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 color1;
layout(location = 2) out vec4 color2;
layout(location = 3) out vec4 color3;

uniform sampler2D shadowmaps[20]; /* four shadowmap per light */


uniform mat4 projection;
uniform vec3 lightpositions[20];
uniform vec3 lightcolors[20];
uniform vec2 near_far;
uniform float lightranges[20];
uniform int renderlights[20];
uniform int num_lights;
uniform int depthbuffer;
uniform int num_buffer;

uniform vec4 window_to_unit;

uniform mat2 lightblock_rects[200];
uniform int num_lightblock_rects;
uniform int do_shadowmap;
 
in vec4 normal;

void attenuation(out float atten, in float distance, in float range, in float a, in float b, in float c)
{
    atten = 1.0 / (a * distance * distance + b * distance + c) * range;
    if(atten < 0.1) { atten = 0.0; }
}

void main()
{
	color = vertex_color * vec4(0.2, 0.2, 0.2, 0.2);

	//if(do_shadowmap == 0)
	//{
		for(int i = 0; i < num_lights; i++)
		{
			vec4 frag = gl_FragCoord * (window_to_unit / 2);

			if(renderlights[i] == 1)
			{
				int shadow = 0;

				vec4 lightpos = vec4(lightpositions[i].xyz, 1.0);
				// light is in game space([0;WINDOW_WIDTH], [0;WINDOW_HEIGHT], [-1;1])
				lightpos = window_to_unit * lightpos;
				lightpos.x -= 1; // xy - 1 because space goes from -1 to 1, not 0 to 2
				lightpos.y -= 1;
				float atten = 0.0;
				attenuation(atten, 	distance(lightpos, vertexpos_modelspace), lightranges[i], 0.3, 1, 1.0);
				vec4 vlight = 		normalize(lightpos - vertexpos_modelspace);
				float ndotl = max(0.0, dot(normal.xyz, vlight.xyz));

				color += vec4(texture(shadowmaps[i], frag.xy).x * atten * ndotl * lightcolors[i] * vertex_color.xyz, 1.0);
			}

			//color += texture(shadowmaps[1], frag.xy);
		}

	//}

	//*
	if(do_shadowmap == 1)
	{
		color = vertex_color;
	}
	//*/
}