#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Scene
{

public:
	Scene();
	virtual void	Render() = 0;
	virtual glm::vec3 getViewMatrix() = 0;
	virtual glm::vec3 getProjectionMatrix() = 0;
	~Scene();
};

#endif