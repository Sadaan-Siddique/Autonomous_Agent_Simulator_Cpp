#include <iostream>
// Step out of 'app' and into 'include' to grab your headers
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/distanceSensor.hpp"
#include "../include/render/renderer.hpp"

// GLFW gives us access to time functions needed for the tick-rate
#include <GLFW/glfw3.h>

int main()
{
    std::cout << "Starting Autonomous Agent Simulator (OpenGL Renderer)...\n";

    // 1. Initialize Simulation Data
    Environment env(15, 10);
    env.placeRandomObstacles(40);

    Vector2D startLocation(0, 0);
    Vector2D targetLocation(14, 9);
    env.clearCell(startLocation);
    env.clearCell(targetLocation);

    DistanceSensor mySensor(3);
    Agent myAgent(1, startLocation, &mySensor);
    myAgent.setTarget(targetLocation);

    // Explicitly place the agent on the grid so the environment knows it's there
    env.placeAgent(myAgent.getPosition());

    // 2. Initialize the Graphics Window
    // This will open a 800x600 window with the specified title
    Renderer renderer(800, 600, "Autonomous Agent Simulator V5.1");

    // 3. Time Management for Kinematics
    // We completely remove tickRate. We need to track exact frame times.
    double lastFrameTime = glfwGetTime();

    // 4. The Main Game Loop
    while (renderer.isRunning())
    {
        // Calculate deltaTime (the fraction of a second the last frame took)
        double currentFrameTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        // --- A. LOGIC TICK (Runs every single frame now) ---
        if (myAgent.getPosition().distance(targetLocation) > 0.1f)
        {
            myAgent.decideNextMove(env, deltaTime);
        }
        else
        {
            // Optional: Print only once, or just let it idle.
            // std::cout << "Target Reached!\n";
        }

        // --- B. RENDER FRAME (Runs as fast as possible) ---
        renderer.clearScreen();
        renderer.renderEnvironment(env);    // Draw the walls and empty spaces
        renderer.renderAgent(myAgent, env); // Draw the agent on top of the grid
        renderer.swapBuffers();             // Swap the hidden buffer to the screen and check for window close events
        renderer.pollEvents();
    }

    std::cout << "Simulation Closed.\n";

    return 0;
}