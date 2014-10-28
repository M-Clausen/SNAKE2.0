/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "render.h"

namespace graphics
{
	void Light::init(Shader *shad, math::mat4f *proj_matrix, math::mat4f *transl_matrix)
	{
		this->shadowmap = new Shadow_Map();
	}

	/*
	void Light::destroy()
	{

	}
	*/
};