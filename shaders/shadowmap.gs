#version 330 core

uniform mat4 shadowmap_rotation_matrices[4];
uniform mat4 projection;

out vec4 pos_from_gs;
flat out int num_buffer;

void main()
{
	for(gl_Layer = 0; gl_Layer < 4; ++gl_Layer)
	{
		for(int tri_vert = 0; tri_vert < 3; ++tri_vert)
		{
			pos_from_gs = gl_in[tri_vert].gl_Position;
			gl_Position = shadowmap_rotation_matrices[gl_Layer] * pos_from_gs;
			gl_Position = projection * gl_Position;
			pos_from_gs = gl_Position;

			num_buffer = gl_Layer;
			EmitVertex();
		}
		EndPrimitive();
	}
}