#include <iostream>
#include <cmath>

#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/lidarSensor.hpp"
#include "../include/render/renderer.hpp"

#include <GLFW/glfw3.h>

using std::cin;
using std::cout;
using std::endl;

int main()
{
    cout << "==========================================\n";
    cout << "  AUTONOMOUS AGENT SIMULATOR V5.1 (LIDAR) \n";
    cout << "==========================================\n";

    // --- PRE-SIMULATION INPUT ---
    int width, height, targetX, targetY, obstacleCount;

    // 1. Grid Dimensions
    cout << "Enter The width of Grid (integer X value): ";
    cin >> width;
    cout << "Enter The height of Grid (integer Y value): ";
    cin >> height;

    // 2. Target Coordinates
    cout << "Enter Target X coordinate (0 to " << width - 1 << "): ";
    cin >> targetX;
    cout << "Enter Target Y coordinate (0 to " << height - 1 << "): ";
    cin >> targetY;

    // 3. Environment Complexity
    cout << "Enter number of random obstacles to spawn: ";
    cin >> obstacleCount;

    // 4. Sensor Specs (Hardcoded)
    float lidarRange = 8.0f;
    float lidarFOVDegrees = 180.0f;
    int lidarRays = 120;

    // --- SAFETY CLAMPS ---
    if (targetX < 0) targetX = 0;
    if (targetX >= width) targetX = width - 1;
    if (targetY < 0) targetY = 0;
    if (targetY >= height) targetY = height - 1;

    int maxObstacles = (width * height) - 2; 
    if (obstacleCount > maxObstacles) obstacleCount = maxObstacles;
    if (obstacleCount < 0) obstacleCount = 0;

    float lidarFOVRadians = lidarFOVDegrees * (M_PI / 180.0f);

    cout << "\nInitializing graphics engine...\n";

    // 1. Initialize Simulation Data
    Environment env(width, height);
    env.placeRandomObstacles(obstacleCount); 

    Vector2D startLocation(0.0f, 0.0f); // Spawn exactly in the center of the first cell
    Vector2D targetLocation(targetX, targetY); // Target the exact center

    env.clearCell(startLocation);
    env.clearCell(startLocation);
    env.clearCell(targetLocation);

    // 2. Initialize LIDAR & Agent
    LidarSensor roofLidar(lidarRange, lidarFOVRadians, lidarRays); 
    Agent myAgent(1, startLocation, &roofLidar, width, height);
    myAgent.setTarget(targetLocation);
    env.placeAgent(myAgent.getPosition());

    // 3. Initialize the Graphics Window, width, height, name
    Renderer renderer(800, 700, "Autonomous Agent Simulator");

    double lastFrameTime = glfwGetTime();
    bool isPaused = false;
    bool spaceWasPressed = false;
    bool rWasPressed = false;
    float timeScale = 1.0f;

    // --- THE FIXED TIMESTEP VARIABLES ---
    const double FIXED_TIME_STEP = 1.0 / 60.0; // Exactly 60Hz physics clock
    double accumulator = 0.0;

    // 4. The Main Game Loop
    while (renderer.isRunning())
    {
        double currentFrameTime = glfwGetTime();
        double frameTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Prevent a "Spiral of Death" if the window lags or is dragged
        if (frameTime > 0.25) frameTime = 0.25;

        // --- KEYBOARD CONTROLS ---
        GLFWwindow *win = renderer.getWindow();

        bool spaceIsPressed = (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS);
        if (spaceIsPressed && !spaceWasPressed) {
            isPaused = !isPaused;
        }
        spaceWasPressed = spaceIsPressed;

        if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS) {
            timeScale += 0.01f;
        }
        if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS) {
            timeScale -= 0.01f;
            if (timeScale < 0.1f) timeScale = 0.1f;
        }

        bool ctrlIsPressed = (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                              glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
        bool rIsPressed = (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS);

        if (ctrlIsPressed && rIsPressed && !rWasPressed) {
            env.clearCell(myAgent.getPosition());
            env.placeRandomObstacles(obstacleCount);
            env.clearCell(startLocation);
            env.clearCell(targetLocation);

            myAgent.reset(startLocation);
            env.placeAgent(myAgent.getPosition());
            cout << "Simulation Reset!\n";
        }
        rWasPressed = rIsPressed;

        // --- A. LOGIC TICK (FIXED TIMESTEP PIPELINE) ---
        accumulator += frameTime * timeScale;

        while (accumulator >= FIXED_TIME_STEP)
        {
            if (!isPaused)
            {
                // 1. Pehle moving blocks ki physics update karo!
                env.updateDynamicObstacles((float)FIXED_TIME_STEP, myAgent.getPosition()); 
                
                myAgent.sense(env); 
                myAgent.decideNextMove(env, (float)FIXED_TIME_STEP); 
            }
            accumulator -= FIXED_TIME_STEP;
        }

        // --- B. RENDER FRAME ---
        renderer.clearScreen(myAgent.isUnreachable());
        renderer.renderInternalMap(myAgent, env); 
        
        // 2. Apne naye moving blocks ko yahan render karo!
        renderer.renderDynamicObstacles(env);     
        
        renderer.renderTarget(targetLocation, width, height); 
        renderer.renderAgent(myAgent, env);
        renderer.renderLidar(myAgent, env);

        renderer.swapBuffers();
        renderer.pollEvents();
    }

    cout << "Simulation Closed.\n";
    return 0;
}


// The Final Pipeline (How it all connects)

//     Gaussian Noise corrupts the agent's location.

//     The Kalman Filter mathematically cleans that noise and outputs an estimatedPosition.

//     The agent fires its LiDAR from that estimatedPosition.

//     The rays travel through the Occupancy Grid (m_probMap), applying the Bayesian Log-Odds update (+2.0f for hits, -1.0f for empty space).

//     If a cell's probability crosses the threshold (> 0.0f), it is rendered as a solid wall for the A* pathfinder.