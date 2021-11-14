#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace std;

const int screen_height = 1080;
const int screen_width = 1920;
const char* vertexShdFile = "vertex.glsl";
const char* fragmentShdFile = "fragment.glsl";

void input(GLFWwindow* w);
bool compileShader(const char* src, unsigned int shader);

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


	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  
	};

	unsigned int VBO /*Vertex Buffer Object*/;
	unsigned int VAO /*Vertex Array Object*/;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * 4));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Legacy OpenGL
		//glBegin(GL_TRIANGLES);
		//glVertex2d(-0.5f, -0.5f);
		//glVertex2d(0.0f, 0.5f);
		//glVertex2d(0.5f, -0.5f);
		//glEnd();
		
		glUseProgram(shdProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
		return false;
	}
	cout << "Shader loaded and compiled succesfully!" << endl;
	return true;
}