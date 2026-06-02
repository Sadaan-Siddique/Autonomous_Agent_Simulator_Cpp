// This file acts as a blueprint. It hides all the messy X11 windowing and raw GPU pointers from your main game loop

#ifndef RENDERER_H
#define RENDERER_H

#include "../../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "../environment/environment.hpp"
#include "../agents/agent.hpp"

class Renderer
{
private:
    GLFWwindow *m_window;

    // GPU Memory IDs
    unsigned int m_shaderProgram;
    unsigned int m_VAO, m_VBO;

    // Internal Setup Functions
    void initShaders();
    void initQuad();

    // The core math drawing function
    void drawQuad(float, float y, float width, float height, float r, float g, float b);

public:
    Renderer(int screenWidth, int screenHeight, const char *title);
    ~Renderer();

    // Game loop Functions
    bool isRunning() const;
    void clearScreen() const;
    void swapBuffers() const;
    void pollEvents() const;

    // Simulation rendering
    void renderEnvironment(const Environment &env);
    void renderAgent(const Agent &agent, const Environment &env);
    void renderLidar(const Agent& agent, const Environment& env);
};

#endif

// Encapsulation: main.cpp shouldn't know what a VAO or VBO is. It should just say renderer.renderAgent(agent) and let this class handle the hardware communication.

// m_VAO and m_VBO: These are Vertex Array Objects and Vertex Buffer Objects. Instead of sending coordinates to the GPU every frame (which bottlenecks the CPU), we will use these to upload a square to the GPU's high-speed Video RAM (VRAM) just once when the program starts.