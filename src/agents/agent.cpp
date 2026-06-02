#include "../../include/agents/agent.hpp"
#include <cmath>

// 1. What is Waypoint Navigation?

// Imagine using Google Maps. It doesn't just draw a single straight line from your house to a restaurant; it gives you a series of intermediate steps (turn right on Main St, go straight for 2 miles, turn left on 4th Ave).

// Waypoint Navigation is the exact same concept for autonomous robotics. Instead of giving the robot one final destination, you feed it a list of intermediate coordinates (waypoints). The robot's physics engine only worries about driving in a straight line to the very next waypoint. Once it hits that waypoint, it aims at the next one, playing "connect the dots" until it reaches the final target.

Agent::Agent(int id, const Vector2D &startPosition, Sensor *sensor) : m_id(id), m_position(startPosition), m_velocity(0, 0), m_sensor(sensor)
{
    // Note: If you want the agent to appear on the grid the moment it is created,
    // you will need to call env.placeAgent() in your main.cpp right after initializing the agent.

    // Initializing kinematics variables
    m_headingAngle = 0.0f; // 0 radians means facing perfectly Right/East
    m_speed = 3.0f;        // Moves 3 grid units per second
}

Vector2D Agent::getPosition() const { return m_position; }
void Agent::setVelocity(const Vector2D &velocity) { m_velocity = velocity; }
void Agent::setTarget(const Vector2D &target) { m_target = target; }

void Agent::move(Environment &env, float deltaTime)
{
    // this logic is basically teleporting my agent from one point in grid to other point (position + velosity)
    // Vector2D targetPosition = m_position + m_velocity;

    // if (env.isInsideBounds(targetPosition) && !env.isObstacle(targetPosition))
    // {
    //     env.clearCell(m_position);
    //     m_position = targetPosition;
    //     env.placeAgent(m_position); // placing agent in the environment
    // }
    // else
    // {
    //     std::cout << "Agent " << m_id << " is blocked!\n";
    // }

    // now, we no longer teleport our agent, we are implementing continuous motion, we use kinematic equation: Position = Position + (Velocity * Time).
    Vector2D targetPosition = m_position + (m_velocity * deltaTime);
    // Because targetPosition is now floating point (e.g., 1.5, 4.2),
    // Environment's isInsideBounds/isObstacle must cast it to int under the hood to check the grid array.
    if (env.isInsideBounds(targetPosition) && !env.isObstacle(targetPosition))
    {
        env.clearCell(m_position);
        m_position = targetPosition;
        env.placeAgent(m_position);
    }
    else
    {
        std::cout << "Agent " << m_id << " is blocked by physics collision!\n";
    }
}

void Agent::sense(Environment &env)
{
    // Generate the point cloud and save it to the agent's memory
    m_currentPointCloud = m_sensor->scan(m_position, m_headingAngle, env);
}

std::vector<Vector2D> Agent::getPointCloud() const { return m_currentPointCloud; }

// I'm changing this decide next move function because of Waypoint Navigation
// The BFS path is now treated as a list of exact physical coordinates.
// We calculate the delta vector between our current float position and the target waypoint,
// convert it to a heading angle, and use the Matrix to steer the velocity vector.

// void Agent::decideNextMove(Environment &env)
// {
//     if (m_position == m_target)
//     {
//         std::cout << "Target reached!\n";
//         return;
//     }

//     // 🔥 SWITCH HERE
//     if (m_usePathfinding)
//     {
//         // --- BFS PATH FOLLOWING ---

//         if (m_path.empty() || m_pathIndex >= m_path.size())
//         {
//             computePath(env);

//             // NEW FIX: If it's STILL empty after computing, just stop.
//             if (m_path.empty())
//                 return;
//         }

//         Vector2D nextPos = m_path[m_pathIndex];

//         m_velocity = nextPos - m_position;

//         if (sense(env) == 1)
//         {
//             std::cout << "Obstacle detected! Recomputing...\n";
//             computePath(env);
//             return;
//         }

//         move(env);
//         m_pathIndex++;
//     }
//     else
//     {
//         // --- OLD GREEDY LOGIC ---
//         Vector2D desiredDir = computeDirectionToTarget();

//         int obstacleDistance = m_sensor->detect(m_position, desiredDir, env);

//         if (obstacleDistance == -1 || obstacleDistance > 1)
//         {
//             m_velocity = desiredDir;
//             move(env);
//         }
//         else
//         {
//             chooseAlternativeDirection(env);
//             move(env);
//         }
//     }
// }

