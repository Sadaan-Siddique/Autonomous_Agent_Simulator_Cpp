#ifndef KALMAN_FILTER_HPP
#define KALMAN_FILTER_HPP

#include "./matrix.hpp"
#include "../core/vector2d.hpp"

class KalmanFilter
{
private:
    Matrix x; // State vector: [x, y, velocity_x, velocity_y]
    Matrix P; // Covariance matrix (Uncertainty/Confidence)
    Matrix F; // State Transition matrix (Kinematics)
    Matrix Q; // Process noise covariance (Physics model error)
    Matrix H; // Measurement matrix (Maps state to sensor data)
    Matrix R; // Measurement noise covariance (Sensor inaccuracy)
    Matrix I; // Identity matrix

public:
    KalmanFilter();

    void predict(float dt);
    void update(const Vector2D& measurement);
    void setState(const Vector2D& pos, const Vector2D& val = Vector2D(0, 0));

    Vector2D getEstimatedPosition() const;
};

#endif