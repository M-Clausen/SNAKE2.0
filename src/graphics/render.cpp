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
graphics::Shader shadowmap_shader("shaders/shadowmap.vs", "shaders/shadowmap.fs");

GLuint VertexArrayID;
GLuint quad_vertex_buffer;
GLuint quad_normal_buffer;
GLuint line_vertex_buffer;
GLuint light_position_buffer;

static GLfloat quad_buffer_data[] = {
	// top face
	//*
	-1.0f,	 1.0f, 	2.0f,
	-1.0f,  -1.0f,  2.0f,
	 1.0f,	-1.0f,	2.0f,

	 1.0f,	 1.0f, 	2.0f,
	-1.0f,	 1.0f,	2.0f,
	 1.0f,	-1.0f,	2.0f,
	 //*/

	//*
	// right face
	1.0f, 	-1.0f, 1.0f,
	1.0f, 	-1.0f,	2.0f,
	1.0f, 	 1.0f, 1.0f,

	1.0f,	-1.0f, 	2.0f,
	1.0f, 	 1.0f, 1.0f,
	1.0f, 	 1.0f, 	2.0f,

	// back face(aka bottom)
	-1.0f,	 1.0f, 	2.0f,
	-1.0f,  -1.0f, 	2.0f,
	 1.0f,	-1.0f,	2.0f,

	 1.0f,	 1.0f, 	2.0f,
	-1.0f,	 1.0f,	2.0f,
	 1.0f,	-1.0f,	2.0f,


	// left face
	-1.0f, 	-1.0f, 1.0f,
	-1.0f, 	-1.0f,	2.0f,
	-1.0f, 	 1.0f, 1.0f,

	-1.0f,	-1.0f, 	2.0f,
	-1.0f, 	 1.0f, 1.0f,
	-1.0f, 	 1.0f, 	2.0f,

	// posY
	-1.0f, 	1.0f, 	1.0f,
	-1.0f, 	1.0f, 	2.0f,
	1.0f,	1.0f,	1.0f,

	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 2.0f,
	-1.0f, 1.0f, 2.0f,

	// negY
	-1.0f, 	-1.0f, 	1.0f,
	-1.0f, 	-1.0f, 	2.0f,
	1.0f,	-1.0f,	1.0f,

	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 2.0f,
	-1.0f, -1.0f, 2.0f
};

static GLfloat quad_normal_buffer_data[] = {
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f
};

static GLfloat line_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, 1.0f, 0.0f
};

float lightpositions[MAX_LIGHTS * 3];
float lightcolors[MAX_LIGHTS * 3];
float lightranges[MAX_LIGHTS];
int renderlights[MAX_LIGHTS];
GLuint shadowmaps[MAX_LIGHTS * 4];
float *shadowmap_translation_matrices[MAX_LIGHTS];
float *shadowmap_rotation_matrices[4];
float *shadowmap_projection_matrices[MAX_LIGHTS];

float *light_block_rects_uniform_array[MAX_LIGHT_BLOCKS];
GLuint light_block_rects_uniform_array_id;
GLuint num_light_block_rects_uniform_id;

math::vec4f light_position(0.0f, 0.0f, 0.1f, 1.0f);

/* mainly for lights. transforms e.g. (500, 400) into something like (-0.2, -0.923) */
math::vec4f window_to_unit_transform_vector;
GLuint quad_window_to_unit_uniform_id;
GLuint smshader_window_to_unit_uniform_id;

math::mat4f translation_matrix;
GLuint quad_translation_matrix_uniform_id;
GLuint smshader_translation_matrix_uniform_id;

math::mat4f camera_translation_matrix;
GLuint quad_camera_translation_matrix_uniform_id;
GLuint smshader_camera_translation_matrix_uniform_id;

math::mat4f camera_rotation_matrix;
GLuint quad_camera_rotation_matrix_uniform_id;
GLuint smshader_camera_rotation_matrix_uniform_id;

math::mat4f projection_matrix;
GLuint quad_projection_matrix_uniform_id;
GLuint smshader_projection_matrix_uniform_id;

