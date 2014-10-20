/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/
 
#include "render.h"

#include "constants.h"

SDL_Window *window = 0;
SDL_GLContext context;

std::string title("Window");

graphics::Shader quad_shader("shaders/quad_shader.vs", "shaders/quad_shader.fs");
graphics::Shader line_shader("shaders/line_shader.vs", "shaders/line_shader.fs");

GLuint VertexArrayID;
GLuint rectangle_vertex_buffer;
GLuint line_vertex_buffer;
GLuint light_position_buffer;

static GLfloat rectangle_buffer_data[] = {
	-1.0f,	 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	 1.0f,	-1.0f,	0.0f,

	 1.0f,	 1.0f, 0.0f,
	-1.0f,	 1.0f,	0.0f,
	 1.0f,	-1.0f,	0.0f
};

static GLfloat line_buffer_data[] = {
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f
};

float lightpositions[MAX_LIGHTS * 3];
float lightcolors[MAX_LIGHTS * 3];
float lightranges[MAX_LIGHTS];
int renderlights[MAX_LIGHTS];

math::vec4f light_position(0.0f, 0.0f, 0.1f, 1.0f);

math::mat4f translation_matrix;
GLuint quad_translation_matrix_uniform_id;

math::mat4f projection_matrix;
GLuint quad_projection_matrix_uniform_id;

math::vec4f scale_vector;
GLuint quad_scale_vector_uniform_id;

GLuint quad_color_vector_uniform_id;

GLuint quad_lightcolor_vectorarray_uniform_id;
GLuint quad_lightpos_vectorarray_uniform_id;
GLuint quad_lightrange_floatarray_uniform_id;
GLuint quad_numlights_int_uniform_id;
GLuint quad_renderlights_intarray_uniform_id;

/*********************************************/

std::vector<math::mat2f *> light_block_rects;

namespace graphics
{
	int num_lights;
	std::vector<Light *> lights;

	int win_width = 800, win_height = 600;

	void set_window_title(std::string t)
	{
		title = t;
	}

	void set_window_size(int w, int h)
	{
		win_width = w;
		win_height = h;
	}

	void add_light(Light *l)
	{
		lights.push_back(l);
		num_lights = lights.size();
	}

	void add_light_block_rect(math::mat2f *rect)
	{
		if(rect)
			light_block_rects.push_back(rect);
	}

	void clear_light_block_rects()
	{
		light_block_rects.clear();
	}

	/**
	 * Create SDL window & context.
	 *
	 * @return 0 on success, otherwise -1 or other error code
	 */
	int init_sdl()
	{
		if(SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::cout << "Error initialising SDL." << std::endl;
			return -1;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_width, win_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if(window == 0)
	   	{
	       	std::cout << "Window could not be created." << std::endl;
	        return -1;
	   	}

	   	context = SDL_GL_CreateContext(window);
	   	if(context == 0)
	   	{
	       	std::cout << "Context could not be created: " << SDL_GetError() << std::endl;
	        return -1;
	   	}

	   	//Initialize GLEW
        glewExperimental = GL_TRUE; 
        GLenum glew_err = glewInit();
        if(glew_err != GLEW_OK)
        {
            printf("Error initializing GLEW! %s\n", glewGetErrorString(glew_err));
        }

	 	if(SDL_GL_SetSwapInterval(1) < 0)
	   	{
	       	std::cout << "Unable to set VSYNC." << std::endl;
	   	}

	   	SDL_ShowCursor(SDL_DISABLE);

		return 0;
	}

	int init_gl()
	{
		quad_shader.compile();

		GLuint vertex_arr_id;
		glGenVertexArrays(1, &vertex_arr_id);
		glBindVertexArray(vertex_arr_id);

		glGenBuffers(1, &rectangle_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, rectangle_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_buffer_data), rectangle_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &line_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, line_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(line_buffer_data), line_buffer_data, GL_STATIC_DRAW);

		/*
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_SRC_COLOR);
		*/

		/*
		projection_matrix = math::mat4f(1, 0, 0, 1,
										0, 1, 0, 1,
										0, 0, 1, 1,
										0, 0, 0, 1);
										*/

		projection_matrix = math::orthoMat4(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);

		quad_translation_matrix_uniform_id = 		quad_shader.getUniformLocation("translation");
		quad_projection_matrix_uniform_id = 		quad_shader.getUniformLocation("projection");
		quad_scale_vector_uniform_id = 				quad_shader.getUniformLocation("scale");
		quad_color_vector_uniform_id = 				quad_shader.getUniformLocation("received_color");
		quad_lightcolor_vectorarray_uniform_id = 	quad_shader.getUniformLocation("lightcolors");
		quad_lightpos_vectorarray_uniform_id = 		quad_shader.getUniformLocation("lightpositions");
		quad_lightrange_floatarray_uniform_id = 	quad_shader.getUniformLocation("lightranges");
		quad_numlights_int_uniform_id = 			quad_shader.getUniformLocation("num_lights");
		quad_renderlights_intarray_uniform_id = 	quad_shader.getUniformLocation("renderlights");

		return 0;
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(int i = 0; i < num_lights; ++i)
		{
			lightpositions[i * 3 + 0] = lights[i]->position.x;
			lightpositions[i * 3 + 1] = lights[i]->position.y;
			lightpositions[i * 3 + 2] = lights[i]->position.z;

			lightcolors[i * 3 + 0] = lights[i]->color.x;
			lightcolors[i * 3 + 1] = lights[i]->color.y;
			lightcolors[i * 3 + 2] = lights[i]->color.z;

			lightranges[i] = lights[i]->range;
			renderlights[i] = 1;
		}
	}

