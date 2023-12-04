#include <iostream>
#include "reactphysics3d/reactphysics3d.h"

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
#include "Debugger.h"
#include "variables.h"
#include "MyCollisionCallback.h"

using namespace std;
using namespace ImGui;
using namespace reactphysics3d;

unsigned int VBOSKY, VAOSKY, EBOSKY;
unsigned int VBO_N, VAO_N = 0, EBO_N;
unsigned int VAO, VBO, EBO;
unsigned int textureSky;
unsigned int specularMap;
unsigned int normalMap;
unsigned int diffuseMap;
unsigned int heightMap;

const unsigned height = 936;
const unsigned width = 1664;

vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -3.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

mat4 projection;
mat4 view;

bool isLightActive = true;
bool isFPS = true;
bool draw = true;
float sizeCube = 1.0f;
float inten = 0.2f;
float clearColor[4] = { 0.6f, 0.8f, 0.4f, 1.0f };
float lightColor[3] = { 1.0f, 1.0f, 1.0f };
float intensity = 0.05f;
float targetCutOut = 20.5f;
float currentCutOut = 20.5f;
float cutOutSpeed = 50.0f;
double lastKeyPressTime = 0.0; // Variable para almacenar el tiempo de la última presión de tecla f
MyCollisionCallback myCollisionCallback;
MyCollisionCallback colisionPiso;

//MECHANICS
//bool isJumping = false;
//MEHCANICS

void initGLFWVersion();
bool gladLoad();

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window, RigidBody* body);
void CameraInput(GLFWwindow* window);
void CameraInputFPS(GLFWwindow* window, RigidBody* body);

void Mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Shader ourShaderSky, Texture1 ourTextureSky, Texture1 outTexture);

void GeneracionBufferSky();
void VertexAttribute(int layout, int data, int total, int start);

void TransformCubo(Shader ourShader);//se necesitan realizar cambios
void TransformCamera(Shader ourShader, bool isSky);
void CameraUniform(Shader shaderName);
void LightUniform(Shader shaderName);
void ActivePointLight(Shader ourShader, float deltatime, GLFWwindow* window);
void ColliderInput(GLFWwindow* window, RigidBody* body);
//void TransformCubo2(Shader ourShader, Vector3 position, Transform interTransform);


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
	limite = (sizeof(texture) / sizeof(texture[0]));

	Shader ourShader("vertexShaderC.vs", "fragmentShaderC.fs");
	//mandamos a llamar al modelo en lugar del texture y el light ya que esto lo hace el modelo
	//mandamos la direccion del modelo obj
	//Model ourModel("Modelos/backpack/backpack.obj");
	Shader ourShaderSky("vertexShaderSky.vs", "fragmenShaderSky.fs");
	Model ourModel("Modelos/laberinto/Laberinto.obj");
	Texture1 ourTextureSky(textureSky);
	//Texture1 outTexture;
	
	//diffuseMap = outTexture.loadTextureID("Modelos/Laberinto/textures/maps_diffuse.png", 2);
	//normalMap = outTexture.loadTextureID("Modelos/laberinto/textures/maps_normal.png", 2);
	//specularMap = outTexture.loadTextureID("Modelos/laberinto/specular.png", 2);
	//heightMap = outTexture.loadTextureID("Modelos/laberinto/textures/maps_height.png", 2);
	//normalMap = outTexture.loadTextureID("Modelos/laberinto/normal.png", 2);

	Texture1 ourTexture(texture, limite);

	for (int i = 0; i < limite; i++)
	{
		ourTexture.GeneraTextura(texture[i], nombre[i], tipo[i]);
	}

	if (limite > 1)
	{
		ourShader.use();
		for (int i = 0; i < limite; i++)
		{
			ourShader.setInt(ourTexture.UniformTexture(), i);
		}
	}

	camera.Position = vec3(0.0f, 0.0f, 0.0f);
	GeneracionBufferSky();
	ourTextureSky.GeneraTexturaSky(faces);
	ourShaderSky.use();
	ourShaderSky.setInt(ourTextureSky.UniformTextureSky(), 0);

	InicialicedImGUI(window);

	updateWindow(window, ourShader, ourModel,ourShaderSky ,ourTextureSky, ourTexture);

	//SKY
	glDeleteVertexArrays(1, &VAOSKY);
	glDeleteBuffers(1, &VBOSKY);
	glDeleteBuffers(1, &EBOSKY);
	glDeleteVertexArrays(1, &VAO_N);
	glDeleteBuffers(1, &VBO_N);
	glDeleteBuffers(1, &EBO_N);


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

