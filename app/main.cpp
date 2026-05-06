#include <iostream>
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/distanceSensor.hpp"
#include "../include/pathFinding/bfs.hpp"

void pauseSimulation() {
    std::cout << "\nPress Enter to continue to the next step...";
    std::cin.get();
    std::cout << "\n----------------------------------------\n\n";
}

int main()
{
    std::cout << "=== AUTONOMOUS AGENT SIMULATOR V4 ===\n";
    std::cout << "=== BFS Pathfinding Mode ===\n\n";

    Environment env(10, 10);
    
    // Create a giant "C" shaped trap that would defeat the old greedy logic
    // Top wall
    env.placeObstacle(Vector2D(3, 2));
    env.placeObstacle(Vector2D(4, 2));
    env.placeObstacle(Vector2D(5, 2));
    env.placeObstacle(Vector2D(6, 2));
    // Back wall
    env.placeObstacle(Vector2D(6, 3));
    env.placeObstacle(Vector2D(6, 4));
    env.placeObstacle(Vector2D(6, 5));
    // Bottom wall
    env.placeObstacle(Vector2D(6, 6));
    env.placeObstacle(Vector2D(5, 6));
    env.placeObstacle(Vector2D(4, 6));
    env.placeObstacle(Vector2D(3, 6));

    // Setup Agent inside the trap
    DistanceSensor mySensor(3); 
    Agent myAgent(1, Vector2D(4, 4), &mySensor);
    
    // Set Target OUTSIDE the trap
    Vector2D targetLocation(9, 4);
    myAgent.setTarget(targetLocation);
    
    env.placeAgent(myAgent.getPosition());

    std::cout << "Starting State:\n";
    std::cout << "Agent Start (Inside Trap): " << myAgent.getPosition() << "\n";
    std::cout << "Target (Outside Trap): " << targetLocation << "\n";
    env.printGrid();
    pauseSimulation();

    int step = 1;
    int maxSteps = 40; 

    while (!(myAgent.getPosition() == targetLocation) && step <= maxSteps) 
    {
        std::cout << "--- Step " << step << " ---\n";
        
        myAgent.decideNextMove(env);

        env.printGrid();
        pauseSimulation();
        step++;
    }

    if (myAgent.getPosition() == targetLocation) {
        std::cout << "SUCCESS: BFS successfully navigated the maze!\n";
    } else {
        std::cout << "FAILED: The agent got stuck or ran out of steps.\n";
    }

    return 0;
}