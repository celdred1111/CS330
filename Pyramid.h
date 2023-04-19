#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "shader.h"

class Pyramid
{
public:
	Pyramid() { }
	Pyramid(Shader& shader, glm::vec3 position, float rotationY, glm::vec3 scale)
	{
		m_position = position;
		m_shader = &shader;
		m_rotationY = rotationY;
		m_scale = scale;

		float vertices[] = {
			// position        normal 
			
			// Front face
			0.0f, 0.5f, 0.0f,    0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -1.0f,
			0.5f, -0.5f, 0.5f,   0.0f, 0.0f, -1.0f,

			// Back face
			0.0f, 0.5f, 0.0f,    0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,

			// Left face
			0.0f, 0.5f, 0.0f,    -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,

			// Right face
			0.0f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

			// We do not need a bottom face.
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

		glDrawArrays(GL_TRIANGLES, 0, 12);
	}

protected:
	glm::vec3 m_position;
	float m_rotationY;
	glm::vec3 m_scale;
	GLuint m_VAO, m_VBO;
	Shader* m_shader;
};