#include "../../include/math/kalmanFilter.hpp"

KalmanFilter::KalmanFilter() 
    : x(4, 1), P(4, 4), F(4, 4), Q(4, 4), H(2, 4), R(2, 2), I(Matrix::identity(4))
{
    // 1. Initial State (Start at 0,0 with 0 velocity)
    x.setValue(0, 0, 0.0);
    x.setValue(1, 0, 0.0);
    x.setValue(2, 0, 0.0);
    x.setValue(3, 0, 0.0);

    // 2. Initial Uncertainty (P)
    // Shuru mein humein apni position ka theek se nahi pata, isliye high uncertainty (1000) rakhi hai.
    P = Matrix::identity(4) * 1000.0;

    // 3. Physics Model (F) - Kinematic equations
    F = Matrix::identity(4);

    // 4. Process Noise (Q)
    // Duniya perfect nahi hoti (friction, slip). Isliye hum apni math par 100% trust nahi karte.
    Q = Matrix::identity(4) * 0.01;

    // 5. Measurement Matrix (H)
    // Humara sensor (LIDAR/GPS) sirf x aur y coordinates deta hai, velocity nahi.
    // Ye matrix [x, y, vx, vy] me se sirf [x, y] ko extract karti hai.
    H.setValue(0, 0, 1.0);
    H.setValue(1, 1, 1.0);

    // 6. Sensor Noise (R)
    // Sensor kitna kharab hai. Agar LIDAR highly accurate hai to value choti hogi.
    R = Matrix::identity(2) * 0.1; 
}

void KalmanFilter::predict(float dt) {
    // x_new = x_old + vx * dt
    // y_new = y_old + vy * dt
    F.setValue(0, 2, dt); 
    F.setValue(1, 3, dt);
    // Equation 1: x = F * x
    x = F * x;
    
    // Equation 2: P = F * P * F^T + Q
    P = (F * P * F.transpose()) + Q;
}

void KalmanFilter::update(const Vector2D& measurement) {
    // Sensor ki 2D Vector reading ko 2x1 Matrix me convert kiya
    Matrix z(2, 1);
    z.setValue(0, 0, measurement.m_x);
    z.setValue(1, 0, measurement.m_y);

    // Equation 3: Kalman Gain K = P * H^T * (H * P * H^T + R)^-1
    Matrix Ht = H.transpose();
    Matrix S = (H * P * Ht) + R;
    Matrix K = P * Ht * S.inverse();

    // Equation 4: Update State x = x + K * (z - H * x)
    Matrix y = z - (H * x); // Innovation (Error between sensor and prediction)
    x = x + (K * y);

    // Equation 5: Update Covariance P = (I - K * H) * P
    P = (I - (K * H)) * P;
}

void KalmanFilter::setState(const Vector2D& pos, const Vector2D& vel) {
    x.setValue(0, 0, pos.m_x);
    x.setValue(1, 0, pos.m_y);
    x.setValue(2, 0, vel.m_x);
    x.setValue(3, 0, vel.m_y);
    
    // Confidence reset karein: Ab humein pakka pata hai hum kahan hain
    P = Matrix::identity(4) * 0.1; 
}

Vector2D KalmanFilter::getEstimatedPosition() const {
    // Filtered aur sab se accurate X aur Y wapas bhej do
    return Vector2D(x.getValue(0, 0), x.getValue(1, 0));
}