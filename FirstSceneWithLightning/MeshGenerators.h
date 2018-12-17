#pragma once
#include "Mesh.h"

#include "GL/freeglut.h"
#include <glm/glm.hpp>
#include <iostream>

Mesh createCubeMesh(std::vector<Texture> textures);
Mesh createQuadMesh(std::vector<Texture> textures);