	void render()
	{
		SDL_GL_SwapWindow(window);
	}

	void destroy()
	{
		free(window);
	}

	namespace draw
	{
		void rectangle(math::mat2f &rect, math::vec4f &color, float z_index)
		{
			float x = rect[0]; /* | x   y | */
			float y = rect[3]; /* |       | */
			float w = rect[1]; /* |       | */
			float h = rect[4]; /* | w   h | */

			rectangle_buffer_data[0] = x;
			rectangle_buffer_data[1] = y;
			
			rectangle_buffer_data[3] = x;
			rectangle_buffer_data[4] = y + h;

			rectangle_buffer_data[6] = x + w;
			rectangle_buffer_data[7] = y + h;

			/*********************************/

			rectangle_buffer_data[9] = x;
			rectangle_buffer_data[10] = y;

			rectangle_buffer_data[12] = x + w;
			rectangle_buffer_data[13] = y + h;

			rectangle_buffer_data[15] = x + w;
			rectangle_buffer_data[16] = y;

			/* check if light should be rendered */
			/*
			for(int i = 0; i < num_lights; ++i)
			{
				math::vec2f lpos(lights[i]->position.x, lights[i]->position.y);
				math::vec2f rpos(x + w / 2, y + h / 2);

				renderlights[i] = 1;
				for(int i_blocks = 0; i_blocks < light_block_rects.size(); ++i_blocks)
				 {
					if(math::line_intersects_rect(lpos, rpos, *(light_block_rects[i_blocks])) == 1)
					{
						renderlights[i] = 0;
					}
				}
			}
			*/

			glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_buffer_data), rectangle_buffer_data, GL_STATIC_DRAW);

			quad_shader.bind();

