/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/

#include "render.h"

math::mat4f rot_pos_x, rot_neg_x, rot_pos_y, rot_neg_y, rot_pos_z, rot_neg_z;

namespace graphics
{
	void init_shadowmap_rotation_matrices()
	{
		rot_pos_x = math::mat4f( 0, 0,-1, 0,
								 0,-1, 0, 0,
								-1, 0, 0, 0,
								 0, 0, 0, 1);

		rot_neg_x = math::mat4f( 0, 0, 1, 0,
								 0,-1, 0, 0,
								 1, 0, 0, 0,
								 0, 0, 0, 1);

		rot_pos_y = math::mat4f( 1, 0, 0, 0,
								 0, 0, 1, 0,
								 0,-1, 0, 0,
								 0, 0, 0, 1);

		rot_neg_y = math::mat4f( 1, 0, 0, 0,
								 0, 0,-1, 0,
								 0, 1, 0, 0,
								 0, 0, 0, 1);

		rot_pos_z = math::mat4f( 1, 0, 0, 0,
								 0,-1, 0, 0,
								 0, 0,-1, 0,
								 0, 0, 0, 1);

		rot_neg_z = math::mat4f(-1, 0, 0, 0,
								 0,-1, 0, 0,
								 0, 0,+1, 0,
								 0, 0, 0, 1);
	}

	void Shadow_Map::init()
	{
		// create texture
		glGenTextures(1, &(this->cubemap_texture));
		glBindTexture(GL_TEXTURE_CUBE_MAP, this->cubemap_texture);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		for(int face = 0; face < 6; ++face)
		{
			glTexImage2D(	GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
							0,
							GL_DEPTH_COMPONENT24, // internal format
							WINDOW_WIDTH, WINDOW_HEIGHT, 0,
							GL_DEPTH_COMPONENT, // format
							GL_UNSIGNED_INT, // datatype
							0);
		}

		// create FBO
		glGenFramebuffers(1, &(this->fbo));
		glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->cubemap_texture, 0);

		/* no color buffers */
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		/* assumes shader is compiled */
		this->posx_id = this->shader->getUniformLocation("shadowmap_rotation_matrices[0]");
		this->negx_id = this->shader->getUniformLocation("shadowmap_rotation_matrices[1]");
		this->posy_id = this->shader->getUniformLocation("shadowmap_rotation_matrices[2]");
		this->negy_id = this->shader->getUniformLocation("shadowmap_rotation_matrices[3]");
		this->posz_id = this->shader->getUniformLocation("shadowmap_rotation_matrices[4]");
		this->negz_id = this->shader->getUniformLocation("shadowmap_rotation_matrices[5]");
		printf("ids: %i, %i\n", this->posx_id, this->negx_id);
		printf("texture id: %i\n", this->cubemap_texture);
	}

	void Shadow_Map::upload_rotation_matrices()
	{
		this->shader->bind();
		glUniformMatrix4fv(this->posx_id, 1, GL_FALSE, &(rot_pos_x[0]));
		glUniformMatrix4fv(this->negx_id, 1, GL_FALSE, &(rot_neg_x[0]));
		glUniformMatrix4fv(this->posy_id, 1, GL_FALSE, &(rot_pos_y[0]));
		glUniformMatrix4fv(this->negy_id, 1, GL_FALSE, &(rot_neg_y[0]));
		glUniformMatrix4fv(this->posz_id, 1, GL_FALSE, &(rot_pos_z[0]));
		glUniformMatrix4fv(this->negz_id, 1, GL_FALSE, &(rot_neg_z[0]));
		this->shader->unbind();
	};
};