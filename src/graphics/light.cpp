/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "render.h"

math::vec4f _window_to_unit_transform_vector = math::vec4f(2.0f / (WINDOW_WIDTH), 2.0f / (WINDOW_HEIGHT), 1.0f, 1.0f);

namespace graphics
{
	void Light::init(Shader *shad, math::mat4f *proj_matrix)
	{
		this->shadowmap = new Shadow_Map();
		this->shadowmap->shader = shad;
		this->shadowmap->init();
	}

	void Light::update_translation_matrix()
	{
		if(this->shadowmap->translation_matrix == 0)
		{

			this->shadowmap->translation_matrix = new math::mat4f;
		}

		math::vec4f translate = math::vec4f(-this->position.x, -this->position.y, this->position.z, 1.0f);
		translate = translate * _window_to_unit_transform_vector;
		
		translate.x += 1.0f;
		translate.y += 1.0f;
		
		*(this->shadowmap->translation_matrix) = math::translationMat4(translate);
	}

	void Light::destroy()
	{
		delete(this->shadowmap);
	}
};