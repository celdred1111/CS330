#pragma once
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include "shader.h"

#include <vector>
using namespace std;

class Torus
{
public:
    static const int mainSegments = 16;
    static const int tubeSegments = 16;
    
    Torus() { } 
	Torus(Shader& shader, glm::vec3 position, glm::vec3 scale, float mainRadius, float tubeRadius)
	{
		m_position = position;
		m_shader = &shader;
        m_scale = scale;

        int numVertices = (mainSegments + 1) * (tubeSegments + 1);
        int primitiveRestartIndex = numVertices;
        int numIndices = (mainSegments * 2 * (tubeSegments + 1)) + mainSegments - 1;

        m_numIndices = numIndices;
        m_primitiveRestartIndex = primitiveRestartIndex;

        vector<float> vertices;
        vertices.reserve(numVertices * 8); // 3, 3, 2 -- position, normal, tex coords.

        vector<GLuint> indices;
        indices.reserve(numIndices);

        // Generate VAO and VBOs for vertex attributes and indices
        glGenVertexArrays(1, &m_VAO); 
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_VEO);
           
        // Precalculate steps in radians for main segment and tube segment
        float mainSegmentAngleStep = glm::radians(360.0f / static_cast<float>(mainSegments));
        float tubeSegmentAngleStep = glm::radians(360.0f / static_cast<float>(tubeSegments));

        // Precalculate steps in texture coordinates for main segment and tube segment 
        const float mainSegmentTextureStep = 12.0f / static_cast<float>(mainSegments);
        const float tubeSegmentTextureStep = 4.0f / static_cast<float>(tubeSegments);
          
        float currentMainSegmentAngle = 0.0f;
        float currentMainSegmentTexCoordV = 0.0f;
        for (int i = 0; i <= mainSegments; i++)
        {
            // Calculate sine and cosine of main segment angle
            const float sinMainSegment = sin(currentMainSegmentAngle);
            const float cosMainSegment = cos(currentMainSegmentAngle);
            float currentTubeSegmentAngle = 0.0f;
            float currentTubeSegmentTexCoordU = 0.0f;
            for (int j = 0; j <= tubeSegments; j++)
            {
                // Calculate sine and cosine of tube segment angle
                const float sinTubeSegment = sin(currentTubeSegmentAngle);
                const float cosTubeSegment = cos(currentTubeSegmentAngle);

                // Calculate vertex position on the surface of torus
                glm::vec3 surfacePosition = glm::vec3(
                    (mainRadius + tubeRadius * cosTubeSegment) * cosMainSegment,
                    tubeRadius * sinTubeSegment,
                    (mainRadius + tubeRadius * cosTubeSegment) * sinMainSegment);

                vertices.push_back(surfacePosition[0]);
                vertices.push_back(surfacePosition[1]);
                vertices.push_back(surfacePosition[2]);
                 
                // Normals
                glm::vec3 normal = glm::vec3(
                    cosMainSegment * cosTubeSegment,
                    sinMainSegment * cosTubeSegment,
                    sinTubeSegment
                );

                vertices.push_back(normal[0]);
                vertices.push_back(normal[1]);
                vertices.push_back(normal[2]);

                // Texture coordinates
                glm::vec2 textureCoordinate = glm::vec2(currentTubeSegmentTexCoordU, currentMainSegmentTexCoordV);

                vertices.push_back(textureCoordinate[0]);
                vertices.push_back(textureCoordinate[1]);
                 
                // Update current tube angle
                currentTubeSegmentAngle += tubeSegmentAngleStep;
                currentTubeSegmentTexCoordU += tubeSegmentTextureStep;
            }

            // Update main segment angle
            currentMainSegmentAngle += mainSegmentAngleStep;
            // Update texture coordinate of main segment
            currentMainSegmentTexCoordV += mainSegmentTextureStep;
        } 
         

        // Finally, generate indices for rendering
        GLuint currentVertexOffset = 0;
        for (int i = 0; i < mainSegments; i++)
        {
            for (int j = 0; j <= tubeSegments; j++)
            {
                GLuint vertexIndexA = currentVertexOffset;  
                GLuint vertexIndexB = currentVertexOffset + tubeSegments + 1; 

                indices.push_back(vertexIndexA);
                indices.push_back(vertexIndexB);
                 
                currentVertexOffset++;
            }

            // Don't restart primitive, if it's last segment, rendering ends here anyway
            if (i != mainSegments - 1) { 
                indices.push_back(primitiveRestartIndex);
            }
        }
          
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * 8, vertices.data(), GL_STATIC_DRAW);

        glBindVertexArray(m_VAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VEO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW); 
	}
     
	void Draw()
	{
        m_shader->use();
		glBindVertexArray(m_VAO); 
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
        model = glm::scale(model, m_scale);
		m_shader->setMat4("model", model);
          
        // Enable primitive restart, because we're rendering several triangle strips (for each main segment)
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(m_primitiveRestartIndex);

        // Render torus using precalculated indices
        glDrawElements(GL_TRIANGLE_STRIP, m_numIndices, GL_UNSIGNED_INT, 0);

        // Disable primitive restart, we won't need it now
        glDisable(GL_PRIMITIVE_RESTART);
	}

protected:
	glm::vec3 m_position;
    glm::vec3 m_scale;
	GLuint m_VAO, m_VBO, m_VEO;
    int m_numIndices, m_primitiveRestartIndex;
	Shader* m_shader;
};