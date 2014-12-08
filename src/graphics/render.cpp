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
GLuint shadow_poly_buffer;

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

GLfloat *shadow_poly_data;

float lightpositions[MAX_LIGHTS * 3];
float lightcolors[MAX_LIGHTS * 3];
float lightranges[MAX_LIGHTS];
int renderlights[MAX_LIGHTS];
GLuint shadowmaps[MAX_LIGHTS];
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
GLuint quad_near_far_vector_uniform_id;

/*********************************************/

std::vector<math::mat2f *> light_block_rects;
std::vector<graphics::Rectangle *> rectangles;

char lightblock_map[(MAP_WIDTH) * (MAP_HEIGHT)];

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

		glGenFramebuffers(1, &(l->shadowmap_framebuffer));
		glBindFramebuffer(GL_FRAMEBUFFER, l->shadowmap_framebuffer);

		/*
		glGenRenderbuffers(1, &(l->shadowmap_texture_up));
		glBindRenderbuffer(GL_RENDERBUFFER, l->shadowmap_texture_up);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, l->shadowmap_texture_up);
		*/

		glGenTextures(1, &(l->shadowmap_texture_up));
 
		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, l->shadowmap_texture_up);
		 
		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		
		// Poor filtering. Needed !
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Set "renderedTexture" as our colour attachement #0
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, l->shadowmap_texture_up, 0);
		 
		// Set the list of draw buffers.
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("shit\n");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void add_light_block_rect(math::mat2f *rect)
	{
		// std::cout << "add light block " << light_block_rects.size() << std::endl;
		if(rect)
			light_block_rects.push_back(rect);

		for(int i = 0; i < light_block_rects.size(); ++i)
		{
			light_block_rects_uniform_array[i] = &((*rect)[0]);
		}

		quad_shader.bind();
		glUniformMatrix2fv(light_block_rects_uniform_array_id, light_block_rects.size(), GL_FALSE, light_block_rects_uniform_array[0]);
		glUniform1i(num_light_block_rects_uniform_id, light_block_rects.size());

		std::cout << "wtf" << std::endl;
	}

	void add_light_block(int x, int y, char t)
	{
		lightblock_map[y * MAP_WIDTH + x] = t;
		// printf("added light at (%i, %i)\n", x, y);
	}

	void clear_light_block_rects()
	{
		light_block_rects.clear();
		memset(lightblock_map, 0, MAP_WIDTH * MAP_HEIGHT);
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

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
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
		quad_shader.prepare();
		shadowmap_shader.prepare();
		if(quad_shader.compile() == -1)
		{
			return -1;
		}

		std::cout << "compiled quad shader" << std::endl;

		// if(shadowmap_shader.addGeometryShader("shaders/shadowmap.gs") == -1) return -1;
		if(shadowmap_shader.compile() == -1) return -1;

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

		shadow_poly_data = new GLfloat[360 * 2 * 3 * 3];
		glGenBuffers(1, &shadow_poly_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, shadow_poly_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shadow_poly_data), shadow_poly_data, GL_DYNAMIC_DRAW);

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
		float n = 0.05f;
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
		quad_num_depthbuffer_int_uniform_id = 		quad_shader.getUniformLocation("num_buffer");
		quad_shadowmaps_gluintarray_uniform_id = 	quad_shader.getUniformLocation("shadowmaps");

		std::cout << "ID: " << quad_shadowmaps_gluintarray_uniform_id << std::endl;

		light_block_rects_uniform_array_id = 		quad_shader.getUniformLocation("lightblock_rects");
		num_light_block_rects_uniform_id = 			quad_shader.getUniformLocation("num_lightblock_rects");
		quad_near_far_vector_uniform_id = 			quad_shader.getUniformLocation("near_far");

		smshader_translation_matrix_uniform_id = 		shadowmap_shader.getUniformLocation("model_translation");
		smshader_projection_matrix_uniform_id = 		shadowmap_shader.getUniformLocation("projection");
		smshader_rotation_matrix_uniform_id	=			shadowmap_shader.getUniformLocation("model_rotation");
		smshader_scale_vector_uniform_id = 				shadowmap_shader.getUniformLocation("scale");
		smshader_window_to_unit_uniform_id =			shadowmap_shader.getUniformLocation("window_to_unit");
		smshader_camera_translation_matrix_uniform_id = shadowmap_shader.getUniformLocation("camera_translation");
		smshader_near_far_vector_uniform_id = 			shadowmap_shader.getUniformLocation("near_far");


		GLuint smshader_camera_rotation_matrix_up_id = 		shadowmap_shader.getUniformLocation("shadowmap_rotation_matrices[0]");
		GLuint smshader_camera_rotation_matrix_right_id = 	shadowmap_shader.getUniformLocation("shadowmap_rotation_matrices[1]");
		GLuint smshader_camera_rotation_matrix_down_id = 	shadowmap_shader.getUniformLocation("shadowmap_rotation_matrices[2]");
		GLuint smshader_camera_rotation_matrix_left_id = 	shadowmap_shader.getUniformLocation("shadowmap_rotation_matrices[3]");

		glEnable(GL_DEPTH_TEST);

		// pass rotation matrices to shadowmap shader
		shadowmap_shader.bind();
		camera_rotation_matrix = math::rotationMat4(90, math::vec3f(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(smshader_camera_rotation_matrix_up_id, 1, GL_FALSE, &(camera_rotation_matrix[0]));

		camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 3.0f));
		glUniformMatrix4fv(smshader_camera_rotation_matrix_right_id, 1, GL_FALSE, &(camera_rotation_matrix[0]));

		camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 2.0f));
		glUniformMatrix4fv(smshader_camera_rotation_matrix_down_id, 1, GL_FALSE, &(camera_rotation_matrix[0]));

		camera_rotation_matrix = math::rotationMat4(90, math::vec3f(1.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(smshader_camera_rotation_matrix_left_id, 1, GL_FALSE, &(camera_rotation_matrix[0]));
		shadowmap_shader.unbind();
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

		float scale_w = (w-1) / (WINDOW_WIDTH);
		float scale_h = (h-1) / (WINDOW_HEIGHT);
		scale_vector = math::vec4f(scale_w, scale_h, height, 1.0);
		translation_matrix = math::translationMat4(math::vec4f(2 * x / (WINDOW_WIDTH) - 1 + scale_w, 2 * (y+1) / (WINDOW_HEIGHT) - 1 + scale_h, -height, 1.0f));

		/* assume quad_shader is being used */
		if(1)
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

			for(int i = 0; i < num_lights; ++i)
			{
				glUniform1ui(quad_shadowmaps_gluintarray_uniform_id + i, i);
			}

			glUniform2fv(quad_near_far_vector_uniform_id, 1, &(near_far_vector[0]));

			GLuint do_shadowmap_id = quad_shader.getUniformLocation("do_shadowmap");
			glUniform1i(do_shadowmap_id, 0);
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
	}

	math::mat4f translate_zero = math::translationMat4(math::vec4f(0.0f, 0.0f, -1.3f, 1.0f));
	math::mat4f rotate_zero = math::rotationMat4(0.0f, math::vec3f(0.0f, 0.0f, 0.0f));
	void render()
	{
		for(int rect_i = 0; rect_i < rectangles.size(); ++rect_i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			render_rect(rectangles[rect_i], projection_matrix, translate_zero, rotate_zero, 0, 0);
		}

		for(int lights_i = 0; lights_i < num_lights; ++lights_i)
		{
			math::vec2f middle(lights[lights_i]->position.x, lights[lights_i]->position.y);

			float prevx, prevy;

			for(float angle = -1; angle < 360 * 2; ++angle)
			{
				// track line
				float curx = middle.x;
				float cury = middle.y;

				// create heading vector
				math::vec2f heading(cos(angle / 2 * DEGTORAD), sin(angle / 2* DEGTORAD));

				float dx = heading.x > 0 ? heading.x - middle.x : middle.x - heading.x;
				float dy = heading.y > 0 ? heading.y - middle.y : middle.y - heading.y;

				// printf("(%f, %f, %f)\n", heading.x, heading.y, dy);

				float len = sqrt(dx*dx + dy*dy);
				float movx = dx / len;
				float movy = dy / len;

				//*
				// * 4 to make sure it's long enough
				for(int i = 0; i < len * 5; ++i)
				{
					// check for intersections and whatnot
					int mapx = floor(curx / MAP_TILE_SIZE);
					int mapy = floor(cury / MAP_TILE_SIZE);
					// if(x == 0) printf("curpos = (%f, %f) -> (%i, %i)\n", curx, cury, mapx, mapy);


					char lightblock = lightblock_map[(mapy * MAP_WIDTH + mapx)];
					if(lightblock != 0 ||
						curx < 0 || curx > WINDOW_WIDTH || cury < 0 || cury > WINDOW_HEIGHT)
					{
						// move curpos to edge of tile
						//*
						char done = 0;
						for(int _i = 0; done == 0; ++_i)
						{
							curx += heading.x / 10;
							cury += heading.y / 10;

							int _mapx = floor(curx / MAP_TILE_SIZE);
							int _mapy = floor(cury / MAP_TILE_SIZE);

							if(lightblock_map[(_mapy * MAP_WIDTH + _mapx)] != lightblock)
								done = 1;
						}
						//*/

						if(angle > -1)
						{
							// add point to polygon
							shadow_poly_data[(int) angle * 3 * 3 + 0] = middle.x;
							shadow_poly_data[(int) angle * 3 * 3 + 1] = middle.y;
							shadow_poly_data[(int) angle * 3 * 3 + 2] = 0.0f;

							shadow_poly_data[(int) angle * 3 * 3 + 3] = prevx;
							shadow_poly_data[(int) angle * 3 * 3 + 4] = prevy;
							shadow_poly_data[(int) angle * 3 * 3 + 5] = 0.0f;

							shadow_poly_data[(int) angle * 3 * 3 + 6] = curx;
							shadow_poly_data[(int) angle * 3 * 3 + 7] = cury;
							shadow_poly_data[(int) angle * 3 * 3 + 8] = 0.0f;
						}
						//*/

						prevx = curx;
						prevy = cury;

						// break loop
						// printf("found block at (%i, %i)\n", mapx, mapy);
						i = len  * 5;
					}

					curx += heading.x;
					cury += heading.y;
				}
				//*/

				math::vec4f color(lights[lights_i]->color.x, lights[lights_i]->color.y, lights[lights_i]->color.z, 0.5f);
				// graphics::draw::line(math::vec2f(middle.x, middle.y), math::vec2f(curx, cury), color, 2);
			}

			//*
			math::vec4f color(1.0f, 1.0f, 1.0f, 1.0f);

			quad_shader.bind();
			glBindFramebuffer(GL_FRAMEBUFFER, lights[lights_i]->shadowmap_framebuffer);
			glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

			glBindBuffer(GL_ARRAY_BUFFER, shadow_poly_buffer);
			glBufferData(GL_ARRAY_BUFFER, 360*2*3*3*8, shadow_poly_data, GL_DYNAMIC_DRAW);

			scale_vector = math::vec4f(2.0f / (WINDOW_WIDTH), 2.0f / (WINDOW_HEIGHT), 1.0f, 1.0f);
			translation_matrix = math::translationMat4(math::vec4f(-1.0f, -1.0f, 4.0f, 1.0f));

			glUniformMatrix4fv(quad_translation_matrix_uniform_id, 			1, GL_FALSE, &(translation_matrix[0]));
			glUniformMatrix4fv(quad_projection_matrix_uniform_id, 			1, GL_FALSE, &(projection_matrix[0]));
			glUniformMatrix4fv(quad_rotation_matrix_uniform_id, 			1, GL_FALSE, &(rotate_zero[0]));

			glUniform4fv(quad_scale_vector_uniform_id, 1, &(scale_vector[0]));
			glUniform4fv(quad_color_vector_uniform_id, 1, &(color[0]));
			glUniform4fv(quad_window_to_unit_uniform_id, 1, &(window_to_unit_transform_vector[0]));

			GLuint do_shadowmap_id = quad_shader.getUniformLocation("do_shadowmap");
			glUniform1i(do_shadowmap_id, 1);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
			   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			   3,                  // size
			   GL_FLOAT,           // type
			   GL_FALSE,           // normalized?
			   0,                  // stride
			   (void*)0            // array buffer offset
			);

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, 360*2*3);
			glDisableVertexAttribArray(0);
			quad_shader.unbind();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			quad_shader.bind();
			glActiveTexture(GL_TEXTURE0 + lights_i);
			glBindTexture(GL_TEXTURE_2D, lights[lights_i]->shadowmap_texture_up);
			quad_shader.unbind();
			shadowmaps[lights_i] = lights_i;
			//*/
		}


		/*
		glBindFramebuffer(GL_READ_FRAMEBUFFER, lights[2]->shadowmap_framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		//*/
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
			//*
			float 	low_x = p1.x, high_x = p2.x,
					low_y = p1.y, high_y = p2.y;
			if(p2.x < p1.x) { low_x = p2.x; high_x = p1.x; };
			if(p2.y < p1.y) { low_y = p2.y; high_y = p1.y; };

			float width = high_x - low_x;
			float height = high_y - low_y;
			// printf("lowx: %f, highx: %f, width: %f\n", low_x, high_x, width);

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
			scale_vector = math::vec4f((high_x > p1.x ? -scale_w : scale_w), (high_y > p1.y ? -scale_h : scale_h), 1.0, 1.0);
			// scale_vector = math::vec4f(scale_w, scale_h, 1.0, 1.0);
			translation_matrix = math::translationMat4(math::vec4f(2 * low_x / (WINDOW_WIDTH) - 1 + scale_w, 2 * low_y / (WINDOW_HEIGHT) - 1 + scale_h, z_index, 1.0f));

			glUniformMatrix4fv(quad_translation_matrix_uniform_id, 1, GL_FALSE, &(translation_matrix[0]));
			glUniformMatrix4fv(quad_projection_matrix_uniform_id, 1, GL_FALSE, &(projection_matrix[0]));
			glUniformMatrix4fv(quad_rotation_matrix_uniform_id, 1, GL_FALSE, &(rotation_matrix[0]));
			glUniform4fv(quad_scale_vector_uniform_id, 1, &(scale_vector[0]));
			glUniform4fv(quad_color_vector_uniform_id, 1, &(color[0]));

			glDrawArrays(GL_LINES, 0, 2);
			glDisableVertexAttribArray(0);
			quad_shader.unbind();
			//*/
		}


		void outline(Polygon &poly, math::vec4f &color, float z_index)
		{
			/*
			glPushMatrix();
				glNormal3f(0.0f, 0.0f, 1.0f);

				glTranslatef(	poly.pos().x + poly.size().x / 2,
								poly.pos().y + poly.size().y / 2,
								z_index);

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
			*/
		}
	};
};
