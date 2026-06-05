#pragma once

#include "../core/vector2d.hpp"

// C++ mein jab do files ek doosre ko #include karti hain, to "Circular Dependency" paida ho jati hai.
//     Environment.hpp kehta hai: "Mujhe DynamicObstacle dikhao taake main list bana sakoon."
//     DynamicObstacle.hpp kehta hai: "Mujhe Environment dikhao taake main takkar (collision) check kar sakoon."
// Compiler confuse ho jata hai ke pehle kisay parhay? Yeh bilkul aisa hai ke: "Murgi pehle aayi ya anda?"

// --- THE FIX: FORWARD DECLARATION ---
// Ye compiler ko batata hai ke Environment ek class hai, is circle toot jayega!
class Environment;

class DynamicObstacle
{
private:
    Vector2D m_position;   // Current float position
    Vector2D m_startPoint; // Patrol Start
    Vector2D m_endPoint;   // Patrol End
    Vector2D m_velocity;   // Current direction of movement
    float m_speed;         // Movement speed
    bool m_movingToEnd;    // State tracking for patrol loop

public:
    DynamicObstacle(const Vector2D &start, const Vector2D &end, float speed);

    // Getters
    Vector2D getPosition() const;

    // Logic
    void move(float deltaTime, const Vector2D& agentPos, const std::vector<DynamicObstacle>& allObs, int myID, const Environment& env);
};
