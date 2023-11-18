#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"
#include "SkyBoxVertex.h"
#include "Model.h"
#include "Texture.h"

using namespace std;
using namespace ImGui;

unsigned int VBOSKY, VAOSKY, EBOSKY;
unsigned int textureSky;

const unsigned height = 720;
const unsigned width = 1200;

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

bool draw = true;
float sizeCube = 1.0f;
float inten = 0.2f;
float clearColor[4] = { 0.6f, 0.8f, 0.4f, 1.0f };

//MECHANICS
//bool isJumping = false;
//MEHCANICS

void initGLFWVersion();
bool gladLoad();

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window);
void CameraInput(GLFWwindow* window);
void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Shader ourShaderSky, Texture1 ourTextureSky);


void GeneracionBufferSky();
void VertexAttribute(int layout, int data, int total, int start);

void TransformCubo(Shader ourShader);//se necesitan realizar cambios
void TransformCamera(Shader ourShader, bool isSky);
void CameraUniform(Shader shaderName);

void InicialicedImGUI(GLFWwindow* window);
void ImGUI();
void FinalizarImGUI();

int main()
{
	initGLFWVersion();

	//Creacion de ventana
	GLFWwindow* window = glfwCreateWindow(width, height, "Winton OPENGL", NULL, NULL);//alto, ancho, nombre, si se puede modficar
	if (window == NULL)
	{
		cout << "Falla al cargar la ventana" << endl;
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
	
	//se manda a llamar el flip, voltea la textura
	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("vertexShaderC.vs", "fragmentShaderC.fs");
	//mandamos a llamar al modelo en lugar del texture y el light ya que esto lo hace el modelo
	//mandamos la direccion del modelo obj
	//Model ourModel("Modelos/backpack/backpack.obj");
	Shader ourShaderSky("vertexShaderSky.vs", "fragmenShaderSky.fs");
	Model ourModel("Modelos/laberinto/Laberintobaseprueba.obj");
	Texture1 ourTextureSky(textureSky);

	camera.Position = vec3(0.0f, 0.0f, 0.0f);

	GeneracionBufferSky();
	ourTextureSky.GeneraTexturaSky(faces);
	ourShaderSky.use();
	ourShaderSky.setInt(ourTextureSky.UniformTextureSky(), 0);

	InicialicedImGUI(window);

	updateWindow(window, ourShader, ourModel,ourShaderSky ,ourTextureSky);

	//SKY
	glDeleteVertexArrays(1, &VAOSKY);
	glDeleteBuffers(1, &VBOSKY);
	glDeleteBuffers(1, &EBOSKY);


	glfwTerminate();
	return 0;
}

void initGLFWVersion() 
{
	//INICIALIZACION DE GLFW 
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // _APPLE_
}

bool gladLoad() 
{
	//VERIGFICAR GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "falla al cargar glad" << endl;
		return false;
	}
	return true;

}

void framebuffer_size_callback(GLFWwindow* window, int w, int h) 
{
	//Mostrar ventana
	glViewport(0, 0, w, h);//Posicion X, posicion Y, ancho, alto
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
		//cout << "W" << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		//cout << "S" << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
		//cout << "A" << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
		//cout << "D" << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		camera.LockMouse(); // Bloquea el mouse cuando se presiona la tecla "L"
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		camera.UnlockMouse(); // Desbloquea el mouse cuando se presiona la tecla "U"
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		// Inicia el salto
		camera.Jump(9.8f);
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


void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Shader ourShaderSky, Texture1 ourTextureSky)
{
	
	//CARGA DE VENTAANA CADA FRAME
	while (!glfwWindowShouldClose(window))
	{
		
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		processInput(window);

		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		NewFrame();

		camera.Update(deltaTime);

		ourShader.use();

		ourShader.setFloat("material.shininess", 64.0f);
		ourShader.setInt("material.diffuse", 0);
		ourShader.setInt("material.specular", 1);

		ourShader.setVec3("light.position", vec3(1.2, 1.0, 15.0));
		ourShader.setVec3("light.ambient", 0.5f, 0.5f, 0.5f);
		ourShader.setVec3("light.diffuse", 0.55f, 0.5f, 0.5f);
		ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		
		TransformCamera(ourShader, false);
		TransformCubo(ourShader);
		ourModel.Draw(ourShader);

		//SKY
		glDepthFunc(GL_LEQUAL);
		ourShaderSky.use();
		TransformCamera(ourShaderSky, true);
		glBindVertexArray(VAOSKY);
		ourTextureSky.ViewTextureSKY();
		glDepthFunc(GL_LESS);


		ImGUI();

		//glm::vec3 cameraPosition = camera.Position;
		//std::cout << "Posición de la cámara: (" << cameraPosition.x << ", " << cameraPosition.y << ", " << cameraPosition.z << ")" << std::endl;


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
}

void TransformCubo(Shader ourShader)//cambia
{
		mat4 modelo = mat4(1.0f);
		//se modifico 
	//modelo = translate(modelo, vec3(0.0f, 0.0f, 0.0f));
		//modelo = rotate(modelo, radians(-45.0f), vec3(0.3f, 0.7f, 0.0f));
		modelo = scale(modelo, vec3(sizeCube));
		ourShader.setMat4("model", modelo);
}

void TransformCamera(Shader ourShader, bool isSky)
{
	projection = perspective(radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);

	if (isSky)
	{
		view = mat4(mat3(camera.GetViewMatrix()));
	}
	else
	{
		view = camera.GetViewMatrix();
	}

	CameraUniform(ourShader);
}

void CameraUniform(Shader shaderName)
{
	shaderName.setMat4("projection", projection);
	shaderName.setMat4("view", view);
}

void InicialicedImGUI(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();
	(void)io;
	StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}
void ImGUI()
{
	Begin("Miku");
	Text("Controla los elementos del modelo");
	Checkbox("Show-Model", &draw);
	SliderFloat("Size", &sizeCube, 0.1f, 2.0f);
	ColorEdit4("BG", clearColor);
	Text("Unlock Cursor"); SameLine();
	TextColored(ImVec4(1, 1, 0, 1), "U"); SameLine(150);
	Text("Lock Cursor"); SameLine();
	TextColored(ImVec4(1, 1, 0, 1), "L");
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}
void FinalizarImGUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

void GeneracionBufferSky()
{
	glGenVertexArrays(1, &VAOSKY);
	glGenBuffers(1, &VBOSKY);
	glGenBuffers(1, &EBOSKY);
	glBindVertexArray(VAOSKY);

	glBindBuffer(GL_ARRAY_BUFFER, VBOSKY);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOSKY);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesSky), indicesSky, GL_STATIC_DRAW);

	VertexAttribute(0, 3, 3, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void VertexAttribute(int layout, int data, int total, int start)
{
	glVertexAttribPointer(layout, data, GL_FLOAT, GL_FALSE, total * sizeof(float), (void*)(start * sizeof(float)));
	glEnableVertexAttribArray(layout);
}