#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <vector>

class Vector2D
{
public:
    float m_x;
    float m_y;

    Vector2D(float x = 0, float y = 0);

    Vector2D operator+(const Vector2D &) const; // position + direction
    Vector2D operator-(const Vector2D &) const;
    bool operator==(const Vector2D &) const;
    Vector2D &operator+=(const Vector2D &);
    Vector2D operator*(float) const;
    Vector2D &operator*=(float);
    friend std::ostream &operator<<(std::ostream &, const Vector2D &);

    void move(const Vector2D &velocity);

    double distance(const Vector2D &) const;

    double magnitude() const;

    void normalize();
};

// Vector2DHash (The Custom Hasher)
//     The Concept: unordered_set and unordered_map are incredibly fast because they turn data into a unique ID number (a hash). C++ knows how to hash an int or a std::string by default. But it has no idea how to hash your custom Vector2D object.
//     Why we use it: You provided a mathematical formula (using the bitwise XOR operator ^) to combine the x and y coordinates into a single, unique ID number so the set and map can function properly!
struct Vector2DHash
{
    std::size_t operator()(const Vector2D &v) const
    {
        return std::hash<int>()(v.m_x) ^ (std::hash<int>()(v.m_y) << 1);
    }
};

#endif


// This is a fantastic question. Hashing often feels like black magic until you look under the hood at the actual ones and zeros.

// To understand how your `Vector2DHash` works, we need to break it down into two parts: the **XOR operator (`^`)** and the **Bitshift operator (`<< 1`)**. 

// Here is the exact line of code we are examining:
// `return std::hash<int>()(v.m_x) ^ (std::hash<int>()(v.m_y) << 1);`

// *(Note: In most C++ compilers, the integer hash of a number is just the number itself. So for simplicity, we will treat `hash(x)` as just `x`).*

// ---

// ### 1. How XOR (`^`) Works
// XOR stands for **"Exclusive OR"**. It compares numbers at the binary level (bit by bit). 

// The rule for XOR is very simple: **If the two bits are different, the result is `1`. If they are the same, the result is `0`.**

// Let's look at `X = 5` and `Y = 3`:
// * Binary of 5: `0101`
// * Binary of 3: `0011`

// Let's XOR them (`5 ^ 3`) by comparing them vertically:
// ```text
//   0 1 0 1  (5)
// ^ 0 0 1 1  (3)
// ---------
//   0 1 1 0  (Result is 6)
// ```

// ### 2. The Problem: Why isn't XOR enough?
// If you only used `x ^ y` to generate your unique ID, your hash table would break immediately because of two mathematical rules of XOR:

// 1. **Self-Cancellation:** Any number XORed with itself is `0`. (`5 ^ 5 = 0`). This means every single diagonal coordinate on your grid `(1,1), (2,2), (3,3)` would all generate the exact same ID (`0`).
// 2. **Symmetry:** XOR doesn't care about order. `5 ^ 3` is `6`, and `3 ^ 5` is also `6`. This means the coordinate `(5, 3)` and the coordinate `(3, 5)` would generate the exact same ID. 

// When two different inputs produce the same ID, it is called a **Hash Collision**, and it slows your program down significantly. 

// ### 3. The Solution: The Bitshift (`<< 1`)
// To fix the symmetry problem, your code modifies the `Y` coordinate before applying the XOR. It uses `<< 1`, which shifts all the binary bits of `Y` to the left by one space. (In decimal terms, this is the equivalent of multiplying the number by 2).

// Let's see what happens to coordinate `(5, 3)` when we shift `Y` left by 1 (`3 << 1`):
// * Binary of 3: `0011`
// * Shift left : `0110` (This is now 6)

// Now we XOR the `X` value (5) with the *shifted* `Y` value (6):
// ```text
//   0 1 0 1  (X = 5)
// ^ 0 1 1 0  (Y shifted = 6)
// ---------
//   0 0 1 1  (Result is 3)
// ```
// Because we shifted `Y`, the coordinate `(5, 3)` produces an ID of `3`. 
// If we reversed it to `(3, 5)`, it would produce an ID of `9`. We have successfully destroyed the symmetry and created unique IDs!

// ### Are they perfectly unique?
// In the world of computer science, a 32-bit or 64-bit integer hash is never *perfectly* unique—eventually, with enough billions of combinations, two different coordinates will produce the same number. However, the `unordered_set` and `unordered_map` in C++ are designed to handle occasional collisions gracefully under the hood. 

// The goal of your hash function is just to make sure the numbers are scrambled and spread out enough that collisions are extremely rare, which this formula achieves beautifully.

// Here is an interactive visualizer where you can plug in X and Y coordinates to see exactly how the bitshift and the XOR operations calculate the final hash ID bit-by-bit!

// ```json?chameleon
// {"component":"LlmGeneratedComponent","props":{"height":"600px","prompt":"Create an interactive Bitwise Hash Visualizer to demonstrate how X ^ (Y << 1) works.\n\nData State:\n- Initial X = 5\n- Initial Y = 3\n\nStrategy: Form Layout.\n\nInputs:\n- Number input for 'X Coordinate' (allow 0 to 255).\n- Number input for 'Y Coordinate' (allow 0 to 255).\n\nBehavior:\n- Display the inputs as an interactive form at the top.\n- Below the inputs, display a step-by-step binary calculation.\n- Step 1: Show the 8-bit binary representation of X.\n- Step 2: Show the 8-bit binary representation of Y, and then visually show it being shifted left by 1 bit (Y << 1).\n- Step 3: Stack the binary of X and the binary of the shifted Y vertically to simulate an addition problem, with an XOR symbol (^) indicating the operation.\n- Step 4: Show the final calculated binary result below the stack, and convert it back to a Decimal number labeled 'Final Hash ID'.\n- Include a small 'XOR Legend' on the side or bottom explaining the rule: 0^0=0, 1^1=0, 1^0=1, 0^1=1.\n- Update the binary strings and results in real-time as the user changes the X and Y inputs.","id":"im_1afc2a593f9681f8"}}
// ```