#include "../../include/agents/agent.hpp"
#include <random>
#include <cmath>

// 1. What is Waypoint Navigation?
// Imagine using Google Maps. It doesn't just draw a single straight line from your house to a restaurant; it gives you a series of intermediate steps (turn right on Main St, go straight for 2 miles, turn left on 4th Ave).
// Waypoint Navigation is the exact same concept for autonomous robotics. Instead of giving the robot one final destination, you feed it a list of intermediate coordinates (waypoints). The robot's physics engine only worries about driving in a straight line to the very next waypoint. Once it hits that waypoint, it aims at the next one, playing "connect the dots" until it reaches the final target.

Agent::Agent(int id, const Vector2D &startPosition, Sensor *sensor, int mapWidth, int mapHeight) : m_id(id), m_position(startPosition), m_velocity(0, 0), m_sensor(sensor), m_kf()
{
    // Initializing kinematics variables
    m_headingAngle = 0.0f;    // 0 radians means facing perfectly Right/East
    m_speed = 3.0f;           // Moves 3 grid units per second
    m_angularVelocity = 0.0f; // Starts out not spinning

    // --- INITIALIZE PID GAINS ---
    // You will tune these later to find the perfect physical steering feel
    m_kp = 1.5f; // Proportional (How aggressive it steers, left/right wobbling)
    m_ki = 0.0f; // Integral (Fixes mechanical drift - 0 for simulation)
    m_kd = 0.5f; // Derivative (The brakes that prevent overshooting/wobbling, dampening)

    m_kf.setState(startPosition);

    m_previousError = 0.0f;
    m_integral = 0.0f;

    m_internalMap = std::vector<std::vector<int>>(mapHeight, std::vector<int>(mapWidth, -1));
}

Vector2D Agent::getPosition() const { return m_position; }
float Agent::getHeading() const { return m_headingAngle; }
bool Agent::isUnreachable() const { return m_isUnreachable; }
std::vector<std::pair<Vector2D, bool>> Agent::getPointCloud() const { return m_currentPointCloud; }
const std::vector<std::vector<int>> &Agent::getInternalMap() const { return m_internalMap; }
void Agent::setVelocity(const Vector2D &velocity) { m_velocity = velocity; }
void Agent::setTarget(const Vector2D &target) { m_target = target; }

void Agent::reset(const Vector2D &startPos)
{
    m_position = Vector2D(0, 0);
    m_velocity = Vector2D(0, 0);
    m_angularVelocity = 0.0f;
    m_previousError = 0.0f;
    m_integral = 0.0f;
    m_headingAngle = 0.0f;
    m_path.clear();
    m_pathIndex = 0;
    m_currentPointCloud.clear();
    m_isUnreachable = false; // RESET THE ALARM
    m_kf.setState(startPos);

    // Wipe the SLAM memory back to Fog of War!
    for (int y = 0; y < m_internalMap.size(); y++)
    {
        for (int x = 0; x < m_internalMap[0].size(); x++)
        {
            m_internalMap[y][x] = -1;
        }
    }
}

