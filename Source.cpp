#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

/// Models
#include "Plane.h"
#include "Cube.h"
#include "Pyramid.h"
#include "Torus.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0, 15.0f, 35.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float cameraSpeed = 2.0f;
bool perspectiveProjection = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 10.0f, 2.0f);

Shader lightingShader;
Shader lightingShaderColor;
unsigned int diffuseMapBuildingWall;  
unsigned int diffuseMapBuildingRoof; 
unsigned int diffuseMapStadium;

/// Models used in the scene
Plane ground;
Cube businessCentre, businessCentre2;
Cube tower1, tower2;
Pyramid pyramidTower1;
Cube stadiumBottom;
Torus stadiumTop;

void setupScene()
{
	/// Load the shaders.
	/// Then, set up models 
	/// Then, load the textures. 

	lightingShader = Shader("shaderfiles/multiple_lights.vs", "shaderfiles/multiple_lights.fs");
	lightingShaderColor = Shader("shaderfiles/multiple_lights_color.vs", "shaderfiles/multiple_lights_color.fs");

	/// Ground, a plane where everything sits on.

	ground = Plane(lightingShaderColor, glm::vec3(0), glm::vec3(100, 100, 100));

	/// Tiny value to add between things, so that two faces do not occupy the same space; this is done to prevent Z-fighting.
	const float PADDING = 0.005f;
	 
	/// Business centre

	{
		const glm::vec3 position(-16.0f, 0.0f, 0.0f);
		const float width = 3.0f;
		const float height = 3.0f;
		businessCentre = Cube(lightingShader, position + glm::vec3(width + PADDING, height/2.0f + PADDING, 0.0f), 0.0f, glm::vec3(width, height, width), 1.0f, 1.0f);
		businessCentre2 = Cube(lightingShader, position + glm::vec3(0.0f, height/2.0f + PADDING, 0.0f), 90.0f, glm::vec3(3 * width, height, width), 1.0f, 1.0f);
	}

	/// Stadium

	{
		const glm::vec3 position(0.0f, 0.0f, 0.0f);
		const float width = 8.0f;
		//const float height = 1.67f;
		const float height = 2.3f;
		const float topHeight = 1.0f;
		stadiumBottom = Cube(lightingShader, position + glm::vec3(0.0f, height / 2 + PADDING, 0.0f), 0.0f, glm::vec3(width * 1.50f, height, width), 1.0f, 1.0f);

		stadiumTop = Torus(lightingShader, position + glm::vec3(0.0f, height  + topHeight, 0.0f), glm::vec3(1.25f, 1.0f, 1.0f), width/1.50f, topHeight*1.50f);
	}

	/// Towers

	{
		const glm::vec3 position(16.0f, 0.0f, 0.0f);
		const float width = 2.0f;
		const float height = 6.0f; const float height2 = 7.0f;
		const float pyramidWidth = 2.0f;
		const float pyramidHeight = 1.5f;
		const glm::vec3 pyramidPosition = position + glm::vec3(0, height + pyramidHeight/2.0f, 0);

		tower1 = Cube(lightingShader, position + glm::vec3(0.0f, height / 2 + PADDING, 0.0f), 0.0f, glm::vec3(width, height, width), 1.f, 4.0f);
		pyramidTower1 = Pyramid(lightingShaderColor, pyramidPosition, 0, glm::vec3(pyramidWidth, pyramidHeight, pyramidWidth));
		 
		const glm::vec3 positionTower2(14.0f, 0.0f, 8.0f);
		tower2 = Cube(lightingShader, positionTower2 + glm::vec3(0.0f, height2 / 2 + PADDING, 0.0f), 0.0f, glm::vec3(width, height2, width), 1.f, 4.0f);
	}
	 
	/// Load textures 
	diffuseMapBuildingWall = loadTexture("building_wall.jpg");
	diffuseMapBuildingRoof = loadTexture("building_roof.jpg");
	diffuseMapStadium = loadTexture("stadium.jpg");
}

