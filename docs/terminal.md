Terminal is an application on your computer that allows you to control things via text-based commands instead of the normal point and click ways that we are used to doing things, it allows you to interact directly with your computer's operating system
It gives us VIP access to our computer
It gives us access to everything that is happening behind the scenes

An emulator is a piece of hardware or software designed to imitate another computer system or device. It allows your computer to act like a different piece of technology, so you can run programs or interact with systems meant for entirely different environments.
A terminal emulator is a software program that imitates a classic, physical computer terminal (like a screen and keyboard). It acts as a text-based bridge that allows you to interact directly with your computer's operating system.

Key ConceptsHow it works: 
Instead of clicking buttons and menus, you use a terminal emulator to type commands and view text-based outputs.
The "Shell": The emulator opens a window, but you actually type your commands into a "shell" (like Bash or Zsh) running behind the scenes.
Why it's called an emulator: In the 1950s and 1960s, computers were massive, and users interacted with them via physical, standalone hardware terminals. Modern terminal apps are software programs that emulate those historical physical machines.

Around 1950s, terminals were actually physical devices that were at the terminal end of a cable connected to a main frame
These early terminals weren't actually computers, they hav eenough memory to draw teh text on the screen and send input from the keyboard
So, the terminal application on our computers is an emulator of one of these earlier devices. These devives were also called a `console`.
The terminal actually requires a `shell` which is where you enter the commands, this is also referred to as a `command line`
There are various shells and these commands might be different depending on where you need to enter them like : windows : dir, linux and Macos : ls
Many code editors have these terminals embedded and that's because you use these to kick off your scripts


The terminal is incredibly slow for simulation because it is entirely CPU-Bound and I/O Bound.
When you call std::cout << "# ":

    Your CPU has to convert that string into bytes.

    The program pauses to ask the Operating System kernel for permission to write to the standard output stream.

    The OS passes those bytes to your terminal emulator (like Alacritty or GNOME Terminal).

    The terminal has to parse the text, figure out the font, apply any ANSI color codes, and draw the literal shapes of the letters.

Doing this sequentially for thousands of characters, frame by frame, completely overwhelms the CPU.


How OpenGL Solves This (The GPU Pipeline)?
OpenGL completely bypasses the text stream bottleneck by handing the work over to your Graphics Processing Unit (GPU). (yaani ye CPU ko dene ke bajaye ke wo rendering kare, GPU ko de deta hai ke tu sambhal)

    Parallel Processing: Your CPU is incredibly smart but only has a few cores (e.g., 8 or 16). Your GPU is relatively "dumb" but has thousands of cores. OpenGL takes your grid of 2,500 cells and tells the GPU to calculate and draw all 2,500 of them simultaneously in a fraction of a millisecond.

    VRAM (Video Memory): Instead of sending the map data every single frame like std::cout, OpenGL allows you to upload the grid coordinates to the GPU's memory (using Vertex Buffer Objects, or VBOs) once. For every frame after that, you just tell the GPU, "Draw the data you already have."

    Double Buffering: OpenGL renders the next frame on a hidden "Back Buffer." While the monitor is displaying Frame 1, the GPU is silently drawing Frame 2. When Frame 2 is ready, it instantly swaps them. This completely eliminates flickering and screen tearing.

With OpenGL, rendering a 100x100 grid of colored squares will easily run at 10,000+ FPS uncapped.



how it makes it possible to achieve 10000+ FPS?

The massive leap from 20 FPS in a terminal to 10,000+ FPS using OpenGL comes down to a fundamental difference in hardware architecture. It is the difference between doing things **sequentially** (one by one) and doing things in **parallel** (all at once).

To understand this, we have to look at the physical chips inside your computer.

### 1. The CPU (The Sports Car)

Your Intel CPU is a masterpiece of complex engineering. It has a few extremely fast, highly intelligent "cores" (usually 4, 8, or 16). It is designed to execute highly complex logic very quickly, but it can only do one thing at a time per core.

Think of the CPU like a Ferrari. It can drive incredibly fast, but it can only carry two passengers. If you need to move 10,000 pixels across the city, the Ferrari has to make 5,000 separate trips. When you use `std::cout` in the terminal, your CPU is driving every single `#` character to the screen one trip at a time.

### 2. The GPU (The Freight Train)

Your GPU (even the integrated Intel HD 520) is built completely differently. Instead of a few complex cores, a GPU contains **hundreds or thousands** of simple, "dumb" cores. They aren't good at complex logic like running operating systems or predicting branches of code, but they are exceptionally good at doing basic math simultaneously.

Think of the GPU like a massive freight train. It doesn't move as fast as the Ferrari, but it has 10,000 seats. If you need to move 10,000 pixels, the train takes everyone in a single trip.

### 3. How OpenGL Bridges the Gap

When you write OpenGL code, you are shifting the workload from the sports car to the train using a pipeline.

1. **Vertex Buffers (VBOs):** Instead of asking the CPU to redraw the 15x10 grid every frame, your C++ code sends the mathematical coordinates of the grid to the GPU's memory **exactly once** when the program starts.
2. **The Draw Call:** During your `while` loop, your C++ code simply yells at the GPU: `"Draw!"`.
3. **The Shaders (Parallel Execution):** The GPU takes that command and wakes up thousands of its tiny cores. If your grid has 150 cells, the GPU assigns 150 different physical cores to calculate the color of each cell simultaneously.

