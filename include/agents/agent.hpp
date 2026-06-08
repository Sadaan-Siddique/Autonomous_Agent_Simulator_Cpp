#ifndef AGENT_H
#define AGENT_H

#include "../core/vector2d.hpp"
#include "../math/matrix.hpp"
#include "../environment/environment.hpp"
#include "../sensors/sensor.hpp"
// #include "../pathFinding/bfs.hpp" // For using BFS, uncomment this
#include "../pathFinding/aStar.hpp"
#include "../pathFinding/pathSmoother.hpp"
#include "../math/kalmanFilter.hpp"
// Using Composition and aggregations
class Agent
{
private:
    int m_id{};
    Vector2D m_position; // Stores where agent currently is
    Vector2D m_velocity; // Stores movement direction/speed
    Vector2D m_target;
    Sensor *m_sensor;

    std::vector<Vector2D> m_path; // full path from start → target
    int m_pathIndex = 0;          // where agent currently is in path
    bool m_usePathfinding = true;
    bool m_isUnreachable = false; // NEW: Panic flag

    // Continuous Physical Properties
    float m_headingAngle;    // The angle the agent is currently facing (in radians)
    float m_speed;           // How fast the agent drives (units per second)
    float m_angularVelocity; // How fast the agent is currently spinning

    // PID Controller Variables
    float m_kp, m_ki, m_kd; // PID constants. Tuning these three numbers (Kp, Ki, Kd) changes how aggressive or sluggish the car feels.
    float m_previousError;
    float m_integral;

    std::vector<std::pair<Vector2D, bool>> m_currentPointCloud; // Stores the latest LIDAR scan
    std::vector<std::vector<int>> m_internalMap;                // The Agent's memory for SLAM Mapping

    // PROBABILISTIC MAPPING 
    std::vector<std::vector<float>> m_probMap; // Log-Odds map (Dimagh ke liye)
    
    // Log-Odds Constants (Tune these for different sensor behaviors) ' l = ln(p / (1 - p)) '; here p is accuracy of sensor like if p = 0.88, then it means our sensor is 88% accurate in identifying a wall

    // +2.0f ka asal matlab hai: "Mujhe 88% yaqeen hai ke yahan deewar hai." Because we have let p = 0.88, asal men ye values LIDAR ki specs pr depend krti hen. Agar acha LIDAR hai to 95%, low hai to 75%
    // l_(hit) = ln(0. 88 / (1 - 0.88)) = 2.0
    const float LOG_ODDS_HIT = 2.0f;       // Jab deewar mile to value taizi se barhao
    
    // let if sensor says that the path is empty, then the chances that its hyposthesis is wrong(ke asal men path empty nhi tha, agey obstacle hai lekin sensor wrongly keh de ke raasta khaali hai to ye type II error hai) is 27% (Probability of Type II error; Ho = Raasta bilkul khali hai, Null hypothesis ko accept kar lena jabke wo false ho.)
    // l_(miss) = ln(0. 27 / (1 - 0.27)) = -1.0
    const float LOG_ODDS_MISS = -3.0f;     // Jab rasta khali ho to value kam karo
    const float MAX_LOG_ODDS = 4.0f;      // Maximum Yaqeen (Wall)
    const float MIN_LOG_ODDS = -5.0f;     // Maximum Yaqeen (Empty Space)

    // Bresenham's algorithm to trace LIDAR rays, The algorithm's job is to look at the start and end points, and figure out every single grid cell the laser passed through in between them.
    // By making it private, you guarantee that the only thing allowed to alter the agent's memory is the agent's own LIDAR sensor.
    void bresenhamTrace(int x0, int y0, int x1, int y1, bool isHit, const Environment &env); 
    // x0, y0 (Start): This is the Agent's exact physical location on the grid. It is the origin point of the LIDAR laser.
    // x1, y1 (End): This is where the LIDAR laser finally stopped. It either hit a wall, or it reached its maximum range and faded out into empty space.

    KalmanFilter m_kf;

public:
    // For SLAM, agent needs to know how big its memory array should be
    Agent(int id, const Vector2D &startPosition, Sensor *sensor, int mapWidth, int mapHeight);

    Vector2D getPosition() const;
    float getHeading() const;
    bool isUnreachable() const;                                   // Getter for the renderer
    std::vector<std::pair<Vector2D, bool>> getPointCloud() const; // Add a getter so the Renderer can draw the dots
    const std::vector<std::vector<int>> &getInternalMap() const;

    void setVelocity(const Vector2D &);
    void setTarget(const Vector2D &target);

    // Method to wipe memory and restart
    void reset(const Vector2D &startPos);

    void move(Environment &, float deltaTime); // 'deltaTime' to properly calculate physics over time

    void sense(Environment &);

    // Autonomous Logic
    void decideNextMove(Environment &, float deltaTime);

    void computePath(Environment &env);
};

#endif