void setShaderVariables(Shader& shader)
{
	// be sure to activate shader when setting uniforms/drawing objects
	shader.use();
	shader.setVec3("viewPos", camera.Position);

	/*
	   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
	   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
	   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
	   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
	*/
	// directional light
	shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader.setVec3("dirLight.ambient", 0.75f, 0.75f, 0.75f);
	shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

	// point light 1
	shader.setVec3("pointLights[0].position", glm::vec3(0.0f, 7.0f, 0.0f));
	shader.setVec3("pointLights[0].ambient", 0.01f, 0.01f, 0.5f);
	shader.setVec3("pointLights[0].diffuse", 0.01f, 0.01f, 0.5f);
	shader.setVec3("pointLights[0].specular", 0.01f, 0.01f, 0.5f);
	shader.setFloat("pointLights[0].constant", 0.003f);
	shader.setFloat("pointLights[0].linear", 0.007f);
	shader.setFloat("pointLights[0].quadratic", 0.0027f);

	// spotLight
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 0.4f, 0.4f, 0.4f);
	shader.setVec3("spotLight.specular", 0.4f, 0.4f, 0.4f);
	shader.setFloat("spotLight.constant", 0.5f);
	shader.setFloat("spotLight.linear", 0.007f);
	shader.setFloat("spotLight.quadratic", 0.011f);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	// view/projection transformations
	glm::mat4 projection;

	if (perspectiveProjection) 
	{
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	}
	else 
	{
		projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);
	}

	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	 
	lightingShaderColor.setFloat("material.shininess", 32.0f);
}

/// Sets both the diffuse and specular uniforms of the color shader to the same value.
void setShaderColor(glm::vec3 color)
{
	lightingShaderColor.setVec3("material.diffuse", color);
	lightingShaderColor.setVec3("material.specular", color);
}

/// Sets both the diffuse and specular uniforms of the texture shader to the same value.
void setShaderTexture(int samplerValue)
{
	lightingShaderColor.setInt("material.diffuse", samplerValue);
	lightingShaderColor.setInt("material.specular", samplerValue);
}

void drawScene()
{  
	/// Non-textured models -- ground and pyramid at the top
	/// ------------------------------
	
	setShaderVariables(lightingShaderColor);
	lightingShaderColor.setFloat("material.shininess", 32.0f);

	/// Render the ground   
	setShaderColor(glm::vec3(0.21f, 0.21f, 0.21f));

	ground.Draw();
	 
	setShaderColor(glm::vec3(0.25f, 0, 0.0f));
	pyramidTower1.Draw();
	 
	/// Textured models -- business centre, stadium, towers
	/// ------------------------------
	
	/// Buildings

	/// Textures -- 0 is the wall, 1 is the roof texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMapBuildingWall); 
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, diffuseMapBuildingRoof);

	setShaderVariables(lightingShader);
	
	/// Draw the business centre 
	 
	setShaderTexture(0);
	businessCentre.Draw(); 
	setShaderTexture(0);
	businessCentre2.Draw();

	/// Render the towers

	setShaderTexture(0);
	tower1.Draw();


	setShaderTexture(0);
	tower2.Draw();
	  
	/// Draw the stadium

	setShaderTexture(1);
	stadiumBottom.Draw();

	/// Load the stadium's texture into the slot 0, then use it to draw the torus.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMapStadium);
	setShaderTexture(0);
	stadiumTop.Draw();
}
 
int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, input_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	setupScene();
	 
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 
		drawScene();
		  
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	 
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);  
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UPWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWNWARD, deltaTime); 
}

void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{ 
		perspectiveProjection = !perspectiveProjection;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraSpeed += (float) yoffset;
	
	if (cameraSpeed < 0.5f)
		cameraSpeed = 0.5f;

	camera.MovementSpeed = cameraSpeed; 
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}