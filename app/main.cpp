#include <iostream>
#include <cmath> // Required for M_PI

// Include headers
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/lidarSensor.hpp" // NEW: Lidar Sensor
#include "../include/render/renderer.hpp"

#include <GLFW/glfw3.h>

int main()
{
    std::cout << "Starting Autonomous Agent Simulator V5.1...\n";

    // 1. Initialize Simulation Data
    Environment env(15, 10);
    env.placeRandomObstacles(40);

    Vector2D startLocation(0, 0);
    Vector2D targetLocation(14, 9);
    env.clearCell(startLocation);
    env.clearCell(targetLocation);

    // 2. Initialize LIDAR & Agent
    // Range: 5.0 units, FOV: 360 degrees (2 * PI radians), Rays: 36
    LidarSensor roofLidar(5.0f, 2.0f * M_PI, 36);
    Agent myAgent(1, startLocation, &roofLidar);
    myAgent.setTarget(targetLocation);

    env.placeAgent(myAgent.getPosition());

    // 3. Initialize the Graphics Window
    Renderer renderer(800, 600, "Autonomous Agent Simulator V5.1 (LIDAR)");

    // Time Management
    double lastFrameTime = glfwGetTime();

    // 4. The Main Game Loop
    while (renderer.isRunning())
    {
        // Calculate deltaTime
        double currentFrameTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        // --- A. LOGIC TICK ---
        // 1. The Brain: Decide the next steering angle and move
        myAgent.decideNextMove(env, deltaTime);
        
        // 2. The Eyes: Fire the lasers from the new position
        myAgent.sense(env); 

        // --- B. RENDER FRAME ---
        renderer.clearScreen();
        renderer.renderEnvironment(env);
        renderer.renderAgent(myAgent, env);
        
        // NEW: Draw the Point Cloud
        renderer.renderLidar(myAgent, env); 
        
        renderer.swapBuffers();
        renderer.pollEvents();
    }

    std::cout << "Simulation Closed.\n";
    return 0;
}