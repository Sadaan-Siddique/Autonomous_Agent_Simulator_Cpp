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
    // GPU Memory IDs for different shapes
    unsigned int m_quadVAO, m_quadVBO;
    unsigned int m_lineVAO, m_lineVBO;   // For LIDAR beams
    unsigned int m_arrowVAO, m_arrowVBO; // For the Agent arrow
    unsigned int m_fovVAO, m_fovVBO; // For the translucent FOV fan

    // Internal Setup Functions
    void initShaders();
    void initPrimitives(); 

    // The core math drawing function
    void drawQuad(float x, float y, float width, float height, float r, float g, float b) const;
    void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a) const; // float a for transparency control

public:
    Renderer(int screenWidth, int screenHeight, const char *title);
    ~Renderer();

    // NEW: Allow main.cpp to access the window to read keyboard presses
    GLFWwindow* getWindow() const;

    // Game loop Functions
    bool isRunning() const;
    void clearScreen(bool isAlarmActive = false) const;
    void swapBuffers() const;
    void pollEvents() const;

    // Simulation rendering
    void renderEnvironment(const Environment &env);
    void renderInternalMap(const Agent& agent, int width, int height);
    void renderTarget(const Vector2D& target, int width, int height) const;
    void renderAgent(const Agent &agent, const Environment &env);
    void renderLidar(const Agent& agent, const Environment& env);
};

#endif

// Encapsulation: main.cpp shouldn't know what a VAO or VBO is. It should just say renderer.renderAgent(agent) and let this class handle the hardware communication.

// m_VAO and m_VBO: These are Vertex Array Objects and Vertex Buffer Objects. Instead of sending coordinates to the GPU every frame (which bottlenecks the CPU), we will use these to upload a square to the GPU's high-speed Video RAM (VRAM) just once when the program starts.