math::mat4f shadowmap_projection_matrix;
// GLuint quad_shadowmap_projection_matrix_uniform_id;

math::mat4f rotation_matrix;
GLuint quad_rotation_matrix_uniform_id;
GLuint smshader_rotation_matrix_uniform_id;

math::vec4f scale_vector;
GLuint quad_scale_vector_uniform_id;
GLuint smshader_scale_vector_uniform_id;

GLuint quad_color_vector_uniform_id;

GLuint quad_lightcolor_vectorarray_uniform_id;
GLuint quad_lightpos_vectorarray_uniform_id;
GLuint quad_lightrange_floatarray_uniform_id;
GLuint quad_numlights_int_uniform_id;
GLuint quad_renderlights_intarray_uniform_id;

GLuint smshader_lightcolor_vectorarray_uniform_id;
GLuint smshader_lightpos_vectorarray_uniform_id;
GLuint smshader_lightrange_floatarray_uniform_id;
GLuint smshader_numlights_int_uniform_id;
GLuint smshader_renderlights_intarray_uniform_id;

GLuint quad_depthbuffer_int_uniform_id;
GLuint quad_num_depthbuffer_int_uniform_id;
GLuint quad_shadowmaps_gluintarray_uniform_id;

math::vec2f near_far_vector;
GLuint smshader_near_far_vector_uniform_id;

/*********************************************/