void Agent::bresenhamTrace(int x0, int y0, int x1, int y1, bool isHit, const Environment &env)
{
    // dx and dy (Delta X / Delta Y): The absolute total distance the line travels horizontally and vertically. If you shoot a laser from (0,0) to (4, 3), dx is 4, and dy is 3.
    // sx and sy (Step X / Step Y): The direction the line is moving. If the laser shoots to the Right, sx is 1. If it shoots to the Left, sx is -1.
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    // err (The Error Accumulator): Because a grid is made of square blocks, you can't draw a perfectly smooth diagonal line. You have to choose which square block best represents the line. The err variable tracks how far "off-center" the true mathematical line is from the grid blocks.
    // e2 (Error x 2): A temporary variable. When the err accumulator gets too high, the algorithm uses e2 to trigger a "step" in the X or Y direction to correct the path and bring the blocks back to the center of the line.
    int err = dx + dy, e2;

    // In this specific function, width and height belong strictly to the Agent's internal map (m_internalMap), not the global Environment.
    int width = m_internalMap[0].size();
    int height = m_internalMap.size();

    // Get the absolute physical static grid to verify before erasing
    const auto& realGrid = env.getGrid();

    while (true)
    {
        if (x0 >= 0 && x0 < width && y0 >= 0 && y0 < height)
        {
            if (x0 == x1 && y0 == y1 && isHit)
            {
                m_internalMap[y0][x0] = 1; // Mark as Wall
            }
            else
            {
                // --- THE PERFECT CLEARING FIX ---
                // Agar physical environment mein yahan koi Static Obstacle (1) nahi hai,
                // Sirf tabhi is block ko erase karo. Ye Ghost Trails ko mita dega 
                // lekin Grazing Rays se static deewaron ko mehfooz rakhega!
                if (realGrid[y0][x0] != 1) 
                {
                    m_internalMap[y0][x0] = 0;
                }
            }
        }
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

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
   // Continuous motion using kinematic equation: Position = Position + (Velocity * Time)
    Vector2D targetPosition = m_position + (m_velocity * deltaTime);

    // --- THE DIRECTIONAL BUMPER FIX ---
    // Represents the physical width/hitbox of the agent's chassis
    float radius = 0.38f;
    bool collision = false;

    // 1. Center Point Check: Ensures the absolute center of the agent never phases through a wall
    if (!env.isInsideBounds(targetPosition) || env.isObstacle(targetPosition))
    {
        collision = true;
    }
    else
    {
        // 2. Vector Heading: Determine the actual direction the agent is physically moving
        float moveAngle = m_headingAngle;
        if (m_velocity.magnitude() > 0.01f)
        {
            moveAngle = std::atan2(m_velocity.m_y, m_velocity.m_x);
        }

        // 3. Directional 3-Point Cone: Casts "sensor probes" strictly in the direction of travel.
        // This prevents the rear/sides from snagging on walls when moving forward or reversing.
        // float angles[3] = {moveAngle, moveAngle - 0.6f, moveAngle + 0.6f};
        float angles[3] = {moveAngle, moveAngle - 0.5f, moveAngle + 0.5f};

        for (int i = 0; i < 3; i++)
        {
            Vector2D checkPos(targetPosition.m_x + std::cos(angles[i]) * radius,
                              targetPosition.m_y + std::sin(angles[i]) * radius);

            if (!env.isInsideBounds(checkPos) || env.isObstacle(checkPos))
            {
                collision = true;
                break;
            }
        }
    }

    if (!collision)
    {
        env.clearCell(m_position);
        m_position = targetPosition;
        env.placeAgent(m_position);
    }
    else
    {
        m_velocity = Vector2D(0, 0); // Hard brake on physical collision
        if (!(m_position == Vector2D(0, 0)))
            std::cout << "Agent has been Stucked!\n";
    }
}

// void Agent::sense(Environment &env)
// {
//     // Generate the point cloud and save it to the agent's memory
//     m_currentPointCloud = m_sensor->scan(m_position, m_headingAngle, env);

//     int startX = (int)m_position.m_x;
//     int startY = (int)m_position.m_y;

//     // Process every laser ray to update the internal map
//     for (const auto &ray : m_currentPointCloud)
//     {
//         int endX = (int)ray.first.m_x;
//         int endY = (int)ray.first.m_y;
//         bresenhamTrace(startX, startY, endX, endY, ray.second, env);
//     }
// }

void Agent::sense(Environment &env)
{
    // --- 1. THE BULLETPROOF GHOST ERASER ---
    // Puraani FOV wali logic hata di hai. Ab hum memory se har us deewar ko 
    // foran mita denge jo physical static grid (orange walls) ka hissa nahi hai.
    int mapW = m_internalMap[0].size();
    int mapH = m_internalMap.size();
    const auto& realGrid = env.getGrid();

    for (int y = 0; y < mapH; y++) {
        for (int x = 0; x < mapW; x++) {
            // Agar agent ki memory mein deewar (1) hai, lekin physical Environment 
            // ki static grid mein nahi hai (matlab wo ek pink moving block tha)...
            if (m_internalMap[y][x] == 1 && realGrid[y][x] != 1) {
                m_internalMap[y][x] = 0; // ...to us GHOST TRAIL ko foran erase kar do!
            }
        }
    }

    // --- 2. NORMAL LIDAR SCAN ---
    // Puraani trails mitane ke baad, ab normally scan karo.
    // Jo pink obstacles abhi LIDAR ke samne honge, wo dobara apni nayi current position par draw ho jayenge.
    m_currentPointCloud = m_sensor->scan(m_position, m_headingAngle, env);

    int startX = (int)m_position.m_x;
    int startY = (int)m_position.m_y;

    for (const auto &ray : m_currentPointCloud) {
        int endX = (int)ray.first.m_x;
        int endY = (int)ray.first.m_y;
        
        // Ye function static walls ko mehfooz rakhega aur naye hits ko map karega
        bresenhamTrace(startX, startY, endX, endY, ray.second, env); 
    }
}

// I'm changing this decide next move function because of Waypoint Navigation
// The BFS path is now treated as a list of exact physical coordinates.
// We calculate the delta vector between our current float position and the target waypoint,
// convert it to a heading angle, and use the Matrix to steer the velocity vector.

void Agent::decideNextMove(Environment &env, float deltaTime)
{
    // --- 1. TARGET REACHED FIX ---
    // Agar agent target ke 0.5f distance ke andar aa gaya hai, to gaari rok do.
    if (m_position.distance(m_target) < 0.5f)
    {
        m_position = m_target;
        m_velocity = Vector2D(0, 0);
        std::cout  << "Target Reached!\n";
        return;
    }

    if (m_usePathfinding)
    {
        static int stuckFrames = 0;
        static Vector2D lastPos = m_position;

        // --- NEW: 360-SWEEP ESCAPE VARIABLES ---
        static int escapeState = 0; // 0: Normal, 1: Backgear, 2: 360 Spin, 3: Pivot, 4: Drive Out
        static float escapeDirAngle = 0.0f;
        static int escapeMoveFrames = 0;
        
        static float spinAccumulator = 0.0f; // Track karta hai ke agent kitna ghoom chuka hai
        static float bestEscapeAngle = 0.0f; // Sab se safe raste ka angle save karega
        static float maxClearance = 0.0f;    // Sab se lambi khali jagah (clearance) save karega

        // --- 2. 360-DEGREE COLLISION RECOVERY STATE MACHINE ---
        // Jab agent stuck hota hai to ye 4-step logic chalti hai
        if (escapeState > 0)
        {
            if (escapeState == 1)
            {
                // STATE 1: BACKGEAR (Peeche aana)
                // Deewar se physically door hone ke liye peeche ki taraf drive karo
                m_velocity = Vector2D(std::cos(escapeDirAngle), std::sin(escapeDirAngle)) * m_speed;
                move(env, deltaTime);
                escapeMoveFrames--;
                if (escapeMoveFrames <= 0) {
                    escapeState = 2; // Backgear poora ho gaya, ab 360 ghoomne ki baari hai
                    spinAccumulator = 0.0f;
                    maxClearance = 0.0f;
                    bestEscapeAngle = m_headingAngle;
                    std::cout << "[SYSTEM] Backgear complete. Scanning for escape towards target...\n";
                }
            }
            else if (escapeState == 2)
            {
                // STATE 2: 360° BODY-AWARE RADAR SWEEP
                m_velocity = Vector2D(0, 0); 
                float spinSpeed = 5.0f; // Spin thora fast kiya
                float stepAngle = spinSpeed * deltaTime;
                m_headingAngle += stepAngle; 
                spinAccumulator += stepAngle;

                while (m_headingAngle > M_PI) m_headingAngle -= 2.0f * M_PI;
                while (m_headingAngle < -M_PI) m_headingAngle += 2.0f * M_PI;

                // --- THE CRITICAL FIX: BODY-WIDTH CHECK ---
                float currentClearance = 0.0f;
                Vector2D forward(std::cos(m_headingAngle), std::sin(m_headingAngle));
                Vector2D side(-forward.m_y, forward.m_x); // Perpendicular vector gaari ki chaurai ke liye

                for (float dist = 0.4f; dist <= 4.0f; dist += 0.4f) {
                    bool bodyFits = true;
                    // Check 3 points: Center, Left Shoulder (-0.35f), Right Shoulder (+0.35f)
                    // Ye ensure karega ke rasta itna khula ho ke poori gaari guzar sakay
                    float offsets[3] = {0.0f, -0.35f, 0.35f}; 
                    for(int j=0; j<3; j++) {
                        Vector2D checkPoint = m_position + (forward * dist) + (side * offsets[j]);
                        int cx = (int)checkPoint.m_x;
                        int cy = (int)checkPoint.m_y;

                        if (cx < 0 || cx >= m_internalMap[0].size() || cy < 0 || cy >= m_internalMap.size() || m_internalMap[cy][cx] == 1) {
                            bodyFits = false;
                            break; // Kisi bhi shoulder par obstacle laga to ye rasta reject kardo
                        }
                    }
                    if (!bodyFits) break; 
                    currentClearance += 0.4f;
                }

                // Check angle towards target
                Vector2D toTarget = m_target - m_position;
                float angleToTarget = std::atan2(toTarget.m_y, toTarget.m_x);
                float angleDiff = angleToTarget - m_headingAngle;
                while (angleDiff > M_PI) angleDiff -= 2.0f * M_PI;
                while (angleDiff < -M_PI) angleDiff += 2.0f * M_PI;

                // EARLY EXIT: Agar rasta gaari ki width ke liaz se khula hai AUR target ki taraf hai
                if (currentClearance >= 2.0f && std::abs(angleDiff) < (M_PI / 2.2f)) {
                    bestEscapeAngle = m_headingAngle;
                    escapeState = 4;
                    escapeMoveFrames = 35; // Stuck zone se nikalne ke liye lamba drive karo
                    std::cout << "[SYSTEM] Safe body-path found towards target. Escaping!\n";
                }

                if (currentClearance > maxClearance) {
                    maxClearance = currentClearance;
                    bestEscapeAngle = m_headingAngle;
                }

                // Agar 360 ghoom gaya aur target ki taraf rasta nahi mila, to fallback angle use karo
                if (spinAccumulator >= 2.0f * M_PI && escapeState == 2) {
                    escapeState = 3;
                    std::cout << "[SYSTEM] 360 Sweep complete. Using deepest available clearance.\n";
                }
            }
            else if (escapeState == 3)
            {
                // STATE 3: PIVOT TO SAFE ANGLE (Safe raste ki taraf murrna)
                // FALLBACK PIVOT (Sirf tab chalega agar State 2 mein early exit nahi mila)
                float error = bestEscapeAngle - m_headingAngle;
                while (error > M_PI) error -= 2.0f * M_PI;
                while (error < -M_PI) error += 2.0f * M_PI;

                if (std::abs(error) > 0.1f)
                {
                    // Agent ko us safe angle ki taraf ghoomaao
                    m_angularVelocity = error * 5.0f;
                    m_headingAngle += m_angularVelocity * deltaTime;
                    m_velocity = Vector2D(0, 0); 
                }
                else
                {
                    // Jab agent bilkul safe raste ki taraf dekhne lage, to Drive state mein jao
                    escapeState = 4; 
                    escapeMoveFrames = 40; // 40 frames tak straight safe raste mein bhaago
                }
            }
            else if (escapeState == 4)
            {
                // STATE 4: DRIVE OUT & REPLAN (Bhaago aur naya rasta dhoondo)
                m_velocity = Vector2D(std::cos(m_headingAngle), std::sin(m_headingAngle)) * m_speed;
                move(env, deltaTime);
                escapeMoveFrames--;
                
                if (escapeMoveFrames <= 0) {
                    // Safe zone mein aagaye. Escape logic band karo aur A* ko dobara path calculate karne ka kaho
                    escapeState = 0; 
                    computePath(env); 
                }
            }
            return; // Jab tak escape logic chal rahi hai, normal driving ko ignore karo
        }

        // --- 3. DYNAMIC REPLANNING (SLAM Integration) ---
        // Agar aage ke 3 waypoints par obstacle aagaya hai to naya path banao
        bool pathBlocked = false;
        if (!m_path.empty())
        {
            for (int i = m_pathIndex; i < std::min((int)m_path.size(), m_pathIndex + 3); i++)
            {
                if (m_internalMap[(int)m_path[i].m_y][(int)m_path[i].m_x] == 1)
                {
                    pathBlocked = true;
                    break;
                }
            }
        }

        if (m_path.empty() || pathBlocked)
        {
            computePath(env);
            if (m_path.empty()) return;
        }


        // ==========================================
        // THE KALMAN FILTER INTEGRATION PIPELINE
        // ==========================================
        
        // 1. GENERATE FAKE SENSOR NOISE
        // Asal hardware mein GPS/LiDAR kabhi exact position nahi dete.
        // Hum ek random normal distribution (Gaussian Curve) banayenge.
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::normal_distribution<float> noiseDistribution(0.0f, 0.05f); // Mean 0, Std Deviation 0.05 units

        float noiseX = noiseDistribution(gen);
        float noiseY = noiseDistribution(gen);
        
        // Noisy Measurement: Asal location mein kharaabi add kardi
        Vector2D noisySensorMeasurement(m_position.m_x + noiseX, m_position.m_y + noiseY);

        // 2. RUN THE FILTER
        // Pehle math se andaza lagao (Prediction)
        m_kf.predict(deltaTime);
        
        // Phir noisy sensor reading de kar update karo (Correction)
        m_kf.update(noisySensorMeasurement);

        // 3. EXTRACT THE PURE TRUTH
        // Filter sari math aur covariances ko use kar ke ek filtered location nikalega
        Vector2D estimatedPosition = m_kf.getEstimatedPosition();

        // ==========================================


        // --- 4. WAYPOINT PRUNING USING ESTIMATED POSITION ---
        // Hum path check karne ke liye real m_position ke bajaye Estimated Position use karenge!
        while (m_pathIndex < m_path.size() && estimatedPosition.distance(m_path[m_pathIndex]) < 0.3f)
        {
            if (m_pathIndex == m_path.size() - 1 && estimatedPosition.distance(m_path[m_pathIndex]) >= 0.15f) {
                break; 
            }
            m_pathIndex++;
        }
        if (m_pathIndex >= m_path.size())
        {
            computePath(env);
            return;
        }

        // --- 5. FULL PID KINEMATIC CONTROLLER ---
        Vector2D nextWayPoint = m_path[m_pathIndex];

        // The agent steers based on where it *thinks* it is (KF Output)
        // NOT where the physical engine knows it is (m_position)
        Vector2D deltaVec = nextWayPoint - estimatedPosition;

        float desiredAngle = std::atan2(deltaVec.m_y, deltaVec.m_x);
        
        float error = desiredAngle - m_headingAngle;
        while (error > M_PI) error -= 2.0f * M_PI;
        while (error < -M_PI) error += 2.0f * M_PI;

        // Proportional, Integral aur Derivative (PID) math
        m_integral += error * deltaTime;
        float derivative = (error - m_previousError) / deltaTime;
        m_angularVelocity = (m_kp * error) + (m_ki * m_integral) + (m_kd * derivative);
        m_previousError = error;

        // Mechanical Limits
        if (m_angularVelocity > 4.0f) m_angularVelocity = 4.0f;
        if (m_angularVelocity < -4.0f) m_angularVelocity = -4.0f;
        
        m_headingAngle += m_angularVelocity * deltaTime;

        // --- 6. FLUID THROTTLE (Continuous Driving) ---
        float alignment = std::cos(error);
        if (alignment < 0.1f) alignment = 0.1f; 
        
        m_velocity = Vector2D(std::cos(m_headingAngle), std::sin(m_headingAngle)) * (m_speed * alignment);

        // Apply physical movement (The actual 2D engine map gets updated here)
        move(env, deltaTime);

        // --- 7. STALL SENSING (Stuck Detection) ---
        if (m_position.distance(lastPos) < 0.005f)
        {
            stuckFrames++;
        }
        else
        {
            stuckFrames = 0;
        }
        lastPos = m_position;

        // --- 8. ESCAPE TRIGGER (Start 360-Sweep Recovery) ---
        if (stuckFrames > 15)
        {
            std::cout << "[SYSTEM] Deadlock detected! Executing 360-Sweep Recovery...\n";
            Vector2D safeDir(0, 0);
            
            // Backgear lagane ke liye 4 neighbours mein se koi thori si safe jagah dhoondo
            std::vector<Vector2D> checkDirs = {Vector2D(0, -1), Vector2D(0, 1), Vector2D(-1, 0), Vector2D(1, 0)};
            for (auto &d : checkDirs)
            {
                Vector2D test(m_position.m_x + d.m_x, m_position.m_y + d.m_y);
                if (env.isInsideBounds(test) && !env.isObstacle(test))
                {
                    safeDir = d;
                    break;
                }
            }
            
            if (safeDir.m_x != 0 || safeDir.m_y != 0)
            {
                // Trigger Phase 1: Backgear lagao aur variables reset karo taake agle frame se escape logic start ho
                escapeDirAngle = std::atan2(safeDir.m_y, safeDir.m_x);
                escapeState = 1;
                escapeMoveFrames = 20; 
                spinAccumulator = 0.0f;
                maxClearance = 0.0f;
            }
            else
            {
                // Agar 100% deewaron mein block ho to fallback par A* call kardo
                computePath(env);
            }
            stuckFrames = 0;
        }
    }
}





void Agent::computePath(Environment &env)
{
    int startX = (int)m_position.m_x;
    int startY = (int)m_position.m_y;

    // --- 1. START-LINE WALL ESCAPE LOGIC ---
    // If the agent spawns or is pushed perfectly inside an obstacle block, A* will fail immediately.
    // This routine checks the 8 neighboring cells, finds the nearest empty floor tile,
    // and manually snaps the agent's coordinates there to free it from the physics lock.
    if (startX >= 0 && startX < m_internalMap[0].size() && startY >= 0 && startY < m_internalMap.size())
    {
        if (m_internalMap[startY][startX] == 1)
        {
            bool foundSafe = false;
            for (int dy = -1; dy <= 1 && !foundSafe; dy++)
            {
                for (int dx = -1; dx <= 1 && !foundSafe; dx++)
                {
                    int nx = startX + dx, ny = startY + dy;
                    if (nx >= 0 && nx < m_internalMap[0].size() && ny >= 0 && ny < m_internalMap.size())
                    {
                        if (m_internalMap[ny][nx] == 0)
                        {
                            startX = nx;
                            startY = ny;

                            // Snap position to the safe integer grid coordinate
                            m_position.m_x = nx;
                            m_position.m_y = ny;

                            foundSafe = true;
                        }
                    }
                }
            }
        }
    }

    Vector2D gridStart((int)m_position.m_x, (int)m_position.m_y);
    Vector2D gridTarget((int)m_target.m_x, (int)m_target.m_y);
    
    // Pass strictly integer coordinates to the A* grid search
    m_path = AStar::findPath(gridStart, gridTarget, m_internalMap);

    if (!m_path.empty())
    {
        m_isUnreachable = false;
        
        // --- 2. HALLWAY CENTERING ---
        // A* generates waypoints on the absolute top-left corner (0.0) of every cell.
        // Adding +0.5 pushes the entire path sequence to the exact center of the corridors
        // so the agent's circular hitbox doesn't scrape the walls.
        for (auto &p : m_path)
        {
            p.m_x += 0.5f;
            p.m_y += 0.5f;
        }

        // --- 3. TARGET ARRIVAL FIX ---
        // Ensures the absolute final coordinate in the path array matches the exact float target.
        // This bridges the gap between the +0.5f centered path and the required stopping coordinate.
        if (m_path.back().distance(m_target) > 0.01f)
        {
            m_path.push_back(m_target);
        }

        m_pathIndex = 1; // Start aiming at index 1 (Index 0 is underneath the agent)
        
        // Pass the raw path to the PathSmoother to convert jagged 90-degree lines into fluid curves
        m_path = PathSmoother::smoothPath(m_path, m_internalMap, 0.1f, 0.2f, 0.001f);
    }
    else
    {
        m_isUnreachable = true;
        std::cout << "\n[ALARM] Target Location Blocked! Press Ctrl + R to reload.\n";
    }
}