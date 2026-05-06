#include <iostream>
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
// Note: We don't need to include vector2d.hpp directly because agent.hpp and environment.hpp already include it!

// Helper function to pause the console so we can watch the simulation step-by-step
void pauseSimulation() {
    std::cout << "\nPress Enter to continue to the next step...";
    std::cin.get();
    std::cout << "\n----------------------------------------\n\n";
}

int main()
{
    std::cout << "=== AUTONOMOUS AGENT SIMULATOR V1 ===\n\n";

    // 1. Initialize a 10x10 Environment
    Environment env(10, 10);

    // 2. Build a custom "maze" of obstacles
    // Creating a wall that the agent will have to navigate around
    env.placeObstacle(Vector2D(3, 0));
    env.placeObstacle(Vector2D(3, 1));
    env.placeObstacle(Vector2D(3, 2));
    env.placeObstacle(Vector2D(3, 3));
    env.placeObstacle(Vector2D(3, 4));
    
    // Add a few random obstacles on the other side
    env.placeObstacle(Vector2D(7, 7));
    env.placeObstacle(Vector2D(6, 8));

    // 3. Initialize the Agent
    // Start Agent 1 at the top-left corner (0,0)
    Agent agent(1, Vector2D(0, 0));
    
    // CRITICAL: Place the agent on the grid for the very first time!
    env.placeAgent(agent.getPosition());

    std::cout << "Starting State:\n";
    env.printGrid();
    pauseSimulation();

    // 4. Create a sequence of moves to test our logic
    // We will use standard Vector2D directions
    Vector2D moveRight(1, 0);
    Vector2D moveDown(0, 1);
    Vector2D moveLeft(-1, 0);
    Vector2D moveUp(0, -1);

    // Let's define a "flight plan" of velocities for the agent to follow
    std::vector<Vector2D> flightPlan = {
        moveRight, moveRight, moveRight, // Will hit the wall on the 3rd move!
        moveDown, moveDown, moveDown, moveDown, moveDown, // Move down to clear the wall
        moveRight, moveRight, moveRight, moveRight, // Move past the wall
        moveUp, moveUp, // Move up towards the top right
        moveRight, moveRight, moveRight, moveRight // Try to go off the edge of the map!
    };

    // 5. The Simulation Loop
    int stepNumber = 1;
    for (const Vector2D& currentVelocity : flightPlan) 
    {
        std::cout << "Step " << stepNumber << ": Agent attempting to move with velocity " << currentVelocity << "\n";
        
        // Give the agent its new velocity instructions
        agent.setVelocity(currentVelocity);

        // Tell the agent to execute the move
        agent.move(env);

        // Print the updated environment
        env.printGrid();
        
        pauseSimulation();
        stepNumber++;
    }

    std::cout << "=== SIMULATION COMPLETE ===\n";

    return 0;
}