#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include "./sensor.hpp"

class DistanceSensor : public Sensor{
    private:
        int m_range;

    public:
        DistanceSensor(int range);
        int detect(const Vector2D& agentPos,
               const Vector2D& direction,
               const Environment& env) const override;
};


#endif