#include "Application.h"

Application::Application()
{
	initGL();
	makeQuad();
	makeShadow();
	testModel();

	// new Scene
}

int Application::initGL()
{
	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit()) {
		return EXIT_FAILURE;
	}

	glfwSetErrorCallback(error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);


	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGLApplication", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_STENCIL_TEST);

	return EXIT_SUCCESS;
}

Application::~Application()
{
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	/*glDeleteVertexArrays(2, VAOs);
	glDeleteBuffers(2, VBOs);
	glDeleteBuffers(2, EBOs);*/
}

void Application::makeShadow()
{
	// Configure depth map FBO
	simpleDepthShader  = Shader("Shader/vertexShader.SimpleDepth.vs", "Shader/fragmentShader.SimpleDepth.fs", "Shader/gShader.SimpleDepth.gs");
	
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// Attach cubemap as depth map FBO's color buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::makeQuad()
{
	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	quadShader = Shader("Shader/vertexShader.Quad.vs", "Shader/fragmentShader.Quad.fs");
	quadShader.use();
	quadShader.setInt("screenTexture", 0);
	quadShader.setInt("effectTexture", 1);

	greyShader = Shader("Shader/vertexShader.Quad.vs", "Shader/fragmentShader.Grey.fs");
	greyShader.use();
	greyShader.setInt("screenTexture", 0);

	blurShader = Shader("Shader/vertexShader.Quad.vs", "Shader/fragmentShader.Blur.fs");
	blurShader.use();
	blurShader.setInt("screenTexture", 0);

	// framebuffer configuration
	// -------------------------
	
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);


	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &framebuffer1);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer1);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer1, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
	// create a color attachment texture
	glGenTextures(1, &textureColorbuffer2);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer2, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::testModel()
{
	//myModel = Model("Assets/Model/Nanosuit/nanosuit.obj");
	
	myModel = Model("Assets/Model/Walls/walls.obj");
	myShader = Shader("Shader/vertexShader.ModelShadow.vs", "Shader/fragmentShader.ModelShadow.fs");
	myCamera = Camera(glm::vec3(0.0f, 0.0f, 14.0f));

	projectionMat4 = glm::perspective(glm::radians(myCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	modelMat4 = glm::translate(modelMat4, glm::vec3(0.0f, -0.0f, 0.0f));
	modelMat4 = glm::scale(modelMat4, glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec4 avec(0.0, 0.0, -1.0, 1.0);
	avec = glm::inverse(myCamera.GetViewMatrix()) * glm::inverse(projectionMat4) * avec;
	std::cout << avec.x / avec.w << "," << avec.y / avec.w << "," << avec.z / avec.w << std::endl;

	glm::vec4 bvec(0.0, 0.0, 13.9f, 1.0);
	bvec = projectionMat4 * myCamera.GetViewMatrix() * bvec;
	std::cout << bvec.x / bvec.w << "," << bvec.y / bvec.w << "," << bvec.z / bvec.w << std::endl;

	myLight = Light("Assets/Model/Light/sun.obj");
	myLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
	myLight.color = glm::normalize(myLight.color);
	myLight.intensity = 20.0f;
	myLight.position = glm::vec3(0.0f, 0.0f, 0.0f);
	lightModelMat4 = glm::translate(lightModelMat4, myLight.position);
	lightModelMat4 = glm::scale(lightModelMat4, glm::vec3(0.2f, 0.2f, 0.2f));
	lightShader = Shader("Shader/vertexShader.Light.vs", "Shader/fragmentShader.Light.fs");

	ambientLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientLight.intensity = 0.1f;

	myShader.use();

	glUniform1i(glGetUniformLocation(myShader.ID, "depthMap"), 1);

	


	//myShader.setFloat("light.constant", 1.0f);
	//myShader.setFloat("light.linear", 0.09f);
	//myShader.setFloat("light.quadratic", 0.032f);
	myShader.setVec3("lightPos", myLight.position);
	myShader.setVec3("viewPos", myCamera.Position);

	myShader.setMat4("projection", projectionMat4);
	myShader.setMat4("model", modelMat4);
	myShader.setBool("shadows", true);
	myShader.setFloat("far_plane", 25.0f);


	lightShader.use();
	lightShader.setMat4("projection", projectionMat4);
	lightShader.setMat4("model", lightModelMat4);
	lightShader.setVec3("lightColor", myLight.color);
}

int Application::run()
{

	while (!glfwWindowShouldClose(window))
	{
		// runtime
		// -----------
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST);
		// render
		// ------
		render();

		glm::vec4 lightvec(myLight.position, 1.0);
		lightvec = projectionMat4 * myCamera.GetViewMatrix() * lightvec;
		//quadShader.setVec3("lightPosNDC", glm::vec3(lightvec.x / lightvec.w, lightvec.y / lightvec.w, lightvec.z / lightvec.w));

		//grey
		// -------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// clear all relevant buffers
		greyShader.use();
		greyShader.setFloat("greyThreashold", 0.4f);
		//greyShader.setVec3("lightPosNDC", glm::vec3(lightvec.x / lightvec.w, lightvec.y / lightvec.w, lightvec.z / lightvec.w));

		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// blur 
		// ---------------

		int blurIteration = 2;
		float sampleScale = 20.0;
		float samplerOffset = sampleScale / SCR_WIDTH;
		for (int i = 0; i < blurIteration; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			blurShader.use();
			blurShader.setVec3("lightPosNDC", glm::vec3(lightvec.x / lightvec.w, lightvec.y / lightvec.w, lightvec.z / lightvec.w));
			blurShader.setFloat("blurOffset", samplerOffset * (i * 2 + 1));

			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer1);
			glDrawArrays(GL_TRIANGLES, 0, 6);


			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			blurShader.use();
			blurShader.setVec3("lightPosNDC", glm::vec3(lightvec.x / lightvec.w, lightvec.y / lightvec.w, lightvec.z / lightvec.w));
			blurShader.setFloat("blurOffset", samplerOffset * (i * 2 + 2));

			glBindVertexArray(quadVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer2);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


		// blend
		// ----------------
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
		// clear all relevant buffers
		quadShader.use();
		
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void Application::render()
{


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	// 0. Create depth cubemap transformation matrices
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	GLfloat _near = 1.0f;
	GLfloat _far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(90.0f, aspect, _near, _far);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(myLight.position, myLight.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(myLight.position, myLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(myLight.position, myLight.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(myLight.position, myLight.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(myLight.position, myLight.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(myLight.position, myLight.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	// 1. Render scene to depth cubemap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	simpleDepthShader.use();
	for (GLuint i = 0; i < 6; ++i)
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.ID, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
	glUniform1f(glGetUniformLocation(simpleDepthShader.ID, "far_plane"), _far);
	glUniform3fv(glGetUniformLocation(simpleDepthShader.ID, "lightPos"), 1, glm::value_ptr(myLight.position));
	myModel.Draw(simpleDepthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);


	myShader.use();
	// set matrix
	// -----------------------------
	myShader.setMat4("view", myCamera.GetViewMatrix());
	myShader.setVec3("viewPos", myCamera.Position);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	myModel.Draw(myShader);

	lightShader.use();
	lightShader.setMat4("view", myCamera.GetViewMatrix());
	myLight.model.Draw(lightShader);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Application::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		myCamera.Position += myCamera.MovementSpeed * deltaTime * myCamera.Front;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		myCamera.Position -= myCamera.MovementSpeed * deltaTime * myCamera.Front;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		myCamera.Position -= glm::normalize(glm::cross(myCamera.Front, myCamera.Up)) * myCamera.MovementSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		myCamera.Position += glm::normalize(glm::cross(myCamera.Front, myCamera.Up)) * myCamera.MovementSpeed * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		myCamera.ProcessMouseMovement(-2.5, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		myCamera.ProcessMouseMovement(2.5, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		myCamera.ProcessMouseMovement(0, 2.5);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		myCamera.ProcessMouseMovement(0, -2.5);
	}
}
