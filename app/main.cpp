#include <iostream>
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/distanceSensor.hpp"

void pauseSimulation() {
    std::cout << "\nPress Enter to continue to the next step...";
    std::cin.get();
    std::cout << "\n----------------------------------------\n\n";
}

int main()
{
    std::cout << "=== AUTONOMOUS AGENT SIMULATOR V3 ===\n";
    std::cout << "=== Fully Autonomous Mode ===\n\n";

    // 1. Setup Environment
    Environment env(10, 10);
    
    // Create a wall in the middle of the grid
    env.placeObstacle(Vector2D(5, 2));
    env.placeObstacle(Vector2D(5, 3));
    env.placeObstacle(Vector2D(5, 4));
    env.placeObstacle(Vector2D(5, 5));
    env.placeObstacle(Vector2D(5, 6));

    // 2. Setup Sensor and Agent
    DistanceSensor mySensor(3); // detects 3 units long
    Agent myAgent(1, Vector2D(1, 4), &mySensor); // id = 1
    
    // 3. Set the Target on the other side of the wall
    Vector2D targetLocation(8, 4);
    myAgent.setTarget(targetLocation);
    
    // Place agent and target on grid (We will represent the target as 'T' in our minds, 
    // though the grid currently prints it as '.' unless you add a TARGET CellType to printGrid)
    env.placeAgent(myAgent.getPosition());

    std::cout << "Starting State:\n";
    std::cout << "Agent Start: " << myAgent.getPosition() << "\n";
    std::cout << "Target: " << targetLocation << "\n";
    env.printGrid();
    pauseSimulation();

    // 4. The Autonomous Simulation Loop
    int step = 1;
    int maxSteps = 30; // Safety limit to prevent infinite loops

    while (!(myAgent.getPosition() == targetLocation) && step <= maxSteps) 
    {
        std::cout << "--- Step " << step << " ---\n";
        
        // The agent does all the thinking and moving here!
        myAgent.decideNextMove(env);

        env.printGrid();
        pauseSimulation();
        step++;
    }

    if (myAgent.getPosition() == targetLocation) {
        std::cout << "SUCCESS: The agent autonomously navigated to the target!\n";
    } else {
        std::cout << "FAILED: The agent got stuck or ran out of steps.\n";
    }

    return 0;
}