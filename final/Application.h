#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"

#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"

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
	void render();
	void testModel();
	void makeShadow();


	glm::mat4 projectionMat4;

	Shader myShader;
	Model myModel;
	glm::mat4 modelMat4;

	Camera myCamera;

	// light
	Shader lightShader;
	Light myLight;
	glm::mat4 lightModelMat4;

	Light ambientLight;
	
	void makeQuad();
	unsigned int quadVAO, quadVBO;
	Shader quadShader;
	Shader greyShader;
	Shader blurShader;
	unsigned int textureColorbuffer;
	unsigned int framebuffer;

	unsigned int textureColorbuffer1;
	unsigned int framebuffer1;

	unsigned int textureColorbuffer2;
	unsigned int framebuffer2;

	void processInput(GLFWwindow *window);

	// shadow
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	GLuint depthCubemap;
	Shader simpleDepthShader;
};

static const unsigned int SCR_WIDTH = 1920;
static const unsigned int SCR_HEIGHT = 1080;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#endif