			translation_matrix = math::translationMat4(math::vec4f(0.0f, 0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(quad_translation_matrix_uniform_id, 1, GL_FALSE, &(translation_matrix[0]));
			glUniformMatrix4fv(quad_projection_matrix_uniform_id, 1, GL_FALSE, &(projection_matrix[0]));
			glUniform4fv(quad_color_vector_uniform_id, 1, &(color[0]));
			glUniform3fv(quad_lightpos_vectorarray_uniform_id, MAX_LIGHTS, lightpositions);
			glUniform3fv(quad_lightcolor_vectorarray_uniform_id, MAX_LIGHTS, lightcolors);

			glUniform1fv(quad_lightrange_floatarray_uniform_id, MAX_LIGHTS, lightranges);
			glUniform1iv(quad_renderlights_intarray_uniform_id, MAX_LIGHTS, renderlights);
			glUniform1i(quad_numlights_int_uniform_id, num_lights);


			glBindBuffer(GL_ARRAY_BUFFER, rectangle_vertex_buffer);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
			   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(0);
			quad_shader.unbind();
		}

		float rot = 0;
		void polygon(Polygon &poly, math::vec4f &color, float z_index)
		{
			glPushMatrix();
				glNormal3f(0.0f, 0.0f, 1.0f); /* top down game, normal is always towards the camera(unless specified in normal map) */

				glTranslatef(	poly.pos().x + poly.size().x / 2, 
								poly.pos().y + poly.size().y / 2,
								z_index); /* translate to the centre of the polygon so rotations are possible */
								/* move towards camera so shadows can be painted */

				glColor4f(color.x, color.y, color.z, color.w);
				glBegin(GL_POLYGON);
					for(int i = 0; i < poly.num_vertices(); ++i)
					{
						math::vec2f vert = poly.get_vertices()[i];

						glVertex2f(	vert.x - (poly.pos().x + poly.size().x / 2),
									vert.y - (poly.pos().y + poly.size().y / 2));
					}
				glEnd();
			glPopMatrix();
		}

		void line(math::vec2f p1, math::vec2f p2, math::vec4f &color, float z_index)
		{
			float 	low_x = p1.x, high_x = p2.x, 
					low_y = p1.y, high_y = p2.y;
			if(p2.x < p1.x) { low_x = p2.x; high_x = p1.x; };
			if(p2.y < p1.y) { low_y = p2.y; high_y = p1.y; };

			float width = high_x - low_x;
			float height = high_y - low_y;

			quad_shader.bind();

			line_buffer_data[0] = p1.x;
			line_buffer_data[1] = p1.y;
			line_buffer_data[3] = p2.x;
			line_buffer_data[4] = p2.y;
			glBindBuffer(GL_ARRAY_BUFFER, line_vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line_buffer_data), line_buffer_data, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
			   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);

			scale_vector = math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
			translation_matrix = math::translationMat4(math::vec4f(0.0f, 0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(quad_translation_matrix_uniform_id, 1, GL_FALSE, &(translation_matrix[0]));
			glUniformMatrix4fv(quad_projection_matrix_uniform_id, 1, GL_FALSE, &(projection_matrix[0]));
			glUniform4fv(quad_scale_vector_uniform_id, 1, &(scale_vector[0]));
			glUniform4fv(quad_color_vector_uniform_id, 1, &(color[0]));
			 
			// Draw the triangle !
			glDrawArrays(GL_LINES, 0, 2);
			glDisableVertexAttribArray(0);
			quad_shader.unbind();
		}

		void outline(Polygon &poly, math::vec4f &color, float z_index)
		{
			glPushMatrix();
				glNormal3f(0.0f, 0.0f, 1.0f); /* top down game, normal is always towards the camera(unless specified in normal map) */

				glTranslatef(	poly.pos().x + poly.size().x / 2, 
								poly.pos().y + poly.size().y / 2,
								z_index); /* translate to the centre of the polygon so rotations are possible */
								/* move towards camera so shadows can be painted */

				glColor4f(color.x, color.y, color.z, color.w);
				glBegin(GL_LINES);
					math::vec2f prev = poly.get_vertices()[0];

					for(int i = 1; i < poly.num_vertices(); ++i)
					{
						math::vec2f vert = poly.get_vertices()[i];

						glVertex2f(	prev.x - (poly.pos().x + poly.size().x / 2),
									prev.y - (poly.pos().y + poly.size().y / 2));

						glVertex2f(	vert.x - (poly.pos().x + poly.size().x / 2),
									vert.y - (poly.pos().y + poly.size().y / 2));
						
						prev = vert;
					}

					glVertex2f(	prev.x - (poly.pos().x + poly.size().x / 2),
								prev.y - (poly.pos().y + poly.size().y / 2));
					
					prev = poly.get_vertices()[0];
					glVertex2f(	prev.x - (poly.pos().x + poly.size().x / 2),
								prev.y - (poly.pos().y + poly.size().y / 2));
				glEnd();
			glPopMatrix();
		}
	};
};