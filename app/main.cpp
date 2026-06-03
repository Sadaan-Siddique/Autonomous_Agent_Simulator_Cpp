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
    int width, height, targetX, targetY, obstacleCount, lidarRays;
    float lidarRange, lidarFOVDegrees;

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

    // 4. Sensor Specs
    cout << "Enter LIDAR Range (in grid units, e.g. 5.0): ";
    cin >> lidarRange;
    cout << "Enter LIDAR Field of View (in degrees, 1 to 360): ";
    cin >> lidarFOVDegrees;
    cout << "Enter LIDAR Ray Count (number of lasers, e.g. 36): ";
    cin >> lidarRays;

    // --- SAFETY CLAMPS ---
    // Ensure target doesn't exceed array bounds (width - 1)
    if (targetX < 0)
        targetX = 0;
    if (targetX >= width)
        targetX = width - 1;
    if (targetY < 0)
        targetY = 0;
    if (targetY >= height)
        targetY = height - 1;

    // Ensure we don't try to spawn more obstacles than cells exist
    int maxObstacles = (width * height) - 2; // Leave room for Agent and Target
    if (obstacleCount > maxObstacles)
        obstacleCount = maxObstacles;
    if (obstacleCount < 0)
        obstacleCount = 0;

    // Ensure valid sensor math
    if (lidarRays < 2)
        lidarRays = 2; // Prevent divide by zero in angle step
    if (lidarRange <= 0.1f)
        lidarRange = 5.0f;

    // Convert degrees to radians for the math engine
    float lidarFOVRadians = lidarFOVDegrees * (M_PI / 180.0f);

    cout << "\nInitializing graphics engine...\n";

    // 1. Initialize Simulation Data
    Environment env(width, height);
    env.placeRandomObstacles(obstacleCount); // Dynamic

    Vector2D startLocation(0, 0); // Always fixed
    Vector2D targetLocation(targetX, targetY);

    env.clearCell(startLocation);
    env.clearCell(targetLocation);

    // 2. Initialize LIDAR & Agent
    LidarSensor roofLidar(lidarRange, lidarFOVRadians, lidarRays); // Dynamic
    Agent myAgent(1, startLocation, &roofLidar, width, height);
    myAgent.setTarget(targetLocation);
    env.placeAgent(myAgent.getPosition());

    // 3. Initialize the Graphics Window
    Renderer renderer(800, 600, "Autonomous Agent Simulator");

    double lastFrameTime = glfwGetTime();
    bool isPaused = false;
    bool spaceWasPressed = false;
    bool rWasPressed = false;
    float timeScale = 1.0f;

    // 4. The Main Game Loop
    while (renderer.isRunning())
    {
        double currentFrameTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
        lastFrameTime = currentFrameTime;

        // --- KEYBOARD CONTROLS ---
        GLFWwindow *win = renderer.getWindow();

        // 1. Pause Toggle (Spacebar)
        bool spaceIsPressed = (glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS);
        if (spaceIsPressed && !spaceWasPressed)
        {
            isPaused = !isPaused;
        }
        spaceWasPressed = spaceIsPressed;

        // 2. Speed Control (Up/Down Arrows)
        if (glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS)
        {
            timeScale += 0.01f;
        }
        if (glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            timeScale -= 0.01f;
            if (timeScale < 0.1f)
                timeScale = 0.1f;
        }

        // --- 3. RESTART SIMULATION (Ctrl + R) ---
        bool ctrlIsPressed = (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                              glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
        bool rIsPressed = (glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS);

        if (ctrlIsPressed && rIsPressed && !rWasPressed)
        {
            env.clearCell(myAgent.getPosition());

            // Use the dynamic obstacle count here too!
            env.placeRandomObstacles(obstacleCount);
            env.clearCell(startLocation);
            env.clearCell(targetLocation);

            myAgent.reset(startLocation);
            env.placeAgent(myAgent.getPosition());

            cout << "Simulation Reset!\n";
        }
        rWasPressed = rIsPressed;

        // --- A. LOGIC TICK ---
        // In autonomous robotics, the software loop must follow the Sense-Plan-Act architecture. The robot must look at the world, update its map, plan a route, and then drive.
        if (!isPaused)
        {
            myAgent.sense(env); // pehle sense krega phir move decide karega
            myAgent.decideNextMove(env, deltaTime * timeScale);
        }

        // --- B. RENDER FRAME ---
        renderer.clearScreen(myAgent.isUnreachable());
        // renderer.renderEnvironment(env);
        renderer.renderInternalMap(myAgent, width, height); // Draw the Fog of War instead of the master map
        renderer.renderTarget(targetLocation, width, height); // Draw the target destination so we can see it in the dark!
        renderer.renderAgent(myAgent, env);
        renderer.renderLidar(myAgent, env);

        renderer.swapBuffers();
        renderer.pollEvents();
    }

    cout << "Simulation Closed.\n";
    return 0;
}