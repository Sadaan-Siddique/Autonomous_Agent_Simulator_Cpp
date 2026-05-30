#include <iostream>
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/distanceSensor.hpp"
#include "../include/pathFinding/bfs.hpp"
#include "../include/math/matrix.hpp"

void pauseSimulation()
{
    // std::cout << "\nPress Enter to continue to the next step...";
    // std::cin.get();
    for (int i = 0; i < 300000000; i++);
    std::cout << "\n----------------------------------------\n\n";
}

int main()
{
    std::cout << "=== AUTONOMOUS AGENT SIMULATOR V5 ===\n";
    std::cout << "=== Dynamic Random Pathfinding ===\n\n";

    // Setup a larger grid for a better maze experience
    Environment env(15, 15);

    // Define Start and Target locations
    Vector2D startLocation(0, 0);    // Top-Left
    Vector2D targetLocation(14, 12); // Bottom-Right

    // Generate random obstacles
    int obstacleCount = 50; // 40 obstacles on a 150-cell grid
    env.placeRandomObstacles(obstacleCount);

    // CRITICAL: Ensure the start and target cells are empty!
    // We don't want an obstacle spawning exactly on our goal or on top of our agent.
    env.clearCell(startLocation);
    env.clearCell(targetLocation);

    // Setup Sensor and Agent
    DistanceSensor mySensor(3);
    Agent myAgent(1, startLocation, &mySensor);
    myAgent.setTarget(targetLocation);

    // Place the agent on the map
    env.placeAgent(myAgent.getPosition());

    std::cout << "Starting State:\n";
    std::cout << "Agent Start: " << startLocation << "\n";
    std::cout << "Target Location: " << targetLocation << "\n";
    std::cout << "Obstacles Generated: " << obstacleCount << "\n\n";
    env.printGrid();
    pauseSimulation();

    // The Simulation Loop
    int step = 1;
    int maxSteps = 60; // Increased steps for a larger, more complex map

    while (!(myAgent.getPosition() == targetLocation) && step <= maxSteps)
    {
        std::cout << "--- Step " << step << " ---\n";

        env.placeRandomObstacles(obstacleCount);
        env.clearCell(myAgent.getPosition());
        env.clearCell(targetLocation);
        env.placeAgent(myAgent.getPosition());

        myAgent.decideNextMove(env);

        env.printGrid();

        // If the BFS logic turned off pathfinding because no path exists, break the loop early
        if (myAgent.getPosition() == startLocation && step > 1)
        {
            // Just a visual cue that it gave up
            std::cout << "Simulation halting due to unreachable target.\n";
            break;
        }

        pauseSimulation();
        step++;
    }

    std::cout << "\n=== SIMULATION RESULTS ===\n";
    if (myAgent.getPosition() == targetLocation)
    {
        std::cout << "SUCCESS: Agent successfully navigated the random minefield!\n";
    }
    else
    {
        std::cout << "FAILED: The agent could not reach the target. It might be completely walled off by random obstacles, or it ran out of steps!\n";
    }

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