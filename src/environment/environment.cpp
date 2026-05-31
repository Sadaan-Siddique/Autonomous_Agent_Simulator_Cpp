#include "../../include/environment/environment.hpp"
#include <random> // Add this for random number generation

Environment::Environment(int width, int height) : m_width(width), m_height(height), m_grid(height, std::vector<int>(width, (int)CellType::EMPTY)) {}

// bool Environment::isInsideBounds(int x, int y) const
// {                   // Checks whether a coordinate (x, y) is valid inside the grid.
//     // x >= 0       // not left of grid
//     // x < m_width  // not beyond right
//     // y >= 0       // not above grid
//     // y < m_height // not below grid
//     return x >= 0 && x < m_width && y >= 0 && y < m_height;
// }
bool Environment::isInsideBounds(const Vector2D& pos) const {
    return pos.m_x >= 0 && pos.m_x < m_width && pos.m_y >= 0 && pos.m_y < m_height;
}

// bool Environment::isObstacle(int x, int y) const { // Checks whether the given cell contains an obstacle.
//     if (!isInsideBounds(x, y)) return false; // to check whether the input is valid or not
//     return m_grid[x][y] == (int)CellType::OBSTACLE;
// }
bool Environment::isObstacle(const Vector2D& pos) const {
    if (!isInsideBounds(pos)) return false;
    return m_grid[pos.m_y][pos.m_x] == (int)CellType::OBSTACLE;
}

// void Environment::placeObstacle(int x, int y) // Marks a cell as an obstacle.
// {
//     if(isInsideBounds(x,y)) m_grid[x][y] = (int)CellType::OBSTACLE;
// }
void Environment::placeObstacle(const Vector2D& pos) {
    if(isInsideBounds(pos)) m_grid[pos.m_y][pos.m_x] = (int)CellType::OBSTACLE;
}

void Environment::placeAgent(const Vector2D& pos) {
    if(isInsideBounds(pos)) m_grid[pos.m_y][pos.m_x] = (int)CellType::AGENT;
}


// void Environment::clearCell(int x, int y) // Removes obstacle / resets cell, used to update environment, moving agents, resetting grid
// {
//     if(isInsideBounds(x, y)) m_grid[x][y] = (int)CellType::EMPTY;
// }
void Environment::clearCell(const Vector2D& pos) {
    if(isInsideBounds(pos)) m_grid[pos.m_y][pos.m_x] = (int)CellType::EMPTY;
}

int Environment::getWidth() const {
    return m_width; // Return your private width/cols variable
}

int Environment::getHeight() const {
    return m_height; // Return your private height/rows variable
}   

// void Environment::placeRandomObstacles(int count, const Vector2D& currentPos, const Vector2D& targetLocation)
void Environment::placeRandomObstacles(int count) {

    // clearCell(currentPos);
    // clearCell(targetLocation);
    for (int y = 0; y < m_height; y++) 
        for (int x = 0; x < m_width; x++)
            clearCell(Vector2D(y, x));
            // if(m_grid[y][x] == (int)CellType::OBSTACLE)
                // m_grid[y][x] = (int)CellType::EMPTY;

    // 1. Setup the random number generator
    std::random_device rd;  // Obtain a random seed from the hardware
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    
    // 2. Define the range for x (0 to width-1) and y (0 to height-1)
    std::uniform_int_distribution<> distX(0, m_width - 1);
    std::uniform_int_distribution<> distY(0, m_height - 1);

    // Prevent infinite loops if we ask for more obstacles than the grid can hold
    int maxObstacles = m_width * m_height;
    if (count > maxObstacles) count = maxObstacles;

    int placed = 0;
    while (placed < count) {
        // Generate random x and y coordinates
        int x = distX(gen);
        int y = distY(gen);
        
        // Only place an obstacle if the cell is currently empty
        if (m_grid[y][x] == (int)CellType::EMPTY) {
            m_grid[y][x] = (int)CellType::OBSTACLE;
            placed++;
        }
    }
}

void Environment::printGrid() const {
    for (int y = 0; y < m_height; y++) 
    {
        for (int x = 0; x < m_width; x++)
        {
            if(m_grid[y][x] == (int)CellType::OBSTACLE) std::cout << "# ";
            else if(m_grid[y][x] == (int)CellType::AGENT) std::cout << "A ";
            else std::cout << ". ";   
        }
        std::cout << "\n";
    }
}