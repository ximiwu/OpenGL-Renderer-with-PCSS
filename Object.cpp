#include "Object.h"

Object::Object(Material& material, Model& model) : material(material), model(model)
{
}

void Object::set_model_matrix(glm::vec3 translate, GLfloat rotate_angle, glm::vec3 rotate_axis, glm::vec3 scale)
{
    model_matrix = glm::identity<glm::mat4>();
    model_matrix = glm::translate(model_matrix, translate);
    model_matrix = glm::rotate(model_matrix, rotate_angle * toRadians, rotate_axis);
    model_matrix = glm::scale(model_matrix, scale);
}

glm::mat4& Object::get_model_matrix()
{
    return model_matrix;
}

void Object::set_material(Material& material)
{
    this->material = material;
}

Material* Object::get_material()
{
    return &material;
}

void Object::set_model(Model& model)
{
    this->model = model;
}

Model& Object::get_model()
{
    return model;
}


void Object::render_this()
{
    get_model().RenderModel();
}
