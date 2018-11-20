#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>f

#include "Model.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


class Light
{
public:
	Light() {
		position = glm::vec3(0.0f);
		color = glm::vec3(1.0f);
		intensity = 1.0f;
		hasModel = false;
	}

	Light(string const &path)
	{
		position = glm::vec3(0.0f);
		color = glm::vec3(1.0f);
		intensity = 1.0f;
		model = Model(path);
		hasModel = true;
	}

	bool hasModel = false;
	glm::vec3 position;
	glm::vec3 color;
	float	  intensity;
	Model model;
};


#endif