void Agent::decideNextMove(Environment &env, float deltaTime)
{
    // Target reached check requires a distance tolerance now, because we will rarely hit EXACTLY (14.000, 9.000)
    if (m_position.distance(m_target) < 0.1f)
    {
        // THE FIX: Snap the agent perfectly to the target coordinate
        // so OpenGL draws it exactly in the center of the cell!
        m_position = m_target;

        std::cout << "Target Reached!\n";
        m_velocity = Vector2D(0, 0); // Stop Moving
        return;
    }

    if (m_usePathfinding)
    {
        if (m_path.empty() || m_pathIndex >= m_path.size())
        {
            computePath(env);
            if (m_path.empty())
                return;
        }

        Vector2D nextWayPoint = m_path[m_pathIndex];

        // --- THE FRAME OVERSHOOT FIX ---
        float distanceToWaypoint = m_position.distance(nextWayPoint);
        float moveStep = m_speed * deltaTime; // How far the agent will travel this exact frame

        // 1. Check if we have arrived at the current waypoint
        if (moveStep >= distanceToWaypoint)
        {
            m_position = nextWayPoint;   // Snap perfectly to the waypoint
            m_velocity = Vector2D(0, 0); // Halt velocity for this frame
            m_pathIndex++;               // Queue up the next waypoint
            return;                      // End the frame here so we don't call move()
        }   

        // 2. Calculate the desired angle to the waypoint
        Vector2D deltaVec = nextWayPoint - m_position;
        float desiredHeadingAngle = std::atan2(deltaVec.m_y, deltaVec.m_x);

        // In a future update, you can add a PID controller here to slowly transition m_heading to desiredHeading.
        // For now, the steering snaps instantly to the correct angle.
        m_headingAngle = desiredHeadingAngle;

        // 3. Build the Rotation Matrix based on our heading
        Matrix rotationMatrix(2, 2);
        rotationMatrix.setValue(0, 0, std::cos(m_headingAngle));
        rotationMatrix.setValue(0, 1, -std::sin(m_headingAngle));
        rotationMatrix.setValue(1, 0, std::sin(m_headingAngle));
        rotationMatrix.setValue(1, 1, std::cos(m_headingAngle));

        // 4. Rotate our local forward vector into world velocity
        Vector2D localForward(1.0f, 0.0f);
        Vector2D worldDir = rotationMatrix * localForward;

        // --- PRECISION DRIFT FIX ---
        // If the velocity is microscopic, snap it to a perfect 0.0 to prevent drift
        if (std::abs(worldDir.m_x) < 0.001f) worldDir.m_x = 0.0f;
        if (std::abs(worldDir.m_y) < 0.001f) worldDir.m_y = 0.0f;

        m_velocity = worldDir * m_speed; // worldDir is a rotated vector and m_speed is the scalar

        // 5. Apply the physics
        move(env, deltaTime);
    }
    // else
    // {
    //     // Old greedy logic remains untouched for now
    //     // In computer science, a "Greedy Algorithm" is one that makes the most obvious, immediate best choice at the current moment without looking ahead at the big picture.
    //     Vector2D desiredDir = computeDirectionToTarget();
    //     int obstacleDistance = m_sensor->detect(m_position, desiredDir, env);

    //     if (obstacleDistance == -1 || obstacleDistance > 1)
    //     {
    //         m_velocity = desiredDir;
    //         move(env, deltaTime);
    //     }
    //     else
    //     {
    //         chooseAlternativeDirection(env);
    //         move(env, deltaTime);
    //     }
    // }
}

// Standard std::atan(val) only takes a single value and returns angles between \(-\frac{\pi }{2}\) and \(\frac{\pi }{2}\) (Quadrants I and IV). std::atan2 handles all four quadrants safely. It also Prevents Division-by-Zero

void Agent::chooseAlternativeDirection(Environment &env)
{
    // std::vector<Vector2D> directions = {
    //     {1,0}, {-1,0}, {0,1}, {0,-1} // For moving one step left, right bottom or top
    // };

    // for(const auto& dir : directions)
    // {
    //     Vector2D nextPos = m_position + dir;

    //     if(env.isInsideBounds(nextPos) && !env.isObstacle(nextPos))
    //     {
    //         m_velocity = dir;
    //         return;
    //     }
    // }

    // Pehle dekho ke agent jana kahan chahta tha
    Vector2D desiredDir = computeDirectionToTarget();
    std::vector<Vector2D> priorityDirections;

    // Agar horizontally jana chahta tha aur wall aagayi,
    // to pehle slide (Up/Down) try karo, peechay (reverse) jana aakhri option!
    if (desiredDir.m_x != 0)
    {
        priorityDirections = {{0, 1}, {0, -1}, {-desiredDir.m_x, 0}};
    }
    // Agar vertically hit kiya hai, to Left/Right slide try karo
    else
    {
        priorityDirections = {{1, 0}, {-1, 0}, {0, -desiredDir.m_y}};
    }

    for (const auto &dir : priorityDirections)
    {
        Vector2D nextPos = m_position + dir;

        if (env.isInsideBounds(nextPos) && !env.isObstacle(nextPos))
        {
            m_velocity = dir;
            return;
        }
    }
}

Vector2D Agent::computeDirectionToTarget() const
{
    Vector2D delta = m_target - m_position;

    // Restrict movement to 4-way (Orthogonal) by picking the longer axis first
    if (std::abs(delta.m_x) > std::abs(delta.m_y))
        return Vector2D(((delta.m_x > 0) ? 1 : -1), 0); // Move Horizontally
    else if (delta.m_y != 0)
        return Vector2D(0, ((delta.m_y > 0) ? 1 : -1)); // Move Vertically

    return Vector2D(0, 0);
}

void Agent::computePath(Environment &env)
{
    // THE FIX: Snap the float position to the nearest integer grid cell for the algorithm
    Vector2D gridStart(std::round(m_position.m_x), std::round(m_position.m_y));

    // Pass the rounded grid position to BFS, not the raw floating point
    m_path = BFS::findPath(gridStart, m_target, env);

    if (m_path.empty())
    {
        std::cout << "No path found! Target is physically unreachable.\n";
    }
    else
    {
        // FIX: Start at index 1, because index 0 is the cell the agent is already standing on!
        m_pathIndex = 1;
    }
}