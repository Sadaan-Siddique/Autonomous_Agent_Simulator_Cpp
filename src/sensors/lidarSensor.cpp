#include "../../include/sensors/lidarSensor.hpp"
#include <cmath>

LidarSensor::LidarSensor(float range, float fov, int rayCount) : m_range(range), m_fov(fov), m_rayCount(rayCount) {}

float LidarSensor::getRange() const { return m_range; }
float LidarSensor::getFOV() const { return m_fov; }
int LidarSensor::getRayCount() const { return m_rayCount; }

std::vector<std::pair<Vector2D, bool>> LidarSensor::scan(const Vector2D &agentPos, float heading, const Environment &env) const
{
    std::vector<std::pair<Vector2D, bool>> pointCloud;

    // Calculate the angle between each ray
    float angleStep = (m_rayCount > 1) ? (m_fov / (float)(m_rayCount - 1)) : 0.0f;

    // Start sweeping from the left edge of the FOV
    float startAngle = heading - (m_fov / 2.0f);

    for (int i = 0; i < m_rayCount; i++)
    {
        float currentAngle = startAngle + (i * angleStep);

        // Build Rotation Matrix for this specific ray
        Matrix rotationMatrix(2, 2);
        rotationMatrix.setValue(0, 0, std::cos(currentAngle));
        rotationMatrix.setValue(0, 1, -std::sin(currentAngle));
        rotationMatrix.setValue(1, 0, std::sin(currentAngle));
        rotationMatrix.setValue(1, 1, std::cos(currentAngle));

        // Rotate our local forward vector in the direction of currentAngle
        Vector2D localForward(1.0f, 0.0f);
        Vector2D rayDir = rotationMatrix * localForward;

        // --- THE RAYCAST ---
        bool hitWall = false;
        Vector2D finalPos = agentPos;

        // Step forward along the ray in small increments (0.5 units)
        for (float step = 0.0f; step <= m_range; step += 0.5f)
        {
            Vector2D checkPos = agentPos + (rayDir * step);

            // If the ray goes off the map, stop this ray
            if (!env.isInsideBounds(checkPos))
            {
                break; // Stop ray, but it didn't hit a physical mapped wall
            }

            // If the ray hits a wall, record the exact float coordinate and stop this ray!
            if (env.isObstacle(checkPos))
            {
                hitWall = true;
                finalPos = checkPos;
                break; // Stop ray, recorded a wall hit!
            }
            finalPos = checkPos; // Keep extending the line through empty space
        }
        // Push the final coordinate AND the reason it stopped in pointCloud
        pointCloud.push_back({finalPos, hitWall});
    }
    return pointCloud;
}