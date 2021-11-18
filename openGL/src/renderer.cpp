#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "stb_image.h"
#include "models.h"


#define lookAtEnabled true

#define SIZEOFARRAY(a) sizeof(a)/sizeof(a[0])

using namespace std;
//rendering
const int screen_height = 1080;
const int screen_width = 1920;
const char* vertexShdFile = "glitchTransformVertex.glsl";
const char* fragmentShdFile = "glitchTransformFrag.glsl";
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
float mandelbrotZoom = 2;
#endif
glm::mat4 projection;
glm::mat4 viewMatrix;

double deltaTime = 1;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool compileShader(const char* src, unsigned int shader);
void updateModelMatrix();
void updateViewMatrix();
bool compileProgram(const char* vertSrc, const char* fragSrc, unsigned int* shader);
class Texture
{
public:
	int img_width, img_height, img_BPP;
	void* img_buffer_ptr;
	unsigned int img_opGLID;
public:
	Texture(const char* path, unsigned int slot);
	~Texture();

};


unsigned int activeShader = -1;
unsigned int shdProgram = -1;
unsigned int shdProgram1 = -1;
unsigned int shdProgram2 = -1;
unsigned int shdProgram3 = -1;


//https://www.youtube.com/watch?v=OR4fNpBjmq8
int main(void)
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(screen_width, screen_height, "Model viewer", NULL, NULL);
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

	if (!compileProgram(vertexShdFile, fragmentShdFile, &shdProgram)) return -1;
	if (!compileProgram("toonVertex.glsl", "toonFrag.glsl", &shdProgram1)) return -1;
	if (!compileProgram("MandelbrotVertex.glsl", "MandelbrotFrag.glsl", &shdProgram2)) return -1;
	if (!compileProgram("specularVertex.glsl", "specularFrag.glsl", &shdProgram3)) return -1;

	activeShader = shdProgram;

	Texture* model_tex = new Texture("model.bmp", 0);
	unsigned int activeTexture = model_tex->img_opGLID;


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int VAO; //vertex array object
	unsigned int VBO; //vertex buffer object

	Model* model_to_render = new Model("model.obj");

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, model_to_render->faceCount * 24 * sizeof(float), model_to_render->mainBuffer, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	//camera
	projection = glm::perspective(glm::radians(60.0f),
								(float)((float)screen_width / (float)screen_height),
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
				
		glUseProgram(activeShader);


		updateViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(activeShader, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
		updateModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(activeShader, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(objMatrix));
		
		glUniformMatrix4fv(glGetUniformLocation(activeShader, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform1f(glGetUniformLocation(activeShader, "appTime"), (float)glfwGetTime());
		glUniform1f(glGetUniformLocation(activeShader, "mandelbrotZoom"), mandelbrotZoom);
		glUniform4f(glGetUniformLocation(activeShader, "lightSrc"), -10, 0, 2, 1);
		glUniform3f(glGetUniformLocation(activeShader, "lightSrc"), camPos.x, camPos.y , camPos.z);

		glDrawArrays(GL_TRIANGLES, 0, model_to_render->faceCount * 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shdProgram);

	glfwTerminate();
	return 0;
}

bool compileProgram(const char* vertSrc, const char* fragSrc, unsigned int* shader)
{
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (!compileShader(vertSrc, vertShader)) return false;
	if (!compileShader(fragSrc, fragShader)) return false;
	*shader = glCreateProgram();

	glAttachShader(*shader, vertShader);
	glAttachShader(*shader, fragShader);
	glLinkProgram(*shader);

	char infoLog[512];
	int success;
	glGetProgramiv(*shader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(*shader, 512, NULL, infoLog);
		cout << "Error:" << endl << infoLog << endl;
		return -1;
	}
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	return true;
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
float rotateSpeed = 10;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == GLFW_REPEAT) 
	{
		switch (key)
		{
			//zoom
		case GLFW_KEY_Q:
			cameraZoom -= zoomSpeed * deltaTime;
			cameraZoom = cameraZoom > 10 ? cameraZoom : 10;
			break;
		case GLFW_KEY_E:
			cameraZoom += zoomSpeed * deltaTime;
			cameraZoom = cameraZoom < 90 ? cameraZoom : 90;
			break;
			// move camera
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
		case GLFW_KEY_UP:
			mandelbrotZoom -= (float)(0.25 * deltaTime);
			break;
		case GLFW_KEY_DOWN:
			mandelbrotZoom += (float)(0.25 * deltaTime);
			mandelbrotZoom = mandelbrotZoom < 2 ? mandelbrotZoom : 2;
			break;
		default:
			break;
		}
		camPos = glm::normalize(camPos) * cameraDist; //TODO arreglar precisión
	}
	if (action == GLFW_PRESS) 
	{
		switch (key)
		{
			//shader swap
		case GLFW_KEY_1:
			activeShader = shdProgram;
			break;
		case GLFW_KEY_2:
			activeShader = shdProgram1;
			break;
		case GLFW_KEY_3:
			activeShader = shdProgram2;
			break;
		case GLFW_KEY_4:
			activeShader = shdProgram3;
			break;
		default:
			break;
		}
	}
}

Texture::Texture(const char* path, unsigned int slot = 0)
{
	stbi_set_flip_vertically_on_load(1);
	img_buffer_ptr = stbi_load(path, &img_width, &img_height, &img_BPP, 4);
	glGenTextures(1, &img_opGLID);
	glBindTexture(GL_TEXTURE_2D, img_opGLID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	//WRAP HORIZONTAL
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);	//WRAP VERTICAL


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_buffer_ptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE + slot);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &img_opGLID);
	if (img_buffer_ptr)
		stbi_image_free(img_buffer_ptr);
}