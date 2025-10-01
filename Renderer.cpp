#include "Renderer.h"

//Renderer::Renderer()
//{
//	append_shader_list("Shaders/shader.vert", "Shaders/shader.frag");
//}
//
//void Renderer::append_render_list(Object& object)
//{
//	if (render_list_length >= MAX_OBJECTS) {
//		printf("append render list failed\n");
//		return;
//	}
//	render_list[++render_list_length] = &object;
//}
//
//void Renderer::render_render_list()
//{
//	uniformModel = shader_list[0]->GetModelLocation();
//	uniformProjection = shader_list[0]->GetProjectionLocation();
//	uniformView = shader_list[0]->GetViewLocation();
//	uniformTexture = shader_list[0]->GetTextureLocation();
//
//	uniformEyePosition = shader_list[0]->GetEyePositionLocation();
//	uniformSpecularIntensity = shader_list[0]->GetSpecularIntensityLocation();
//	uniformShininess = shader_list[0]->GetShininessLocation();
//
//	for (int i = 0; i < render_list_length; ++i) {
//
//		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(render_list[i]->get_model_matrix()));
//		glUniform1i(uniformTexture, 0);
//		render_list[i]->get_material().UseMaterial(uniformSpecularIntensity, uniformShininess);
//	}
//}
//
//void Renderer::append_shader_list(const char* vertex_shader, const char* fragment_shader)
//{
//	Shader* shader = new Shader();
//	shader->CreateFromFiles(vertex_shader, fragment_shader);
//	shader_list[++shader_list_length] = shader;
//}
