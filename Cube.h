#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "shader.h"

/// The cube is rendered with two textures: one for the sides, and one for the top and bottom faces.
/// Walls and the roof of buildings are made distinct this way.
/// Its side faces are defined by the first 24 vertices in the vertex buffer.
/// The top and bottom are the last 12. Based on this, the two parts are drawn separately, using textures bound to slots 0 and 1.

class Cube
{
public: 
	Cube() { }

	/// textureScaleX and textureScaleY control the texture coordinates' scale across the cube's faces. 
	/// When creating cubes in larger scales, these can be used so that the texture is not stretched in a 
	/// non-uniform manner.
	/// rotationY is an angle in degrees.

	Cube(Shader& shader, glm::vec3 position, float rotationY, glm::vec3 scale, float textureScaleX, float textureScaleY)
	{
		m_position = position;
		m_shader = &shader;
		m_rotationY = rotationY;
		m_scale = scale;

		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f * textureScaleY,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f * textureScaleX,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f * textureScaleX,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f * textureScaleY,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f * textureScaleY,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f * textureScaleX,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f * textureScaleX,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f * textureScaleY,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f * textureScaleX,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f * textureScaleY,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f * textureScaleX,  0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f * textureScaleY,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

			 // Bottom face
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f * textureScaleY,  
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f * textureScaleX,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f * textureScaleX,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f * textureScaleY,

			// Top face
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f * textureScaleY,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f * textureScaleX,  1.0f * textureScaleY,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f * textureScaleX,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f * textureScaleX,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f * textureScaleY
		};

		glGenBuffers(1, &m_VBO);
		glGenVertexArrays(1, &m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(m_VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	 

	void Draw()
	{
		m_shader->use();
		glBindVertexArray(m_VAO); 
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
		model = glm::rotate(model, glm::radians(m_rotationY), glm::vec3(0, 1, 0));
		model = glm::scale(model, m_scale);
		m_shader->setMat4("model", model);
		 
		glDrawArrays(GL_TRIANGLES, 0, 24);

		/// Draw the roof 
		m_shader->setInt("material.diffuse", 1);
		m_shader->setInt("material.specular", 1);
		glDrawArrays(GL_TRIANGLES, 24, 12);
	}

protected:
	glm::vec3 m_position;
	float m_rotationY;
	glm::vec3 m_scale;
	GLuint m_VAO, m_VBO;
	Shader* m_shader;
};