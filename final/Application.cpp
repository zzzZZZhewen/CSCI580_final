#include "Application.h"

Application::Application()
{
	initGL();
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



void Application::testModel()
{
	//myModel = Model("Assets/Model/Nanosuit/nanosuit.obj");
	
	myModel = Model("Assets/Model/Walls/walls.obj");
	myShader = Shader("Shader/vertexShader.ModelTest.vs", "Shader/fragmentShader.ModelTest.fs");
	myCamera = Camera(glm::vec3(0.0f, 0.0f, 15.0f));

	projectionMat4 = glm::perspective(glm::radians(myCamera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	modelMat4 = glm::translate(modelMat4, glm::vec3(0.0f, -0.0f, 0.0f));
	modelMat4 = glm::scale(modelMat4, glm::vec3(1.0f, 1.0f, 1.0f));


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

	myShader.setVec3("light.ambient", ambientLight.color * ambientLight.intensity);
	myShader.setVec3("light.diffuse", myLight.color * myLight.intensity);
	myShader.setVec3("light.specular", myLight.color * myLight.intensity);
	myShader.setVec3("light.position", myLight.position);

	myShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
	myShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
	myShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
	myShader.setFloat("material.shininess", 64.0f);

	myShader.setFloat("light.constant", 1.0f);
	myShader.setFloat("light.linear", 0.09f);
	myShader.setFloat("light.quadratic", 0.032f);

	myShader.setVec3("viewPos", myCamera.Position);

	myShader.setMat4("projection", projectionMat4);
	myShader.setMat4("model", modelMat4);



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
		// render
		// ------
		render();

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

	myShader.use();
	// set matrix
	// -----------------------------
	myShader.setMat4("view", myCamera.GetViewMatrix());
	myShader.setVec3("viewPos", myCamera.Position);
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
