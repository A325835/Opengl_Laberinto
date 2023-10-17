#include <iostream>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.h"
#include "Cubo.h"
#include "Camera.h"
#include "Texture.h"
#include "stb_image.h"

using namespace std;

const unsigned width = 800;
const unsigned height = 600;


unsigned int VBO, VAO, EBO;
//VAO light
unsigned int VAO_L;

//Agregamos las variables que vamos a usar para la textura
unsigned int diffuseMap;
unsigned int specularMap;


vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

mat4 projection;
mat4 view;

void initGLFWVersion();
bool gladLoad();
void updateWindow(GLFWwindow* window, Shader ourShader, Shader ourLight, Texture outTexture);

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window);
void CameraInput(GLFWwindow* window);
void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void GeneracionBuffer();
void VertexAttribute(int layout, int data, int total, int start);
void TransformCubo(Shader ourShader);
void TransformCamera(Shader ourShader);

void TransformCuboLight(Shader ourLight);
void CameraUniform(Shader shaderName);




int main()
{
	initGLFWVersion();

	// Creación de ventana
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		cout << "Falla al cargar ventana" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoad())
	{
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetCursorPosCallback(window, Mouse_callback);
	glfwSetScrollCallback(window, Scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("vertexShader.vs", "fragmentShader.fs");

	// Shader de Luz
	Shader ourLight("vertexLight.vl", "fragmentLight.fl");

	// Clase Texture
	Texture outTexture;

	GeneracionBuffer();

	 diffuseMap = outTexture.loadTextureID("caja.png", 2);
	 specularMap = outTexture.loadTextureID("caja_especular.png", 2);

	


	updateWindow(window, ourShader, ourLight, outTexture);

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAO_L);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void initGLFWVersion()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__
}
bool gladLoad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Falla al cargar glad" << endl;
		return false;
	}
	return true;
}
void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	CameraInput(window);
}
void CameraInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void Mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = xposIn;
	float ypos = yposIn;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void updateWindow(GLFWwindow* window, Shader ourShader, Shader ourLight, Texture outTexture)
{
	while (!glfwWindowShouldClose(window))
	{
		float currenTime = glfwGetTime();
		deltaTime = currenTime - lastTime;
		lastTime = currenTime;

		processInput(window);

		glClearColor(0.6f, 0.8f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Se mandan las variables al header de texture con sus datos
		outTexture.ViewTextureMap(0, diffuseMap);
		outTexture.ViewTextureMap(1, specularMap);

		ourShader.use();

		//mandamos las variables para la luz de las texturas junto con su material
		ourShader.setVec3("viewPos", camera.Position);

		ourShader.setFloat("material.shininess", 64.0f);
		// ourShader.setInt("material.diffuse", 0);
		// ourShader.setInt("material.specular", 1);

		ourShader.setVec3("light.position", posCubeLight[0]);
		ourShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		ourShader.setVec3("light.diffuse", 0.55f, 0.5f, 0.5f);
		ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		

		TransformCamera(ourShader);
		TransformCubo(ourShader);

		//light
		ourLight.use();

		CameraUniform(ourLight);
		TransformCuboLight(ourLight);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void GeneracionBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//vertices
	VertexAttribute(0, 3, 8, 0);
	//Normales
	VertexAttribute(1, 3, 8, 3);
	//Texturas solo ocupamos 2 datos para la textura
	VertexAttribute(2, 2, 8, 6);



	//light
	glGenVertexArrays(1, &VAO_L);
	glBindVertexArray(VAO_L);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//cubo de luz los vertices
	VertexAttribute(0, 3, 8, 0);
	//estos son para que no haya erores ed lectura
	VertexAttribute(1, 3, 8, 3);
	VertexAttribute(2, 2, 8, 6);

	

	/*
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);*/
}
void VertexAttribute(int layout, int data, int total, int start)
{
	glVertexAttribPointer(layout, data, GL_FLOAT, GL_FALSE, total * sizeof(float), (void*)(start * sizeof(float)));
	glEnableVertexAttribArray(layout);
}

void TransformCubo(Shader ourShader)
{
	int tam = sizeof(posCube) / sizeof(posCube[0]);
	glBindVertexArray(VAO);
	for (int i = 0; i < tam; i++)
	{
		mat4 modelo = mat4(1.0f);
		modelo = translate(modelo, posCube[i]);
		modelo = rotate(modelo, radians(-45.0f), vec3(0.3f, 0.7f, 0.0f));
		ourShader.setMat4("model", modelo);
		glDrawElements(GL_TRIANGLES, 64, GL_UNSIGNED_INT, 0);
	}
}

void TransformCamera(Shader ourShader)
{
	projection = perspective(radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	CameraUniform(ourShader);
}

void CameraUniform(Shader shaderName)
{
	shaderName.setMat4("projection", projection);
	shaderName.setMat4("view", view);
}

void TransformCuboLight(Shader ourLight)
{
	int tam = sizeof(posCubeLight) / sizeof(posCubeLight[0]);
	glBindVertexArray(VAO_L);
	for (int i = 0; i < tam; i++)
	{
		mat4 modelo = mat4(1.0f);
		modelo = translate(modelo, posCubeLight[i]);
		modelo = scale(modelo, vec3(0.2f));
		ourLight.setMat4("model", modelo);
		glDrawElements(GL_TRIANGLES, 64, GL_UNSIGNED_INT, 0);
	}
}
