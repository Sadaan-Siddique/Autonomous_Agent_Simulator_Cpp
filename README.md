# Autonomous Agent Environment Simulator (C++ & OpenGL) 🚀

A high-performance, from-scratch 2D robotic simulation environment built in C++. This project demonstrates advanced autonomous navigation, real-time SLAM (Simultaneous Localization and Mapping), and reactive obstacle avoidance using custom-built physics and rendering engines. 

## 🧠 Core Features & Architecture

This simulator is built heavily on robust **Object-Oriented Programming (OOP)** principles, utilizing tight **Encapsulation**, **Abstraction** (e.g., generic sensor interfaces), and **Composition** (Agents *have* sensors, Agents *have* internal maps) to maintain a scalable codebase.

### 🧱 Data Structures & Algorithms (DSA)
At its core, the simulator is driven by applied Data Structures and Algorithms:
* **Graph Theory & Traversal:** The environment and the agent's internal memory are represented as 2D matrices (grids) acting as implicit graphs.
* **A* (A-Star) Search:** Implements heuristic-driven shortest-path routing from the agent to the target, avoiding dynamic and static obstacles.
* **Breadth-First Search (BFS):** Utilized for structural reachability checks, deadlock detection, and identifying safe escape routes when the agent is cornered.
* **Point Clouds:** Sensory data is stored and processed using dynamic arrays of paired data types (`std::vector<std::pair<Vector2D, bool>>`) to map hit coordinates and wall status.
* **Bresenham's Line Algorithm:** Applied for discrete ray-casting in the LIDAR simulation to determine line-of-sight and wall intersections on a 2D grid.
* **Gradient Descent Smoothing:** Post-processes the blocky A* grid path into a fluid, drivable trajectory using mathematical optimization algorithms.

### 🤖 Robotics & Navigation
* **Real-Time SLAM:** The agent starts with a "Fog of War" and dynamically builds an internal grid map of its environment using sensory data.
* **LIDAR Sensor Simulation:** Implements a customizable ray-casting sensor (defaulting to 180° FOV with 30 laser rays). 
* **Live Memory Sync:** Zero-persistence dynamic mapping ensures the agent instantly updates its internal map to account for moving objects, avoiding "ghost trails."
* **PID Controller:** A custom Proportional-Integral-Derivative steering controller ensures fluid, realistic acceleration and turning, preventing robotic "snapping" to waypoints.

### ⚙️ Physics & Environment
* **Dynamic Reactive Obstacles:** Autonomous "enemy" blocks patrol the grid. They feature real-time collision detection, instantly rebounding off static walls, other obstacles, and the agent.
* **Procedural Generation:** The environment uses the modern C++ `<random>` library (Mersenne Twister) to populate the grid with a chaotic mix of static and dynamic obstacles upon every reset.
* **Custom Mathematics Module:** Built a lightweight, custom `Matrix` and `Vector2D` library to handle all 2D kinematics, dot products, vector normalization, and rotational transformations natively.

### 🎨 Rendering Engine
* **Raw OpenGL Pipeline:** Rendering is handled via a custom minimal wrapper over **OpenGL 3.3 Core Profile**, using **GLFW** for window management and **GLAD** for function pointers.
* **Shader Programming:** Custom vertex and fragment shaders (GLSL) handle screen scaling, offsets, and dynamic alpha-blending for the translucent LIDAR fan.

---

## 🛠️ Installation & Compilation

This project is fully cross-platform and compatible with Linux (tested on Arch Linux), Windows, and macOS. Ensure you have `g++` (or `clang++`), `glfw`, and `opengl` development packages installed on your system.

> ⚠️ **Important Warning regarding GLAD:** The `glad.c` and `glad/include` files provided in this repository are generated based on specific hardware and OpenGL drivers. Because GLAD configurations can be highly specific from GPU to GPU, using these exact files on a different graphics card might cause rendering failures or segmentation faults. If you experience issues, you will need to generate your own GLAD files for your specific GPU architecture using the [GLAD web service](https://glad.dav1d.de/) (Set to C/C++, OpenGL 3.3 Core) and replace the existing `glad` folder.

**1. Clone the repository:**
```bash
git clone [https://github.com/Sadaan-Siddique/Autonomous_Agent_Simulator_Cpp.git](https://github.com/Sadaan-Siddique/Autonomous_Agent_Simulator_Cpp.git)
cd Autonomous_Agent_Simulator_Cpp

```

**2. Compile the project:**
Run the compilation command corresponding to your Operating System.

**For Linux:**

```bash
g++ app/main.cpp src/core/vector2d.cpp src/math/matrix.cpp src/environment/environment.cpp src/environment/dynamicObstacle.cpp src/sensors/lidarSensor.cpp src/pathFinding/aStar.cpp src/pathFinding/pathSmoother.cpp src/agents/agent.cpp src/render/renderer.cpp glad/src/glad.c -o executables/main -I./glad/include -lglfw -lGL -ldl

```

**For Windows (MSYS2 / MinGW-w64):**

```bash
g++ app/main.cpp src/core/vector2d.cpp src/math/matrix.cpp src/environment/environment.cpp src/environment/dynamicObstacle.cpp src/sensors/lidarSensor.cpp src/pathFinding/aStar.cpp src/pathFinding/pathSmoother.cpp src/agents/agent.cpp src/render/renderer.cpp glad/src/glad.c -o executables/main.exe -I./glad/include -lglfw3 -lopengl32 -lgdi32 -luser32 -lshell32

```

**For macOS:**

```bash
clang++ app/main.cpp src/core/vector2d.cpp src/math/matrix.cpp src/environment/environment.cpp src/environment/dynamicObstacle.cpp src/sensors/lidarSensor.cpp src/pathFinding/aStar.cpp src/pathFinding/pathSmoother.cpp src/agents/agent.cpp src/render/renderer.cpp glad/src/glad.c -o executables/main -I./glad/include -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

```

**3. Run the simulator:**

**Linux / macOS:**

```bash
./executables/main

```

**Windows:**

```bash
./executables/main.exe

```

---

## 🎮 Simulator Controls

Once the OpenGL window launches, you can use the following keyboard controls to interact with the simulation:

* **[ SPACE ]** : Pause / Resume the simulation physics.
* **[ UP ARROW ]** : Increase simulation speed (Time Scale).
* **[ DOWN ARROW ]** : Decrease simulation speed (Time Scale).
* **[ CTRL + R ]** : Hard Reset. Wipes the agent's memory, reshuffles the maze, and assigns new random dynamic obstacles.

---

## 📸 Demo
![Simulator Demo](./assets/demo.png)
![Simulator Demo](./assets/demo_reached.png)
---

### Author

**Sadaan Siddique** | Computer Engineering

*Designed for exploration in low-level systems programming, graphics, and autonomous robotics.*

```

