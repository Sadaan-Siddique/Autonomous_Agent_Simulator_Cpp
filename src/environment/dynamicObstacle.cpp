#include "../../include/environment/dynamicObstacle.hpp"

// Hum .cpp file ko kabhi #include kyun nahi karte?
// Yeh C++ ka sab se bada rule hai: "Never include a .cpp file."
// Wajah (The Linker Error):
// Jab aap compile karte hain, to compiler har .cpp file ko alag se parh kar ek "Object File" (.o) banata hai.
//     Agar aap Environment.cpp ko dynamicObstacle.cpp mein include kar lein ge, to Environment ka saara code do dafa (twice) compile ho jayega.
//     Aakhir mein jab Linker sab files ko jorne aayega, to woh kahega: "Bhai, Environment ke functions to do jagah par likhay hue hain, main konsa uthaon?"
// Isay kehte hain "Multiple Definition Error." Isliye hum hamesha sirf .hpp (Blueprints) share karte hain aur .cpp (Asal kaam) ko alag rakhte hain.

// --- ADD THIS LINE ---
// Implementation file mein actual header include karna zaroori hai
#include "../../include/environment/environment.hpp" 

DynamicObstacle::DynamicObstacle(const Vector2D &start, const Vector2D &end, float speed) : m_position(start), m_startPoint(start), m_endPoint(end), m_speed(speed), m_movingToEnd(true)
{
    // Direction vector calculate karo start se end ki taraf
    Vector2D dir = m_endPoint - m_startPoint;
    dir.normalize();
    m_velocity = dir * m_speed;
}

Vector2D DynamicObstacle::getPosition() const
{
    return m_position;
}

void DynamicObstacle::move(float deltaTime, const Vector2D &agentPos, const std::vector<DynamicObstacle> &allObs, int myID, const Environment &env)
{
    // 1. Calculate where the obstacle wants to go next
    Vector2D nextPos = m_position + (m_velocity * deltaTime);

    // --- NEW: COLLISION AVOIDANCE WITH AGENT ---
    // THE INSTANT REVERSE LOGIC
    // Agar agla qadam agent ke bilkul oopar ja raha hai (takrane wala hai)...
    if (nextPos.distance(agentPos) < 0.85f)
    {
        // ...to obstacle wahan rukne ke bajaye foran reverse ho jaye ga!
        m_velocity *= -1.0f;            // Velocity reverse kardi
        m_movingToEnd = !m_movingToEnd; // Target point (start/end) bhi switch kardiya

        // Is frame mein movement skip kardo taake agle frame se naye raste par jaye
        return;
    }

    // --- 2. COLLISION WITH STATIC ORANGE WALLS ---
    if (env.isStaticObstacle(nextPos))
    {
        m_velocity *= -1.0f;            // Takra kar wapas murein
        m_movingToEnd = !m_movingToEnd; // Target badlein
        return;
    }

    // --- 3. COLLISION WITH OTHER PINK OBSTACLES ---
    for (size_t i = 0; i < allObs.size(); i++)
    {
        if (i == myID)
            continue; // Apne aap se collision check nahi karna!
        // Agar koi doosra pink obstacle 0.85 units ke andar aagaya hai...
        if (nextPos.distance(allObs[i].getPosition()) < 0.85f)
        {
            m_velocity *= -1.0f;            // Bounce Back!
            m_movingToEnd = !m_movingToEnd; // Swap targets
            return;
        }
    }

    // --- 4. POSITION UPDATE ---
    m_position = nextPos;

    // Bouncing Logic: Agar end point par pohnch jaye, to wapas start ki taraf mure
    if (m_movingToEnd)
    {
        if (m_position.distance(m_endPoint) < 0.2f) // moving obstacles' endPoint
        {
            m_position = m_endPoint; // Snap to target
            m_movingToEnd = false;

            // Reverse velocity
            // Vector2D dir = m_startPoint - m_endPoint;
            // dir.normalize();
            // m_velocity = dir * m_speed;
            m_velocity = m_velocity * -1.0f; // Instantly reverse direction!
        }
    }
    else
    {
        // Agar wapas start par pohnch jaye, to dobara end ki taraf mure
        if (m_position.distance(m_startPoint) < 0.2f)
        {
            m_position = m_startPoint; // Snap to start
            m_movingToEnd = true;

            // Reverse velocity
            // Vector2D dir = m_endPoint - m_startPoint;
            // dir.normalize();
            // m_velocity = dir * m_speed;
            m_velocity *= -1.0f; // Instantly reverse direction!
        }
    }
}