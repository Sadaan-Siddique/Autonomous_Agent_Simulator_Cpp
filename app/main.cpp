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

    // 3. Time Management for the "Tick Rate"
    // We don't want the agent making 60 decisions a second.
    double lastTickTime = glfwGetTime();
    double tickRate = 0.5; // The agent takes one step every 0.5 seconds

    // 4. The Main Game Loop
    // This loop runs as fast as your CPU/GPU allows (often 1000+ times a second)
    while (renderer.isRunning())
    {

        // env.placeRandomObstacles(40);
        // env.clearCell(startLocation);
        // env.clearCell(targetLocation);

        double currentTime = glfwGetTime();

        // --- A. LOGIC TICK (Runs only twice a second) ---
        if (currentTime - lastTickTime >= tickRate)
        {

            // Only decide a move if the agent hasn't reached the goal
            if (!(myAgent.getPosition() == targetLocation))
            {
                myAgent.decideNextMove(env);
                if (myAgent.getPosition() == startLocation)
                {
                    renderer.~Renderer();
                    break;
                }
            }
            else
            {
                std::cout << "Target Reached!\n";
                // Optional: You could add a break; here to close the window,
                // or just let it stay open so you can view the final state.
            }

            // Reset the timer for the next move
            lastTickTime = currentTime;
        }

        // --- B. RENDER FRAME (Runs as fast as possible) ---
        renderer.clearScreen();

        // Draw the walls and empty spaces
        renderer.renderEnvironment(env);

        // Draw the agent on top of the grid
        renderer.renderAgent(myAgent, env);

        // Swap the hidden buffer to the screen and check for window close events
        renderer.swapBuffers();
        renderer.pollEvents();
    }

    std::cout << "Simulation Closed.\n";

    // Matrix
    // std::cout << "=== MATRIX MATH MODULE TEST ===\n\n";

    // Matrix A(2, 2);
    // A.setValue(0, 0, 1); A.setValue(0, 1, 2);
    // A.setValue(1, 0, 3); A.setValue(1, 1, 4);

    // Matrix B(2, 2);
    // B.setValue(0, 0, 5); B.setValue(0, 1, 6);
    // B.setValue(1, 0, 7); B.setValue(1, 1, 8);

    // std::cout << "Matrix A:\n";
    // A.display();

    // std::cout << "\nMatrix B:\n";
    // B.display();

    // std::cout << "\nAddition (A + B):\n";
    // Matrix C = A + B;
    // C.display();

    // std::cout << "\nMultiplication (A * B):\n";
    // Matrix D = A * B;
    // D.display();

    // std::cout << "\nTranspose of A:\n";
    // A.transpose().display();

    return 0;
}