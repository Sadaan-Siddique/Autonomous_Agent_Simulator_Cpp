#include "../../include/environment/environment.hpp"
#include <random>    // Add this for random number generation
#include <algorithm> // std::shuffle ke liye

Environment::Environment(int width, int height) : m_width(width), m_height(height), m_grid(height, std::vector<int>(width, (int)CellType::EMPTY)) {}

// bool Environment::isInsideBounds(int x, int y) const
// {                   // Checks whether a coordinate (x, y) is valid inside the grid.
//     // x >= 0       // not left of grid
//     // x < m_width  // not beyond right
//     // y >= 0       // not above grid
//     // y < m_height // not below grid
//     return x >= 0 && x < m_width && y >= 0 && y < m_height;
// }
bool Environment::isInsideBounds(const Vector2D &pos) const
{
    // Add a tiny epsilon (-0.01f) to the zero-bounds to prevent floating-point
    // precision from falsely flagging a perfect 0.0 as negative out-of-bounds.
    return pos.m_x >= -0.01f && pos.m_x < m_width &&
           pos.m_y >= -0.01f && pos.m_y < m_height;
}

bool Environment::isObstacle(const Vector2D &pos) const
{ // Checks whether the given cell contains an obstacle.
    if (!isInsideBounds(pos))
        return false; // to check whether the input is valid or not

    // 1. Static Grid Check
    if (m_grid[(int)pos.m_y][(int)pos.m_x] == (int)CellType::OBSTACLE)
        return true;

    // 2. Dynamic Obstacle Check (Treating them as 1x1 physical blocks)
    for (const auto &obs : m_dynamicObstacles)
    {
        // Agar current check position moving block ke center se 0.5 distance ke andar hai,
        // to iska matlab wahan physical block mojood hai!
        if (obs.getPosition().distance(pos) <= 0.6f)
            return true;
    }

    return false;
}

// Yeh sirf physical orange walls check karega
bool Environment::isStaticObstacle(const Vector2D &pos) const
{
    if (!isInsideBounds(pos)) return false; 
    return m_grid[(int)pos.m_y][(int)pos.m_x] == (int)CellType::OBSTACLE;
}

void Environment::placeObstacle(const Vector2D &pos)
{ // Marks a cell as an obstacle.
    // if(isInsideBounds(pos)) m_grid[pos.m_y][pos.m_x] = (int)CellType::OBSTACLE;
    if (isInsideBounds(pos))
        m_grid[(int)pos.m_y][(int)pos.m_x] = (int)CellType::OBSTACLE;
}

void Environment::placeAgent(const Vector2D &pos)
{
    // if(isInsideBounds(pos)) m_grid[pos.m_y][pos.m_x] = (int)CellType::AGENT;
    if (isInsideBounds(pos))
        m_grid[(int)pos.m_y][(int)pos.m_x] = (int)CellType::AGENT;
}

void Environment::clearCell(const Vector2D &pos)
{ // Removes obstacle / resets cell, used to update environment, moving agents, resetting grid
    // if(isInsideBounds(pos)) m_grid[pos.m_y][pos.m_x] = (int)CellType::EMPTY;
    if (isInsideBounds(pos))
        m_grid[(int)pos.m_y][(int)pos.m_x] = (int)CellType::EMPTY;
}

int Environment::getWidth() const
{
    return m_width; // Return your private width/cols variable
}

int Environment::getHeight() const
{
    return m_height; // Return your private height/rows variable
}

std::vector<std::vector<int>> Environment::getGrid() const { return m_grid; }

// void Environment::placeRandomObstacles(int count, const Vector2D& currentPos, const Vector2D& targetLocation)
void Environment::placeRandomObstacles(int count)
{

    // 1. Clear Static Grid
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            clearCell(Vector2D(x, y)); // FIXED: x comes first
        }
    }

    // 2. Clear previous moving obstacles (For Ctrl+R Reset)
    m_dynamicObstacles.clear();

    // i. Setup the random number generator
    std::random_device rd;  // Obtain a random seed from the hardware
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

    // ii. Define the range for x (0 to width-1) and y (0 to height-1)
    std::uniform_int_distribution<> distX(0, m_width - 1);
    std::uniform_int_distribution<> distY(0, m_height - 1);

    // Prevent infinite loops if we ask for more obstacles than the grid can hold
    int maxObstacles = m_width * m_height;
    if (count > maxObstacles)
        count = maxObstacles;

    // Temporary array to remember where we placed obstacles
    std::vector<Vector2D> placedPositions;

    int placed = 0;
    while (placed < count)
    {
        // Generate random x and y coordinates
        int x = distX(gen);
        int y = distY(gen);

        // Only place an obstacle if the cell is currently empty
        if (m_grid[y][x] == (int)CellType::EMPTY)
        {
            m_grid[y][x] = (int)CellType::OBSTACLE;
            // Store center of the cell
            placedPositions.push_back(Vector2D(x + 0.5f, y + 0.5f));
            placed++;
        }
    }

    // --- DYNAMIC OBSTACLE LOGIC ---
    // Generate a random percentage between 30% (0.3) and 60% (0.6)
    std::uniform_real_distribution<float> distPercent(0.3f, 0.6f);
    float dynamicPercent = distPercent(gen);
    int dynamicCount = (int)(count * dynamicPercent);

    // Shuffle the placed positions so we pick random ones to make dynamic
    std::shuffle(placedPositions.begin(), placedPositions.end(), gen);

    // Speed randomizer: Move between 1.0 (Slow) and 3.5 (Fast) blocks per second
    std::uniform_real_distribution<float> distSpeed(1.0f, 3.5f);

    for (int i = 0; i < dynamicCount; i++)
    {
        Vector2D startPos = placedPositions[i];

        // CRITICAL: Remove it from the static grid!
        // Warna iski starting point par hamesha ek fake deewar reh jayegi.
        clearCell(startPos);

        // Generate a random END point for this specific obstacle
        Vector2D endPos(distX(gen) + 0.5f, distY(gen) + 0.5f);

        // Make sure it doesn't randomly pick its own start point as the end point
        while (endPos.distance(startPos) < 1.0f)
        {
            endPos = Vector2D(distX(gen) + 0.5f, distY(gen) + 0.5f);
        }

        float speed = distSpeed(gen);

        // Create the moving obstacle and add it to our list
        m_dynamicObstacles.push_back(DynamicObstacle(startPos, endPos, speed));
    }
}

void Environment::printGrid() const
{
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            if (m_grid[y][x] == (int)CellType::OBSTACLE)
                std::cout << "# ";
            else if (m_grid[y][x] == (int)CellType::AGENT)
                std::cout << "A ";
            else
                std::cout << ". ";
        }
        std::cout << "\n";
    }
}

// --- Update all moving obstacles based on time ---
void Environment::updateDynamicObstacles(float deltaTime, const Vector2D &agentPos)
{
    // for(auto& obs : m_dynamicObstacles)
    //     obs.move(deltaTime, agentPos, ); // Pass agent position to each obstacle
    for (size_t i = 0; i < m_dynamicObstacles.size(); i++)
        m_dynamicObstacles[i].move(deltaTime, agentPos, m_dynamicObstacles, i, *this); // Pass the full vector and the current index 'i' so it knows which one it is
}

const std::vector<DynamicObstacle> &Environment::getDynamicObstacles() const
{
    return m_dynamicObstacles;
}
