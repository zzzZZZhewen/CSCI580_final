#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Shader.h"
#include "stb_image.h"
class Application
{
public:
	Application();
	~Application();	
	int run();
private:
	GLFWwindow *window;

	float lastFrame = 0.0f;
	float deltaTime = 0.0f;
	float currentFrame = 0.0f;

	int initGL();
	void loadTexture();
	void testVertexBuffer();
	void render();

	unsigned int VBOs[2], VAOs[2], EBOs[2];
	unsigned int texture1, texture2;
	Shader myShader;
	Camera myCamera;
	//unsigned int shaderProgram;

	void processInput(GLFWwindow *window);
};

/*
static const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"  vertexColor = aColor;"
"}\0";
static const char *fragmentShader1Source = "#version 330 core\n"
"in vec3 vertexColor;\n"
"out vec4 FragColor;\n"
"uniform vec4 outColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(vertexColor, 1.0) * outColor.g;\n"
"}\n\0";
*/
static const unsigned int SCR_WIDTH = 800;
static const unsigned int SCR_HEIGHT = 600;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

#endif