Below is the same roadmap(as in file `team_distribution_suggestion.md`), but now clearly marked with:
* **[CORE / REQUIRED]** → Needed to satisfy your minimum proposal / MVP
* **[OPTIONAL / LATER]** → Beyond proposal, can be added after core project works

This way you can prioritize properly and distribute tasks without losing track of scope.

---

# FULL ORDERED ROADMAP

## Autonomous Agent Environment Simulator + Advanced Math Engine Extensions

---

# **PHASE 0 — Planning / Architecture** [CORE / REQUIRED]

### Tasks

* Finalize UML / class diagram
* Define module boundaries/interfaces
* Create GitHub repo + workflow
* Assign teammates to modules
* Define MVP vs optional extensions

### Why First

No coding should start before architecture is agreed.

---

# **PHASE 1 — Mathematical / Geometric Foundations** [CORE / REQUIRED]

> Everything depends on this.

---

## **1.1 Vector / Coordinate Geometry Module** [CORE / REQUIRED]

### Implement

* `Vector2D`
* Magnitude / normalization
* Dot product
* Distance calculations
* Angle between vectors
* Rotation helpers

### Used In

* Movement
* Sensors
* Physics
* Pathfinding heuristics

---

## **1.2 Matrix / Linear Algebra Module** [OPTIONAL / LATER]

### Implement

* Dynamic `Matrix` class
* Addition / subtraction
* Multiplication
* Scalar multiplication
* Transpose
* Determinant / inverse (optional)

### Why Optional

Useful for advanced math engine / transformations, but not required for MVP.

---

# **PHASE 2 — Core Simulation World** [CORE / REQUIRED]

---

## **2.1 Grid / Environment System** [CORE / REQUIRED]

### Implement

* `GridCell`
* `Environment/Grid`
* Obstacle placement
* Boundaries
* Occupancy map

---

## **2.2 CLI Visualization / Renderer** [CORE / REQUIRED]

### Implement

* Print grid
* Draw obstacles
* Draw agents
* Update simulation frame-by-frame

---

# **PHASE 3 — Agent Architecture** [CORE / REQUIRED]

---

## **3.1 Abstract Agent System** [CORE / REQUIRED]

### Implement

* Abstract `Agent`
* Derived `Car/Vehicle`
* Position / heading / velocity storage

---

## **3.2 Basic Kinematics / Physics Engine** [CORE / REQUIRED]

### Implement

* Position update
* Velocity update
* Direction update

### Physics Concepts

* Position
* Velocity
* Acceleration

---

## **3.3 Advanced Physics Constraints** [OPTIONAL / LATER]

### Add

* Friction / drag
* Turn radius
* Momentum-like constraints
* Energy consumption model

---

# **PHASE 4 — Sensor / Perception Layer** [CORE / REQUIRED]

---

## **4.1 Sensor Framework** [CORE / REQUIRED]

### Implement

* Abstract `Sensor`
* Distance / Proximity sensors

---

## **4.2 Advanced Sensor Models** [OPTIONAL / LATER]

### Add

* Field-of-view / cone sensors
* Multiple sensor types
* Sensor fusion

---

# **PHASE 5 — Navigation / Pathfinding** [CORE / REQUIRED]

---

## **5.1 Basic Pathfinding** [CORE / REQUIRED]

### Implement

* BFS / DFS / basic shortest-path logic

---

## **5.2 Advanced Pathfinding** [OPTIONAL / LATER]

### Add

* Dijkstra
* A*

---

# **PHASE 6 — Path Optimization / Smoothing** [OPTIONAL / LATER]

---

## Implement

* Remove redundant waypoints
* Smooth turns
* Minimize path length / cost

### Techniques

* Cost functions
* Heuristic optimization
* Splines / interpolation

---

# **PHASE 7 — Numerical Math Engine** [OPTIONAL / LATER]

> Build separately so it can integrate cleanly later.

---

## **7.1 Function Hierarchy** [OPTIONAL / LATER]

### Implement

* `MathFunction` abstract base
* Polynomial / Trig / Log functions

---

## **7.2 Numerical Differentiation** [OPTIONAL / LATER]

### Implement

[
f'(x)\approx \frac{f(x+h)-f(x)}{h}
]

---

## **7.3 Numerical Integration** [OPTIONAL / LATER]

### Implement

* Trapezoidal Rule
* Simpson’s Rule

---

# **PHASE 8 — Integrate Math Engine into Motion** [OPTIONAL / LATER]

---

## **8.1 Function-Based Motion Models** [OPTIONAL / LATER]

### Implement

[
x=f(t),\ y=g(t)
]

---

## **8.2 Derivative-Based Velocity/Acceleration** [OPTIONAL / LATER]

### Use

* Compute velocity from trajectory
* Compute acceleration from velocity

---

## **8.3 Integral-Based Metrics** [OPTIONAL / LATER]

### Use

* Total path length
* Energy estimation
* Area under sensor response curves

---

# **PHASE 9 — Probability / Statistics Layer** [OPTIONAL / LATER]

---

## **9.1 Sensor Noise Modeling** [OPTIONAL / LATER]

### Add

* Gaussian noise
* Uniform random error

---

## **9.2 Probabilistic Decision Making** [OPTIONAL / LATER]

### Use

* Weighted movement decisions
* Randomized exploration

---

## **9.3 Simulation Analytics** [OPTIONAL / LATER]

### Track

* Average travel time
* Collision rate
* Path efficiency

---

# **PHASE 10 — Multi-Agent System** [OPTIONAL / LATER]

---

## Implement

* Multiple autonomous agents
* Inter-agent collision avoidance
* Cooperative navigation

---

# **PHASE 11 — Persistence / Logging** [OPTIONAL / LATER]

---

## Implement

* Save simulation history
* Log path metrics
* Export analytics

---

# **PHASE 12 — Graphics / Visualization** [OPTIONAL / LATER]

---

## Implement

* OpenGL / SFML / GUI rendering
* Real-time animation

---

# PRIORITY BUILD ORDER SUMMARY

PHASE 0  → Planning
PHASE 1  → Vectors / Geometry
PHASE 2  → Environment / Grid / CLI
PHASE 3  → Agent / Basic Physics
PHASE 4  → Sensors
PHASE 5  → Basic Pathfinding
--------------------------------
↑ This completes your proposal MVP
--------------------------------
PHASE 6+ → Advanced Extensions
---

# MVP COMPLETION CHECKPOINT

Once you complete through **Phase 5**, you have:

### Fully Valid Minimum Project

* Autonomous agent
* Environment
* Obstacles
* Sensors
* Pathfinding
* CLI simulation

Everything after that is enhancement / bonus.

---

---

# Final Strategic Advice

Your development mindset should be:

> **Build the proposal first.**
> **Then add advanced math/science features incrementally.**

That way:

* You always have a working submission-ready project
* You can stop at any point if time runs short
* Every extra feature increases sophistication safely
