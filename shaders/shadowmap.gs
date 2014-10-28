#version 330 core

uniform mat4 camera_rotation_matrices[6];

out vec4 pos_from_gs;

void main()
{
	for(gl_Layer = 0; gl_Layer < 6; ++gl_Layer)
	{
		for(int tri_vert = 0; tri_vert < 3; ++tri_vert)
		{
			pos_from_gs = gl_in[tri_vert].gl_Position;
			gl_Position = camera_rotation_matrices[gl_Layer] * pos_from_gs;
			EmitVertex();
		}
		EndPrimitive();
	}
}