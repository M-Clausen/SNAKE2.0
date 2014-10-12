/******************************************
 * COPYRIGHT (C) 2014 MADS HAUPT CLAUSEN
 ******************************************/
 
#include "draw.h"

SDL_Window *window = 0;
SDL_GLContext context;

std::string title("Window");

namespace video
{
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

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

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

	 	if(SDL_GL_SetSwapInterval(1) < 0)
	   	{
	       	std::cout << "Unable to set VSYNC." << std::endl;
	   	}

		return 0;
	}

	int init_gl()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glOrtho(0, win_width, win_height, 0, -1, 1);

		glDisable(GL_DEPTH_TEST); /* not needed, 2D drawing */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/* glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);


		GLfloat lightpos[] = {600, 400, 0.4f, 1.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
		*/

		return 0;
	}

	void clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

			glPushMatrix();
				glNormal3f(0.0f, 0.0f, 1.0f);

				glTranslatef(x + w / 2, y + h / 2, z_index);

				/*
				if(glIsEnabled(GL_LIGHTING) == GL_TRUE)
				{
					GLfloat mat_color[] = {color.x, color.y, color.z, color.w};
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);
				}
				else
				{
					glColor4f(color.x, color.y, color.z, color.w);
				}
				*/

				glColor4f(color.x, color.y, color.z, color.w);
				glBegin(GL_QUADS);
					glVertex2f(-(w / 2), 	-(h / 2));
					glVertex2f(-(w / 2),	 (h / 2));
					glVertex2f( (w / 2), 	 (h / 2));
					glVertex2f( (w / 2), 	-(h / 2));
				glEnd();
			glPopMatrix();
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

			glPushMatrix();
				glNormal3f(0.0f, 0.0f, 1.0f); /* top down game, normal is always towards the camera(unless specified in normal map) */

				glTranslatef(	low_x + width / 2, 
								low_y + height / 2,
								z_index); /* translate to the centre of the polygon so rotations are possible */
								/* move towards camera so shadows can be painted */

				glColor4f(color.x, color.y, color.z, color.w);
				glLineWidth(1.0f);
				glBegin(GL_LINES);
					glVertex2f(p1.x -(low_x + width / 2),  p1.y -(low_y + height / 2));
					glVertex2f(p2.x -(low_x + width / 2),  p2.y -(low_y + height / 2));
				glEnd();
			glPopMatrix();
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