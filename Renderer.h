#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Object.h"
#include "Shader.h"

#define MAX_OBJECTS 1024
#define MAX_SHADER 1024

class Renderer
{
public:
	Renderer();
	void append_render_list(Object& object);
	void render_render_list();


	

private:
	void append_shader_list(const char* vertex_shader, const char* fragment_shader);


	Object* render_list[MAX_OBJECTS] = {};
	unsigned int render_list_length = 0;

	Shader* shader_list[MAX_SHADER] = {};
	unsigned int shader_list_length = 0;


	GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0, uniformTexture = 0, uniformEyePosition,
		uniformSpecularIntensity = 0, uniformShininess = 0;
};