std::vector<math::mat2f *> light_block_rects;
std::vector<graphics::Rectangle *> rectangles;

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

	void add_light(Light *l, char gen_framebuffer)
	{
		lights.push_back(l);
		num_lights = lights.size();

		/*
		if(gen_framebuffer == 1)
		{
			std::cout << "generating shadowmap shit" << std::endl;
			printf("glGenFramebuffer = %p\n", glGenFramebuffers);
			l->shadowmap_aspectratio = 1 / 200;
			glGenFramebuffers(1, &(l->shadowmap_framebuffer));
			std::cout << "generated framebuffer" << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, l->shadowmap_framebuffer);

			// generate four textures, so everything around the point light can be covered
			glGenTextures(1, &l->shadowmap_texture_up);
			glBindTexture(GL_TEXTURE_2D, l->shadowmap_texture_up);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			std::cout << "generating texture 1 with ID #" << l->shadowmap_texture_up << std::endl;

			glGenTextures(1, &l->shadowmap_texture_right);
			glBindTexture(GL_TEXTURE_2D, l->shadowmap_texture_right);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			std::cout << "generating texture 2 with ID #" << l->shadowmap_texture_right << std::endl;

			glGenTextures(1, &l->shadowmap_texture_down);
			glBindTexture(GL_TEXTURE_2D, l->shadowmap_texture_down);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			std::cout << "generating texture 3 with ID #" << l->shadowmap_texture_down << std::endl;

			glGenTextures(1, &l->shadowmap_texture_left);
			glBindTexture(GL_TEXTURE_2D, l->shadowmap_texture_left);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			std::cout << "generating texture 4 with ID #" << l->shadowmap_texture_left << std::endl;

			// add textures to framebuffer
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, l->shadowmap_texture_up, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, l->shadowmap_texture_right, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, l->shadowmap_texture_down, 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, l->shadowmap_texture_left, 0);

			GLenum drawbuffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
			glDrawBuffers(4, drawbuffers);

			shadowmaps[(num_lights - 1) * 4 + 0] = l->shadowmap_texture_up;
			shadowmaps[(num_lights - 1) * 4 + 1] = l->shadowmap_texture_right;
			shadowmaps[(num_lights - 1) * 4 + 2] = l->shadowmap_texture_down;
			shadowmaps[(num_lights - 1) * 4 + 3] = l->shadowmap_texture_left;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		*/
	}

	void add_light_block_rect(math::mat2f *rect)
	{
		std::cout << "add light block " << light_block_rects.size() << std::endl;
		if(rect)
			light_block_rects.push_back(rect);

		for(int i = 0; i < light_block_rects.size(); ++i)
		{
			light_block_rects_uniform_array[i] = &((*rect)[0]);
		}
	
		quad_shader.bind();
		glUniformMatrix2fv(light_block_rects_uniform_array_id, light_block_rects.size(), GL_FALSE, light_block_rects_uniform_array[0]);
		glUniform1i(num_light_block_rects_uniform_id, light_block_rects.size());
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

	float calculate_frustumscale(float fov_deg)
	{
	    const float deg_to_rad = 3.14159f * 2.0f / 360.0f;
	    float fov_rad = fov_deg * deg_to_rad;
	    return 1.0f / tan(fov_rad / 2.0f);
	}

	int init_gl()
	{
		if(quad_shader.compile() == -1)
		{
			return -1;
		}

		if(shadowmap_shader.compile() == -1 || shadowmap_shader.addGeometryShader("shaders/shadowmap.gs") == -1)
		{
			return -1;
		}

		GLuint vertex_arr_id;
		glGenVertexArrays(1, &vertex_arr_id);
		glBindVertexArray(vertex_arr_id);

		glGenBuffers(1, &quad_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_buffer_data), quad_buffer_data, GL_STATIC_DRAW);

		glGenBuffers(1, &quad_normal_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_normal_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_normal_buffer_data), quad_normal_buffer_data, GL_STATIC_DRAW);

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

		projection_matrix = math::orthoMat4(-1, 1, -1, 1, 1, 5);

		//*
		float n = 0.01f;
		float t = tan(90 / 2 * (DEGTORAD)) * n;
		float b = -t;
	    float r = t * 1;
	    float l = b * 1;
		shadowmap_projection_matrix = math::perspectiveMat4(l, r, -t, -b, n, n + 4);
		std::cout << "top: " << t << std::endl;
		//*/

		rotation_matrix = math::rotationMat4(0.0f, math::vec3f(0.0f, 0.0f, 0.0f));

		translation_matrix = math::translationMat4(math::vec4f(0.0f, 0.0f, 0.0f, 1.0f));
		window_to_unit_transform_vector = math::vec4f(2.0f / (WINDOW_WIDTH), 2.0f / (WINDOW_HEIGHT), 1.0f, 1.0f);
		printf("window_to_unit = (%f, %f, %f)\n", window_to_unit_transform_vector.x, window_to_unit_transform_vector.y, 1.0f);

		quad_translation_matrix_uniform_id = 		quad_shader.getUniformLocation("model_translation");
		quad_projection_matrix_uniform_id = 		quad_shader.getUniformLocation("projection");
		quad_rotation_matrix_uniform_id	=			quad_shader.getUniformLocation("model_rotation");
		quad_scale_vector_uniform_id = 				quad_shader.getUniformLocation("scale");
		quad_color_vector_uniform_id = 				quad_shader.getUniformLocation("received_color");
		quad_lightcolor_vectorarray_uniform_id = 	quad_shader.getUniformLocation("lightcolors");
		quad_lightpos_vectorarray_uniform_id = 		quad_shader.getUniformLocation("lightpositions");
		quad_lightrange_floatarray_uniform_id = 	quad_shader.getUniformLocation("lightranges");
		quad_numlights_int_uniform_id = 			quad_shader.getUniformLocation("num_lights");
		quad_renderlights_intarray_uniform_id = 	quad_shader.getUniformLocation("renderlights");
		quad_window_to_unit_uniform_id =			quad_shader.getUniformLocation("window_to_unit");
		quad_camera_translation_matrix_uniform_id = quad_shader.getUniformLocation("camera_translation");
		quad_camera_rotation_matrix_uniform_id =	quad_shader.getUniformLocation("camera_rotation");
		quad_depthbuffer_int_uniform_id = 			quad_shader.getUniformLocation("depthbuffer");
		quad_num_depthbuffer_int_uniform_id = 		quad_shader.getUniformLocation("num_depthbuffer");
		quad_shadowmaps_gluintarray_uniform_id = 	quad_shader.getUniformLocation("shadowmaps");
		light_block_rects_uniform_array_id = 		quad_shader.getUniformLocation("lightblock_rects");
		num_light_block_rects_uniform_id = 			quad_shader.getUniformLocation("num_lightblock_rects");

		smshader_translation_matrix_uniform_id = 		shadowmap_shader.getUniformLocation("model_translation");
		smshader_projection_matrix_uniform_id = 		shadowmap_shader.getUniformLocation("projection");
		smshader_rotation_matrix_uniform_id	=			shadowmap_shader.getUniformLocation("model_rotation");
		smshader_scale_vector_uniform_id = 				shadowmap_shader.getUniformLocation("scale");
		smshader_window_to_unit_uniform_id =			shadowmap_shader.getUniformLocation("window_to_unit");
		smshader_camera_translation_matrix_uniform_id = shadowmap_shader.getUniformLocation("camera_translation");
		smshader_near_far_vector_uniform_id = 			shadowmap_shader.getUniformLocation("near_far");

		glEnable(GL_DEPTH_TEST);

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

			/*
			if(i == 1)
			{
				math::vec4f translate = math::vec4f(-lights[i]->position.x, -lights[i]->position.y, lights[i]->position.z, 1.0f);
				// math::vec4f translate(0.0f, 0.0f, -2.0f, 1.0f);
				translate =  translate * window_to_unit_transform_vector;
				
				translate.x /= 2;
				translate.x += 0.645f;
				translate.y /= 2;
				translate.y += 0.23f;
				
				// camera_translation_matrix = math::translationMat4(translate);
				// camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, -1.0f));
			}
			//*/
		}
	}

	void render_rect(Rectangle *rect, math::mat4f &projection, math::mat4f &camera_translate, math::mat4f &camera_rotation, char shadowmap = 0, int num_depthbuffer = 0)
	{
		float x = rect->rect[0]; /* | x   y | */
		float y = rect->rect[3]; /* |       | */
		float w = rect->rect[1]; /* |       | */
		float h = rect->rect[4]; /* | w   h | */
		math::vec4f color = rect->color;
		float height = rect->height;

		float scale_w = w / (WINDOW_WIDTH);
		float scale_h = h / (WINDOW_HEIGHT);
		scale_vector = math::vec4f(scale_w, scale_h, height, 1.0);
		translation_matrix = math::translationMat4(math::vec4f(2 * x / (WINDOW_WIDTH) - 1 + scale_w, 2 * y / (WINDOW_HEIGHT) - 1 + scale_h, -height, 1.0f));

		/* assume quad_shader is being used */
		if(shadowmap == 0)
		{	
			quad_shader.bind();
			glUniformMatrix4fv(quad_translation_matrix_uniform_id, 			1, GL_FALSE, &(translation_matrix[0]));
			glUniformMatrix4fv(quad_projection_matrix_uniform_id, 			1, GL_FALSE, &(projection[0]));
			glUniformMatrix4fv(quad_rotation_matrix_uniform_id, 			1, GL_FALSE, &(rotation_matrix[0]));
			glUniformMatrix4fv(quad_camera_translation_matrix_uniform_id, 	1, GL_FALSE, &(camera_translate[0]));
			glUniformMatrix4fv(quad_camera_rotation_matrix_uniform_id, 		1, GL_FALSE, &(camera_rotation[0]));

			glUniform4fv(quad_scale_vector_uniform_id, 1, &(scale_vector[0]));
			glUniform4fv(quad_color_vector_uniform_id, 1, &(color[0]));
			glUniform4fv(quad_window_to_unit_uniform_id, 1, &(window_to_unit_transform_vector[0]));

			glUniform3fv(quad_lightpos_vectorarray_uniform_id, 		MAX_LIGHTS, lightpositions);
			glUniform3fv(quad_lightcolor_vectorarray_uniform_id, 	MAX_LIGHTS, lightcolors);

			glUniform1fv(quad_lightrange_floatarray_uniform_id, MAX_LIGHTS, lightranges);
			glUniform1iv(quad_renderlights_intarray_uniform_id, MAX_LIGHTS, renderlights);
			glUniform1uiv(quad_shadowmaps_gluintarray_uniform_id, MAX_LIGHTS * 4, shadowmaps);

			glUniform1i(quad_numlights_int_uniform_id, num_lights);
			glUniform1i(quad_depthbuffer_int_uniform_id, shadowmap == 0 ? 0 : 1);
			glUniform1i(quad_num_depthbuffer_int_uniform_id, num_depthbuffer);

			glBindBuffer(GL_ARRAY_BUFFER, quad_normal_buffer);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
			   1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);

			glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
			   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);

			glDrawArrays(GL_TRIANGLES, 0, 6*6);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			quad_shader.unbind();
		}
		else if(shadowmap == 1) // use special shader for each shadowmap
		{
			// render a shadowmap for each light
			for(int i = 0; i < num_lights; ++i)
			{
				Light *l = lights[i];
				Shadow_Map *sm = l->shadowmap;

				glBindFramebuffer(GL_FRAMEBUFFER, sm->fbo);
				sm->shader->bind();
				// everything except the orthogonal projection matrix should be uploaded
				glUniformMatrix4fv(smshader_translation_matrix_uniform_id, 			1, GL_FALSE, &(translation_matrix[0]));
				glUniformMatrix4fv(smshader_projection_matrix_uniform_id, 			1, GL_FALSE, &(projection[0]));
				glUniformMatrix4fv(smshader_rotation_matrix_uniform_id, 			1, GL_FALSE, &(rotation_matrix[0]));
				glUniformMatrix4fv(smshader_camera_translation_matrix_uniform_id, 	1, GL_FALSE, &((*(sm->translation_matrix))[0]));
				sm->upload_rotation_matrices();

				glUniform4fv(smshader_scale_vector_uniform_id, 1, &(scale_vector[0]));
				glUniform4fv(smshader_window_to_unit_uniform_id, 1, &(window_to_unit_transform_vector[0]));

				glBindBuffer(GL_ARRAY_BUFFER, quad_normal_buffer);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(
				   1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				   3,                  // size
				   GL_FLOAT,           // type
				   GL_FALSE,           // normalized?
				   0,                  // stride
				   (void*)0            // array buffer offset
				);

				glBindBuffer(GL_ARRAY_BUFFER, quad_vertex_buffer);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(
				   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				   3,                  // size
				   GL_FLOAT,           // type
				   GL_FALSE,           // normalized?
				   0,                  // stride
				   (void*)0            // array buffer offset
				);

				glDrawArrays(GL_TRIANGLES, 0, 6*6);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);

				sm->shader->unbind();
			}
		}
	}

	math::mat4f translate_zero = math::translationMat4(math::vec4f(0.0f, 0.0f, -1.3f, 1.0f));
	math::mat4f rotate_zero = math::rotationMat4(0.0f, math::vec3f(0.0f, 0.0f, 0.0f));
	void render()
	{
		for(int rect_i = 0; rect_i < rectangles.size(); ++rect_i)
		{
			// render shadowmaps
			/*
			for(int i = 0; i < num_lights; ++i)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, lights[i]->shadowmap_framebuffer);
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				if(i == 0) glClear(GL_COLOR_BUFFER_BIT);
				math::vec4f translate = math::vec4f(-lights[i]->position.x, -lights[i]->position.y, lights[i]->position.z, 1.0f);
				translate =  translate * window_to_unit_transform_vector;
				
				translate.x += 1.0f;
				translate.y += 1.0f;
				
				camera_translation_matrix = math::translationMat4(translate);

				// up
				camera_rotation_matrix = math::rotationMat4(90, math::vec3f(0.0f, 0.0f, 0.0f));
				// render_rect(rectangles[rect_i], shadowmap_projection_matrix, camera_translation_matrix, camera_rotation_matrix, 1, 0);

				// right
				camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 3.0f));
				// render_rect(rectangles[rect_i], shadowmap_projection_matrix, camera_translation_matrix, camera_rotation_matrix, 1, 1);
				// if(i == 0) std::cout << "camera translate: " << translate.x << ", " << translate.y << ", " << translate.z << std::endl;

				// down
				camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 2.0f));
				if(i == 0) render_rect(rectangles[rect_i], shadowmap_projection_matrix, camera_translation_matrix, camera_rotation_matrix, 1, 3);
				
				// left
				camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 1.0f));
				// render_rect(rectangles[rect_i], shadowmap_projection_matrix, camera_translation_matrix, camera_rotation_matrix, 1, 3);
			}
			//*/

			/*
			for(int i = 0; i < num_lights; ++i)
			{
				glActiveTexture(GL_TEXTURE0 + lights[i]->shadowmap_texture_up);
				glBindTexture(GL_TEXTURE_2D, lights[i]->shadowmap_texture_up);
				glActiveTexture(GL_TEXTURE0 + lights[i]->shadowmap_texture_right);
				glBindTexture(GL_TEXTURE_2D, lights[i]->shadowmap_texture_right);
				glActiveTexture(GL_TEXTURE0 + lights[i]->shadowmap_texture_down);
				glBindTexture(GL_TEXTURE_2D, lights[i]->shadowmap_texture_down);
				glActiveTexture(GL_TEXTURE0 + lights[i]->shadowmap_texture_left);
				glBindTexture(GL_TEXTURE_2D, lights[i]->shadowmap_texture_left);
			}
			//*/

			math::vec4f translate = math::vec4f(-lights[2]->position.x, -lights[2]->position.y, lights[2]->position.z, 1.0f);
			translate =  translate * window_to_unit_transform_vector;
			
			translate.x += 1.0f;
			translate.y += 1.0f;
			
			camera_translation_matrix = math::translationMat4(translate);
			camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 2.0f));
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			render_rect(rectangles[rect_i], shadowmap_projection_matrix, translate_zero, rotate_zero);
			// if(rectangles[rect_i]->render_in_shadowmap == 1) render_rect(rectangles[rect_i], shadowmap_projection_matrix, camera_translation_matrix, camera_rotation_matrix, 1, 0);
			quad_shader.unbind();
		}
		SDL_GL_SwapWindow(window);
	}

	void destroy()
	{
		free(window);
	}

	namespace draw
	{
		void add_rectangle(Rectangle *rect)
		{
			::rectangles.push_back(rect);
		}

		void clear_rectangles()
		{
			::rectangles.clear();
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
			/*
			float 	low_x = p1.x, high_x = p2.x, 
					low_y = p1.y, high_y = p2.y;
			if(p2.x < p1.x) { low_x = p2.x; high_x = p1.x; };
			if(p2.y < p1.y) { low_y = p2.y; high_y = p1.y; };

			float width = high_x - low_x;
			float height = high_y - low_y;

			quad_shader.bind();

			glBindBuffer(GL_ARRAY_BUFFER, line_vertex_buffer);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
			   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);

			float scale_w = width / (WINDOW_WIDTH);
			float scale_h = height / (WINDOW_HEIGHT);
			scale_vector = math::vec4f(scale_w, scale_h, 1.0, 1.0);
			translation_matrix = math::translationMat4(math::vec4f(2 * low_x / (WINDOW_WIDTH) - 1 + scale_w, 2 * low_y / (WINDOW_HEIGHT) - 1 + scale_h, 0.0f, 1.0f));

			glUniformMatrix4fv(quad_translation_matrix_uniform_id, 1, GL_FALSE, &(translation_matrix[0]));
			glUniformMatrix4fv(quad_projection_matrix_uniform_id, 1, GL_FALSE, &(projection_matrix[0]));
			glUniformMatrix4fv(quad_rotation_matrix_uniform_id, 1, GL_FALSE, &(rotation_matrix[0]));
			glUniform4fv(quad_scale_vector_uniform_id, 1, &(scale_vector[0]));
			glUniform4fv(quad_color_vector_uniform_id, 1, &(color[0]));
			 
			glDrawArrays(GL_LINES, 0, 2);
			glDisableVertexAttribArray(0);
			quad_shader.unbind();
			*/
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