#pragma once


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Material.h"
#include "Model.h"


class Object
{
public:
	Object(Material& material, Model& model);

	void set_model_matrix(glm::vec3 translate, GLfloat rotate_angle, glm::vec3 rotate_axis, glm::vec3 scale);
	glm::mat4& get_model_matrix();
	void set_material(Material& material);
	Material* get_material();
	void set_model(Model& model);
	Model& get_model();

	void render_this();

private:
	const GLfloat toRadians = 3.14159265f / 180.0f;
	glm::mat4 model_matrix = glm::identity<glm::mat4>();
	Material& material;
	Model& model;
};