Because the GPU computes the entire grid at the exact same moment, drawing the frame takes a fraction of a millisecond. If a frame takes 0.1 milliseconds to draw, you hit 10,000 FPS.






Can you write your own windowing code from scratch?
Yes, absolutely, but you need to know exactly what "from scratch" means in C++.

Standard C++ does not know what a "window" or a "monitor" is. It only knows about memory and math. To draw a window, you have to talk directly to the Operating System.

On a Linux system like Arch, this means writing raw X11 (Xlib/XCB) or Wayland protocol code.

    The Reality: Writing an X11 window from scratch requires hundreds of lines of highly cryptic, OS-specific boilerplate code just to open a blank black square, listen for the "X" button to be clicked, and not crash. And if you compile that code and send it to a friend on Windows, it will completely fail, because Windows uses the Win32 API, not X11.

This is exactly why GLFW exists.
GLFW is the code that creates the window. It is a lightweight, open-source library that hides the OS nightmare from you. When you call glfwCreateWindow(), GLFW checks if you are on Arch, Windows, or Mac, translates your request into raw X11/Wayland/Win32 code, and gives you a blank canvas.

Using GLFW to open the window, and writing your own OpenGL shaders and math to draw the grid inside that window, is considered the industry standard for writing a custom graphics engine from scratch.



You absolutely can use C++ to talk directly to your OS to render a window. In fact, because C++ is a low-level systems language, it is the primary language used to do this.

C++ ko khud koi idea nhi hota ke window, screen, pixel wagera kiya hen? To draw a window, your C++ code has to ask the Operating System to do it for you using an OS-Specific API.
On Windows, you talk to the Win32 API. On macOS, you talk to Cocoa.
But because you are on Arch Linux, you have to talk to a Display Server—and that brings us exactly to X11 and Wayland.




Purpose of GLAD

If GLFW is the tool that builds your **Window**, GLAD is the tool that finds your **Graphics Card**.

To understand why GLAD is strictly necessary, you have to understand a very weird quirk about how OpenGL is designed: **OpenGL is not actually a piece of software; it is just a PDF document.**

It is a specification—a list of rules and function names (like `glClear` or `glDrawArrays`) created by the Khronos Group. The actual *code* that makes those functions work is written by Nvidia, AMD, or Intel, and it lives deep inside your specific graphics driver.

Here is the exact problem GLAD solves and how it fits into your simulator.

### The Problem: The Missing Pointers

When you `#include <GL/gl.h>` in your C++ code, you are just including a list of empty function names. Your C++ compiler knows those functions exist, but it has no idea where the actual implementation code lives in your computer's memory.

If you try to call `glClear()` without finding the actual driver code first, your program will instantly crash with a Segmentation Fault.

To fix this manually, you would have to ask your Operating System to search the GPU driver for the memory address of the function, and then assign it to a C++ pointer. On Linux, it looks like this:

```cpp
// You would have to do this for EVERY SINGLE OpenGL function you want to use.
typedef void (*GL_CLEAR_PTR)(unsigned int);
GL_CLEAR_PTR myGlClear = (GL_CLEAR_PTR)glXGetProcAddress("glClear");

myGlClear(GL_COLOR_BUFFER_BIT); 

```

Doing this manually for the hundreds of functions required to render a 2D grid would take thousands of lines of boilerplate code.

### The Solution: GLAD (The Phonebook)

GLAD is an **Extension Loader Library**. It is essentially an automated phonebook for your GPU.

Instead of writing all that manual pointer code, you go to the GLAD web portal, tell it which version of OpenGL you want to use (usually Version 3.3 Core Profile for modern 2D/3D graphics), and it generates a single `glad.c` file for you.

When you call `gladLoadGLLoader()` in your C++ code, GLAD rapidly searches your system's graphics drivers, finds the memory addresses for every single OpenGL function, and automatically wires them up.

### The Engine Initialization Order

Because GLAD needs to talk to the graphics driver, and the graphics driver needs a window to draw on, the initialization order in your `main.cpp` must be exactly this:

```cpp
// 1. Initialize GLFW and tell it to use Wayland/X11
glfwInit();

// 2. Create the actual Window (but it can't draw anything yet)
GLFWwindow* window = glfwCreateWindow(800, 600, "Simulator", NULL, NULL);
glfwMakeContextCurrent(window);

// 3. NOW initialize GLAD. 
// It looks at the window, finds the Intel HD 520 driver, and wires up the pointers.
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}

// 4. Now it is safe to use OpenGL math!
glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

```

### Summary of the Stack

To bring it all together for your Phase 1 rendering upgrade:

* **C++:** Contains your logic, BFS algorithms, and Matrix math.
* **Wayland/X11:** The Arch Linux systems that put the GUI on your screen.
* **GLFW:** Translates your C++ code into Wayland/X11 commands to open the Window.
* **GLAD:** Connects your C++ code directly to your Intel GPU driver so you can render the Agent and the Grid at 10,000+ FPS.