void processInput(GLFWwindow* window, RigidBody* body)
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
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		isFPS = true;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		isFPS = false;
	}

	if (isFPS)
	{
		CameraInputFPS(window, body);
	}
	else
	{
		CameraInput(window);
	}
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

}

void CameraInputFPS(GLFWwindow* window, RigidBody* body)
{

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		camera.ProcessKeyboardFPS(FORWARD, deltaTime);
		//body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		camera.ProcessKeyboardFPS(BACKWARD, deltaTime);
		//body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		camera.ProcessKeyboardFPS(LEFT, deltaTime);
		//body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		camera.ProcessKeyboardFPS(RIGHT, deltaTime);
		//body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	// Detectar el salto
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !camera.IsJumping) {
		// Solo permite saltar si no está ya saltando
		camera.IsJumping = true;
		// Resto de la lógica para el salto
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
}

void ColliderInput(GLFWwindow* window, RigidBody* body)
{
	Vector3 Up(0.0, 0.981, 0.0);  // Fuerza hacia arriba

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		body->setTransform(Transform(Vector3(camera.Position.x, body->getTransform().getPosition().y, camera.Position.z), Quaternion::identity()));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		body->applyLocalForceAtCenterOfMass(Up);
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


void updateWindow(GLFWwindow* window, Shader ourShader, Model ourModel, Shader ourShaderSky, Texture1 ourTextureSky, Texture1 outTexture)
{

	//Simular physicas
	float timeStep = 1.0f / 60.0f;
	double accumulator = 0.0f;
	
	Transform currTransform;

	//Crear mundo
	PhysicsCommon physicsCommon;
	PhysicsWorld* world = physicsCommon.createPhysicsWorld();

	//Creando un rigid body
	vec3 cameraPosition = camera.Position;
	Vector3 position(0.0, 0.0, 0.0);

	Quaternion orientation = Quaternion::identity();
	Transform transform(position, orientation);
	RigidBody* body = world->createRigidBody(transform);

	/*
	- STATIC: masa infinita, velocidad cero, posicion manual (ej. piso)
	- KINEMATIC: masa infinita, velocidad manual, posicion calculada (ej. plataforma)
	- DYNAMIC:  masa definidad, velocidad calculada, posicion calculada (ej. objetos o personaje)
	*/
	body->setType(BodyType::DYNAMIC);
	body->setLinearDamping(1.25);
	body->setAngularDamping(1.25);
	body->setAngularLockAxisFactor(Vector3(0.0, 1.0, 0.0));

	//Asignando un collider al rigid body
	const Vector3 halfExtents(0.5, 0.5, 0.5);
	CapsuleShape* capsuleShape = physicsCommon.createCapsuleShape(1.0, 5.0);
	Transform prevTransform = Transform::identity();
	Collider* collider;
	collider = body->addCollider(capsuleShape, transform);

	//Piso  -68.9814  -10.998 -6.54624  
	Vector3 pisoPos(-34.4907, -3.27312, -5.494);
	Quaternion pisoOrient = Quaternion::identity();
	Transform pisoTransf(pisoPos, pisoOrient);
	RigidBody* piso = world->createRigidBody(pisoTransf);

	piso->setType(BodyType::STATIC);
	Vector3 pisoHalfExt(101.0, 0.1, 101.0);
	BoxShape* pisoBox = physicsCommon.createBoxShape(pisoHalfExt);
	Collider* pisoCollider = piso->addCollider(pisoBox, pisoTransf);


	const Mesh& firstMesh = ourModel.getMesh(0);
	const std::vector<unsigned int>& indices = firstMesh.getIndices();
	const int nbVertices = firstMesh.getVertices().size();
	const int nbTriangles = indices.size() / 3;

	
	std::vector<float> vertices;
	for (const Vertex& vertex : firstMesh.getVertices()) {
		vertices.push_back(vertex.Position.x);
		vertices.push_back(vertex.Position.y);
		vertices.push_back(vertex.Position.z);
	}

	// Crear un TriangleVertexArray
	TriangleVertexArray* triangleArray = new TriangleVertexArray(
		nbVertices,
		vertices.data(),  // Obtén el puntero al primer elemento del vector
		3 * sizeof(float),
		nbTriangles,
		indices.data(),   // Obtén el puntero al primer elemento del vector
		3 * sizeof(unsigned int),
		TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
	);


	// Crear un TriangleMesh
	TriangleMesh* triangleMesh = physicsCommon.createTriangleMesh();

	// Agregar el TriangleVertexArray al TriangleMesh
	triangleMesh->addSubpart(triangleArray);

	// Crear el ConcaveMeshShape usando el TriangleMesh
	ConcaveMeshShape* concaveMesh = physicsCommon.createConcaveMeshShape(triangleMesh);

	// Configurar la transformación para el objeto
	Vector3 modelPosition(0.0, 0.0, 0.0); // Ajustar la posición según sea necesario
	Quaternion modelOrientation = Quaternion::identity(); // Ajustar la orientación según sea necesario
	Transform modelTransform(modelPosition, modelOrientation);

	// Crear un RigidBody para el objeto
	RigidBody* meshRigidBody = world->createRigidBody(modelTransform);
	meshRigidBody->setType(BodyType::STATIC);

	// Agregar un colisionador al RigidBody
	Collider* meshCollider = meshRigidBody->addCollider(concaveMesh, modelTransform);

	//Render de physicas
	Shader debugShader("debugVertexShader.vs", "debugFragmentShader.fs");
	Debugger deb(world);
	deb.enableDebugRendering();
	
	//CARGA DE VENTAANA CADA FRAME
	camera.Position = vec3(position.x, position.y, position.z);

	while (!glfwWindowShouldClose(window))
	{

		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		accumulator += deltaTime;
		world->testCollision(collider->getBody(), meshCollider->getBody(), myCollisionCallback);
		if (myCollisionCallback.areBodiesTouching) {
			cout << "Los colliders están tocándose\n";
		}
		else {
			cout << "Los colliders no están tocándose\n";
			processInput(window, body);
			ColliderInput(window,body);
		}

		// Limpia el estado del CollisionCallback para la siguiente iteración
		myCollisionCallback.areBodiesTouching = false;


		//world->testCollision(collider->getBody(), pisoCollider->getBody(), colisionPiso);
		//if (colisionPiso.areBodiesTouching && !camera.IsJumping) {
		//	// Si hay una colisión, sigue al collider con la cámara
		//	camera.FollowCollider(body, collider, world, pisoCollider);
		//	camera.IsJumping = true;
		//}
		//camera.UpdateJump(deltaTime);
		world->testCollision(collider->getBody(), pisoCollider->getBody(), colisionPiso);

		//if (colisionPiso.areBodiesTouching) {
		//	// Si hay una colisión con el suelo y no está saltando, sigue al collider con la cámara
		//	if (!camera.IsJumping) {
		//		camera.seguir(body);
		//	}
		//	camera.IsJumping = false; // Está en el suelo, por lo que no está saltando
		//}

		if (accumulator >= timeStep)
		{
			glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Calculos de las f?sicas
			while (accumulator >= timeStep)
			{
				world->update(timeStep);
				accumulator -= timeStep;
			}
			decimal factor = accumulator / timeStep;
			currTransform = body->getTransform();
			Transform interTransform = Transform::interpolateTransforms(prevTransform, currTransform, factor);
			prevTransform = currTransform;
			
			float matrix[16];
			interTransform.getOpenGLMatrix(matrix);

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			NewFrame();

			outTexture.ViewTexture();
			ourShader.use();
			ourShader.setFloat("material.shininess", 32.0f);
			ourShader.setInt("material.diffuse", 0);
			ourShader.setInt("material.specular", 0);

			ActivePointLight(ourShader, deltaTime, window);

			TransformCamera(ourShader, false);
			TransformCubo(ourShader);
			if (draw)
				ourModel.Draw(ourShader);

			//SKY
			glDepthFunc(GL_LEQUAL);
			ourShaderSky.use();
			TransformCamera(ourShaderSky, true);
			glBindVertexArray(VAOSKY);
			ourTextureSky.ViewTextureSKY();
			glDepthFunc(GL_LESS);

			//Debug Rendering

			debugShader.use();
			TransformCamera(debugShader, false);
			//TransformCubo2(debugShader,position, interTransform);
			deb.drawColliders();

			ImGUI();

			glfwSwapBuffers(window);
		}
		glfwPollEvents();
	}
	
}

void ActivePointLight(Shader ourShader, float deltatime, GLFWwindow *window)
{

	double currentFrameTime = glfwGetTime();
	// Cambiar targetCutOut cuando se presiona la tecla
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS &&
		currentFrameTime - lastKeyPressTime > 0.2) {

		lastKeyPressTime = currentFrameTime; // Actualizar el tiempo de la última pulsación
		if (!isLightActive) {
			targetCutOut = 20.5f; // Valor objetivo cuando la luz está encendida
			isLightActive = true;
		}
		else {
			targetCutOut = 0.0f; // Valor objetivo cuando la luz está apagada
			isLightActive = false;
		}
	}

	// Calcular la transición gradual de cutOut
	if (currentCutOut != targetCutOut) {
		float transitionSpeed = 10000.0f; // Velocidad de transición de la luz
		if (currentCutOut < targetCutOut) {
			currentCutOut = std::min(currentCutOut + transitionSpeed * deltaTime, targetCutOut);
		}
		else {
			currentCutOut = std::max(currentCutOut - transitionSpeed * deltaTime, targetCutOut);
		}
	}

	ourShader.setVec3("light.position", camera.Position);
	ourShader.setVec3("light.direction", camera.Front);
	ourShader.setFloat("light.outerCutOff", cos(radians(15.5)));
	ourShader.setFloat("light.cutOut", cos(radians(currentCutOut)));
	ourShader.setVec3("light.ambient", 0.01f, 0.01f, 0.01f);
	ourShader.setVec3("light.diffuse", lightColor[0], lightColor[1], lightColor[2]);
	ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
	ourShader.setFloat("light.constant", 1.0f);
	ourShader.setFloat("light.lineal", intensity);
	ourShader.setFloat("light.quatratic", 0.000032f);
}

