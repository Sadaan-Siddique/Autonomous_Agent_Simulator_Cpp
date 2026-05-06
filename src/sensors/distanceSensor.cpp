#include "../../include/sensors/distanceSensor.hpp"

DistanceSensor::DistanceSensor(int range) : m_range(range) {}

int DistanceSensor::detect(const Vector2D& agentPos,
                           const Vector2D& direction,
                           const Environment& env) const
{
    for(int i = 0; i <= m_range; i++)
    {
         Vector2D checkPos = agentPos + direction * i;

        if (!env.isInsideBounds(checkPos))
            return i;

        if (env.isObstacle(checkPos))
            return i;
    }

    return -1; // No obstacle within range
}

