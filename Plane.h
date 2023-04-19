#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "shader.h"

class Plane
{
public: 
	Plane() { }

	/// A plane on the XZ axis. Scale is uniform along both.
	Plane(Shader& shader, glm::vec3 position, glm::vec3 scale) 
	{
		m_position = position;
		m_shader = &shader;
		m_scale = scale;

		float vertices[] = {
			// positions          // normals           
			-0.5f, 0.0f,-0.5f,  0.0f,  0.0f, -1.0f,  
			 0.5f, 0.0f,-0.5f,  0.0f,  0.0f, -1.0f,   
			 0.5f, 0.0f, 0.5f,  0.0f,  0.0f, -1.0f,   
			 0.5f, 0.0f, 0.5f,  0.0f,  0.0f, -1.0f,   
			-0.5f, 0.0f, 0.5f,  0.0f,  0.0f, -1.0f, 
			-0.5f, 0.0f,-0.5f,  0.0f,  0.0f, -1.0f,   
		};

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(m_VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1); 
	} 

	void Draw()
	{
		m_shader->use();
		glBindVertexArray(m_VAO); 
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
		model = glm::scale(model, m_scale); 
		m_shader->setMat4("model", model);
		 
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

protected:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	GLuint m_VAO, m_VBO;
	Shader* m_shader;
};