void TransformCubo(Shader ourShader)//cambia
{
		mat4 modelo = mat4(1.0f);
		modelo = scale(modelo, vec3(sizeCube));
		ourShader.setMat4("model", modelo);
}

//void TransformCubo2(Shader ourShader, Vector3 position, Transform interTransform)
//{
//	glBindVertexArray(VAO);
//	float matrix[16];
//	interTransform.getOpenGLMatrix(matrix);
//
//	mat4 modelo = mat4(
//		matrix[0], matrix[1], matrix[2], matrix[3],
//		matrix[4], matrix[5], matrix[6], matrix[7],
//		matrix[8], matrix[9], matrix[10], matrix[11],
//		matrix[12], matrix[13], matrix[14], matrix[15]
//	);
//	modelo = translate(modelo, vec3(position.x, position.y, position.z));
//	modelo = rotate(modelo, radians(-45.0f), vec3(0.3f, 0.7f, 0.0f));
//	ourShader.setMat4("modelo", modelo);
//
//}

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
	LightUniform(ourShader);
}

void CameraUniform(Shader shaderName)
{
	shaderName.setMat4("projection", projection);
	shaderName.setMat4("view", view);
}

void LightUniform(Shader shaderName)
{
	shaderName.setVec3("viewPos", camera.Front);
	shaderName.setVec3("lightPos", camera.Position);
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
	ColorEdit4("BG", clearColor);
	ColorEdit4("light Color", lightColor);
	SliderFloat("light intesity", &intensity, -0.01f, 0.5f);
	Text("Unlock Cursor"); SameLine();
	TextColored(ImVec4(1, 1, 0, 1), "U"); SameLine(150);
	Text("Lock Cursor"); SameLine();
	TextColored(ImVec4(1, 1, 0, 1), "L");
	Text("Turn on/off spotlight"); SameLine();
	TextColored(ImVec4(1, 1, 0, 1), "F");
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