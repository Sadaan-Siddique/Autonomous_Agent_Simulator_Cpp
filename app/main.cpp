#include <iostream>
#include "../include/environment/environment.hpp"
#include "../include/agents/agent.hpp"
#include "../include/sensors/distanceSensor.hpp" // Include the specific sensor

// Helper function to pause the console
void pauseSimulation() {
    std::cout << "\nPress Enter to continue to the next step...";
    std::cin.get();
    std::cout << "\n----------------------------------------\n\n";
}

int main()
{
    std::cout << "=== AUTONOMOUS AGENT SIMULATOR V2 (Sensors) ===\n\n";

    // 1. Initialize Environment
    Environment env(10, 10);
    
    // Create a vertical wall
    env.placeObstacle(Vector2D(5, 2));
    env.placeObstacle(Vector2D(5, 3));
    env.placeObstacle(Vector2D(5, 4));
    env.placeObstacle(Vector2D(5, 5));

    // 2. Initialize the Sensor
    // Create a distance sensor that can see up to 3 units away
    DistanceSensor mySensor(3); 

    // 3. Initialize the Agent
    // We pass the memory address (&) of our sensor into the agent
    Agent myAgent(1, Vector2D(1, 3), &mySensor);
    env.placeAgent(myAgent.getPosition());

    std::cout << "Starting State:\n";
    env.printGrid();
    pauseSimulation();

    // 4. Define our movement vectors
    Vector2D moveRight(1, 0);
    Vector2D moveDown(0, 1);

    // 5. The Sensor-Driven Simulation Loop
    // The agent wants to move right, but will use its sensor to avoid crashing
    for (int step = 1; step <= 8; step++) 
    {
        std::cout << "--- Step " << step << " ---\n";
        
        // Point the agent to the right so it can "look" that way
        myAgent.setVelocity(moveRight);
        
        // Ping the sensor!
        int distanceToHit = myAgent.sense(env);

        std::cout << "Agent is looking Right. Sensor returns: " << distanceToHit << "\n";

        // Decision Making Logic based on Sensor data
        if (distanceToHit > 0 && distanceToHit <= 1) {
            std::cout << ">> WARNING: Obstacle imminent! Changing direction to Down.\n";
            myAgent.setVelocity(moveDown);
        } else if (distanceToHit == -1) {
            std::cout << ">> Path is completely clear. Moving Right.\n";
        } else {
            std::cout << ">> Obstacle detected " << distanceToHit << " steps away. Continuing Right.\n";
        }

        // Execute the move
        myAgent.move(env);

        // Print the updated environment
        env.printGrid();
        
        pauseSimulation();
    }

    std::cout << "=== SIMULATION COMPLETE ===\n";

    return 0;
}