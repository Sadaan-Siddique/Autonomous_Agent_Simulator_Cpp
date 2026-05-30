// This file acts as a blueprint. It hides all the messy X11 windowing and raw GPU pointers from your main game loop

#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.c>
#include <GLFW/glfw3.h>
#include "../environment/environment.hpp"
#include "../agents/agent.hpp"
