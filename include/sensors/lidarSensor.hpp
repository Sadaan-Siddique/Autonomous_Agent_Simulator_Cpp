#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H

#include "./sensor.hpp"
#include "../math/matrix.hpp"

class LidarSensor : public Sensor
{
private:
    // Imagine you are standing in a pitch-black room with a flashlight.
    // Your Position (agentPos) is where your feet are planted on the floor.
    // Your Field of View (FOV) is how wide the flashlight beam is.
    // Your Heading is the direction your chest is currently pointing.
    float m_range;
    float m_fov;    // Field of view in radians
    int m_rayCount; // How many lasers to shoot in that FOV (no. of lasers)

public:
    LidarSensor(float range, float fov, int rayCount);

    float getRange() const override;
    float getFOV() const override;
    int getRayCount() const override;

    // heading is the exact angle that the "front" of the agent is currently facing (like the direction of its chest)
    std::vector<std::pair<Vector2D, bool>> scan(const Vector2D &agentPos, float heading, const Environment &env) const override;
};

#endif