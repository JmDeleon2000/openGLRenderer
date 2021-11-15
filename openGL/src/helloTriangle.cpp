#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#define lookAtEnabled true

using namespace std;
//rendering
const int screen_height = 1080;
const int screen_width = 1920;
const char* vertexShdFile = "vertex.glsl";
const char* fragmentShdFile = "fragment.glsl";
//object
glm::vec3 objectPos = glm::vec3(0, 0, 0);
glm::vec3 objectRot = glm::vec3(0, 0, 0);
glm::vec3 objectScl = glm::vec3(1, 1, 1);
glm::mat4 objMatrix;
//camera
glm::vec3 camPos = glm::vec3(0, 0, 10);
glm::vec3 camRot = glm::vec3(0, 0, 0);
glm::vec3 cameraTarget = glm::vec3(0, 0, 0);
#if lookAtEnabled
//https://learnopengl.com/Getting-started/Camera
//https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/7.4.camera_class/camera_class.cpp
glm::vec3 cameraDirection = glm::normalize(camPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
float cameraZoom = 60.0f;
const float cameraDist = 10;
#endif
glm::mat4 projection;
glm::mat4 viewMatrix;

double deltaTime = 1;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool compileShader(const char* src, unsigned int shader);
void updateModelMatrix();
void updateViewMatrix();

//https://www.youtube.com/watch?v=OR4fNpBjmq8
int main(void) 
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	
	window = glfwCreateWindow(screen_width, screen_height, "Hello Triangle!", NULL, NULL);
	if (!window) 
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	cout << "Window created succesfuly" << endl;

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (glewInit() != GLEW_OK)
	{
		cout << "Couldn't initialize GLEW!" << endl;
		return -1;
	}
	cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;

	char infoLog[512];
	int success;
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	if (!compileShader(vertexShdFile, vertShader)) return -1;
	if (!compileShader(fragmentShdFile, fragShader)) return -1;

	unsigned int shdProgram = glCreateProgram();
	glAttachShader(shdProgram, vertShader);
	glAttachShader(shdProgram, fragShader);
	glLinkProgram(shdProgram);

	glGetProgramiv(shdProgram, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(shdProgram, 512, NULL, infoLog);
		cout << "Error:" << endl << infoLog << endl;
		return -1;
	}
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	glEnable(GL_DEPTH_TEST);

	float vertices[] = {
		-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 
		 0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,  
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		 -0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
		 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 1.0f
	};

	unsigned int index_data[] = {
		0, 1, 2,
		0, 3, 1, 
		3, 1, 4,
		4, 2, 1,
		3, 2, 4,
		0, 2, 3
	};

	unsigned int VBO /*Vertex Buffer Object*/;
	unsigned int VAO /*Vertex Array Object*/;
	unsigned int EAO /*Element Array Object*/;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EAO);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EAO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * 4));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//camera
	projection = glm::perspective(glm::radians(60.0f), 
								(float)((float)screen_width/(float)screen_height),
								0.1f,
								1000.0f);
	const double limitFPS = 60.0;
	double currTime = 0, lastFrame = 0;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if lookAtEnabled
		projection = glm::perspective(glm::radians(cameraZoom),
			(float)((float)screen_width / (float)screen_height),
			0.1f,
			1000.0f);
#endif

		currTime = glfwGetTime();
		deltaTime = (currTime - lastFrame) * limitFPS;
		lastFrame = currTime;

		//camera
		cameraDirection = glm::normalize(camPos - cameraTarget);
		cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		cameraUp = glm::cross(cameraDirection, cameraRight);
		
		glUseProgram(shdProgram);
		glBindVertexArray(VAO);
		updateViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shdProgram, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		updateModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shdProgram, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(objMatrix));
		
		glUniformMatrix4fv(glGetUniformLocation(shdProgram, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform1f(glGetUniformLocation(shdProgram, "appTime"), (float)glfwGetTime());

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, sizeof(index_data)/sizeof(unsigned int), GL_UNSIGNED_INT, index_data);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shdProgram);

	glfwTerminate();
	return 0;
}

bool compileShader(const char* src, unsigned int shader)
{
	fstream fileStream;
	fileStream.open(src, std::fstream::in);
	if (!fileStream.is_open())
	{
		cout << "Shader source not was not found." << endl;
		return false;
	}
	string shdSrcTempBuffer = "";
	string readingBuffer;
	while (getline(fileStream, readingBuffer))
		shdSrcTempBuffer += readingBuffer + '\n';
	fileStream.close();
	const char* vertShdDump = shdSrcTempBuffer.c_str();

	//shader compilation
	char infoLog[512];
	glShaderSource(shader, 1, &vertShdDump, NULL);
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cout << "Error:" << endl;
		cout << infoLog << endl;
		return false;
	}
	cout << "Shader loaded and compiled succesfully!" << endl;
	return true;
}

void updateModelMatrix() 
{
	glm::mat4 identity = glm::mat4(1);
	glm::mat4 translate = glm::translate(identity, objectPos);

	glm::mat4 rotation = glm::rotate(identity, glm::radians(objectRot.x), glm::vec3(1, 0, 0)) *		//pitch
						glm::rotate(identity, glm::radians(objectRot.y), glm::vec3(0, 1, 0)) *		//yaw
						glm::rotate(identity, glm::radians(objectRot.z), glm::vec3(0, 0, 1));		//roll

	glm::mat4 scale = glm::scale(identity, objectScl);

	objMatrix = translate * rotation * scale;
}
#if lookAtEnabled
void updateViewMatrix() 
{
	viewMatrix = glm::lookAt(camPos,				//position
							objectPos,				//target
							glm::vec3(0, 1, 0));	//up
}
#else
void updateViewMatrix()
{
	glm::mat4 identity = glm::mat4(1);
	glm::mat4 translate = glm::translate(identity, camPos);


	glm::mat4 rotation = glm::rotate(identity, glm::radians(camRot.x), glm::vec3(1, 0, 0)) *	//pitch
						glm::rotate(identity, glm::radians(camRot.y), glm::vec3(0, 1, 0)) *		//yaw
						glm::rotate(identity, glm::radians(camRot.z), glm::vec3(0, 0, 1));		//roll

	viewMatrix = glm::inverse(translate * rotation);
}
#endif


float zoomSpeed = 20;
float rotateSpeed = 20;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == GLFW_REPEAT) 
	{
		switch (key)
		{
		case GLFW_KEY_Q:
			cameraZoom -= zoomSpeed * deltaTime;
			cameraZoom = cameraZoom > 10 ? cameraZoom : 10;
			break;
		case GLFW_KEY_E:
			cameraZoom += zoomSpeed * deltaTime;
			cameraZoom = cameraZoom < 90 ? cameraZoom : 90;

			break;
		case GLFW_KEY_A:
			camPos -= cameraRight * (float)(rotateSpeed * deltaTime);
			break;
		case GLFW_KEY_D:
			camPos += cameraRight * (float)(rotateSpeed * deltaTime);
			break;
		case GLFW_KEY_W:
			camPos += cameraUp * (float)(rotateSpeed * deltaTime);
			break;
		case GLFW_KEY_S:
			camPos -= cameraUp * (float)(rotateSpeed * deltaTime);
			break;
		default:
			break;
		}
		camPos = glm::normalize(camPos) * cameraDist; //TODO arreglar precisión
	}
}