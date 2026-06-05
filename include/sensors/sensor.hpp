#ifndef SENSOR_H
#define SENSOR_H

#include <vector>
#include <utility> // for std::pair
#include "../core/vector2d.hpp"
#include "../environment/environment.hpp"

// It's abstract because different sensors can detect differently, same interface for all

//  |  Feature  |  Virtual Function  |  Pure Virtual Function  |
//  | --- | --- | --- |
//  |  Definition  |  Has a body (implementation) in the base class.  |  Typically has no implementation in the base class.  |
//  |  Syntax  |  virtual void func() { ... }  |  virtual void func() = 0;  |
//  |  Mandatory Override  |  Optional; derived classes can use the base version.  |  Mandatory; derived classes must implement it to be concrete.  |
//  |  Abstract Class  |  Does not make a class abstract.  |  Makes the class abstract.  |
//  |  Instantiation  |  Objects of the base class can be created.  |  Objects of the base class cannot be created.  |

// Pure Virtual Function make the class abstract while virtual function doesn't

class Sensor
{
public:
    // Constructors can't be virtual
    virtual ~Sensor() = default; // destructors must be virtual if you plan to use Polymorphism.

    virtual float getRange() const = 0;
    virtual float getFOV() const = 0;
    virtual int getRayCount() const = 0;

    // Now returns the coordinate AND whether it hit a solid wall (true) or empty space (false)
    virtual std::vector<std::pair<Vector2D, bool>> scan(const Vector2D &agentPos, float heading, const Environment &env) const = 0; // The = 0 syntax defines a Pure Virtual Function.
                                                                                                                                    // It tells the compiler, "This function exists, but it has no body (no instructions) in this class.".
                                                                                                                                    //  It forces any "child" class (like distanceSensor) to provide its own logic for that function. If a child doesn't, it also becomes abstract and cannot be used.
                                                                                                                                    // No, the = 0 is a special marker, not a mathematical assignment. You cannot use = string or = 5. If you remove = 0, the function becomes a regular "Virtual Function," and you must provide a body (e.g., { return 0; })
};

#endif