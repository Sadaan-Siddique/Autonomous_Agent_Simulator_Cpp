#include "../../include/render/renderer.hpp"
#include <iostream>
#include <cmath>

// Initialization and Windowing

Renderer::Renderer(int screenWidth, int screenHeight, const char *title)
{
    // 1. Initialize GLFW (The window Manager)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. create the window
    m_window = glfwCreateWindow(screenWidth, screenHeight, title, NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    // 3. Load opengl funtion pointer with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    // Enable Transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the rendering area
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup GPU Memory and Shaders
    initShaders();
    initPrimitives();
}

Renderer::~Renderer()
{
    glfwTerminate();
}

GLFWwindow *Renderer::getWindow() const { return m_window; }

bool Renderer::isRunning() const
{
    return !glfwWindowShouldClose(m_window);
}

void Renderer::clearScreen(bool isAlarmActive) const {
    if (isAlarmActive) {
        // If the agent is trapped, clear the screen with a Dark Red color
        glClearColor(0.4f, 0.0f, 0.0f, 1.0f);
    } else {
        // Normal Dark Charcoal background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::swapBuffers() const
{
    glfwSwapBuffers(m_window);
}

void Renderer::pollEvents() const
{
    glfwPollEvents();
}

// GLFW Setup: This bypasses the need to write raw X11 protocol code for an Arch system. It opens a window natively.

// GLAD Setup: OpenGL is just a specification document. It doesn't actually contain the code to draw things. GLAD talks to the specific driver of your Intel HD Graphics 520 hardware to find the exact memory addresses of the functions that draw pixels.

// Shaders And GPU Memory

void Renderer::initShaders()
{
    // 1. Vertex Shader: Handles Positions
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec2 aPos;\n"
                                     "uniform vec2 u_offset;\n"
                                     "uniform vec2 u_scale;\n"
                                     "void main() {\n"
                                     "   gl_Position = vec4((aPos * u_scale) + u_offset, 0.0, 1.0);\n"
                                     "}\0";

    // 2. Fragment Shader: Handles Colors
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 u_color;\n"
                                       "void main() {\n"
                                       "   FragColor = u_color;\n"
                                       "}\n\0";

    // Compile and link them into m_shaderProgram
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);
    glLinkProgram(m_shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Renderer::initPrimitives()
{
    // --- 1. THE QUAD (For Environment Walls) ---
    float quadVertices[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // --- 2. THE LINE (For LIDAR Beams) ---
    glGenVertexArrays(1, &m_lineVAO);
    glGenBuffers(1, &m_lineVBO);
    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    // We allocate empty memory using GL_DYNAMIC_DRAW because line coordinates change every frame
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // --- 3. THE ARROW (For the Agent, holds TWO triangles to make a chevron: 6 points = 12 floats) ---
    glGenVertexArrays(1, &m_arrowVAO);
    glGenBuffers(1, &m_arrowVBO);
    glBindVertexArray(m_arrowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_arrowVBO);
    // Allocate empty memory for a triangle (6 points * 2 coordinates = 12 floats)
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 4. FOV FAN (Holds Center + Up to 100 Ray Hits = 202 floats)
    glGenVertexArrays(1, &m_fovVAO);
    glGenBuffers(1, &m_fovVBO);
    glBindVertexArray(m_fovVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_fovVBO);
    glBufferData(GL_ARRAY_BUFFER, 400 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

// Shaders: Modern GPUs refuse to draw anything unless you give them a program (written in GLSL) telling them exactly how to calculate the geometry and color. We hardcode them here as strings so you don't have to deal with file I/O loading just yet.

// The u_offset and u_scale: Instead of uploading 100 different squares for your grid, we upload ONE generic 1x1 square to the m_VBO. We then use these "uniforms" to tell the GPU to scale and slide that single square across the screen like a rubber stamp.

// The Math and Drawing Logic
// This maps your discrete int grid to the monitor.

void Renderer::drawQuad(float x, float y, float width, float height, float r, float g, float b) const
{
    glUseProgram(m_shaderProgram);

    // Pass the color to the fragment shader
    int colorLoc = glGetUniformLocation(m_shaderProgram, "u_color");
    glUniform4f(colorLoc, r, g, b, 1.0f); // (Hardcoded 1.0f alpha for solid walls)

    // Pass the scale and position to the vertex shader
    int scaleLoc = glGetUniformLocation(m_shaderProgram, "u_scale");
    glUniform2f(scaleLoc, width, height);

    int offsetLoc = glGetUniformLocation(m_shaderProgram, "u_offset");
    glUniform2f(offsetLoc, x, y);

    // Draw the stamped square
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a) const
{
    glUseProgram(m_shaderProgram);

    // Turn off shader scaling/offsets so we can use exact screen coordinates
    int scaleLoc = glGetUniformLocation(m_shaderProgram, "u_scale");
    glUniform2f(scaleLoc, 1.0f, 1.0f);

    int offsetLoc = glGetUniformLocation(m_shaderProgram, "u_offset");
    glUniform2f(offsetLoc, 0.0f, 0.0f);

    int colorLoc = glGetUniformLocation(m_shaderProgram, "u_color");
    glUniform4f(colorLoc, r, g, b, a); // Accepts custom alpha for fading beams

    // Inject the two specific coordinates into the GPU line buffer
    float vertices[4] = {x1, y1, x2, y2};
    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // Command the GPU to connect the dots!
    glDrawArrays(GL_LINES, 0, 2);
}

void Renderer::renderEnvironment(const Environment &env)
{
    // 1. Ask the environment for its dynamic dimensions and cast them to floats for decimal math.
    float gridWidth = (float)env.getWidth();
    float gridHeight = (float)env.getHeight();

    // 2. OpenGL's screen goes from -1.0 to 1.0 (a total width/height of 2.0).
    // We divide 2.0 by our grid dimensions to find out exactly how much screen space ONE cell takes up.
    float cellWidth = 2.0f / gridWidth;
    float cellHeight = 2.0f / gridHeight;

    // 3. Loop through every single row (Y axis) from 0 up to the dynamic height.
    for (int y = 0; y < env.getHeight(); y++)
    {

        // 4. Loop through every single column (X axis) in the current row from 0 up to the dynamic width.
        for (int x = 0; x < env.getWidth(); x++)
        {

            // 5. Map the discrete X grid coordinate (0, 1, 2...) to OpenGL's -1.0 to 1.0 screen coordinate.
            float screenX = -1.0f + (x * cellWidth);

            // 6. Map the Y coordinate, but invert it! OpenGL's Y grows UPwards, but standard arrays/grids grow DOWNwards.
            float screenY = 1.0f - ((y + 1) * cellHeight);

            // 7. Ask the environment object if this specific (x, y) coordinate is a wall.
            if (env.isObstacle(Vector2D(x, y)))
            {

                // 8. It IS a wall. Draw a quad here using a Light Gray color (R:0.7, G:0.7, B:0.7).
                // The 0.95f multiplier shrinks the square by 5% to create a visual gap (grid lines) between cells.
                drawQuad(screenX, screenY, cellWidth * 0.95f, cellHeight * 0.95f, 0.7f, 0.7f, 0.7f);
            }
            else
            {

                // 9. It is NOT a wall (Empty Space). Draw a quad here using a Dark Gray color (R:0.2, G:0.2, B:0.2).
                drawQuad(screenX, screenY, cellWidth * 0.95f, cellHeight * 0.95f, 0.2f, 0.2f, 0.2f);
            }
        }
    }
}

void Renderer::renderAgent(const Agent &agent, const Environment &env)
{
    float cellWidth = 2.0f / (float)env.getWidth();
    float cellHeight = 2.0f / (float)env.getHeight();

    Vector2D pos = agent.getPosition();

    // 1. Find the exact mathematical center of the cell on the screen
    float centerX = -1.0f + (pos.m_x * cellWidth) + (cellWidth / 2.0f);
    float centerY = 1.0f - ((pos.m_y + 1.0f) * cellHeight) + (cellHeight / 2.0f);

    // 2. Define a generic triangle facing Right
    float w = cellWidth * 0.45f;
    float h = cellHeight * 0.4f;
    // The Chevron Shape Definition
    Vector2D p1(w, 0.0f);         // Front Nose
    Vector2D p2(-w, h);           // Top Tail
    Vector2D p3(-w, -h);          // Bottom Tail
    Vector2D p4(-w * 0.3f, 0.0f); // The "Notch" pushed inward

    // 3. Mathematical visual inversion!
    // Grid Y grows DOWN. Screen Y grows UP. We must invert the angle so it rotates visually correct.
    float visualAngle = -agent.getHeading();

    // 4. Use your custom Matrix module to rotate the triangle points!
    Matrix R(2, 2);
    R.setValue(0, 0, std::cos(visualAngle));
    R.setValue(0, 1, -std::sin(visualAngle));
    R.setValue(1, 0, std::sin(visualAngle));
    R.setValue(1, 1, std::cos(visualAngle));

    p1 = R * p1;
    p2 = R * p2;
    p3 = R * p3;
    p4 = R * p4;

    // 5. Shift the rotated points to the center of the screen coordinates, build TWO triangles to make the chevron with the notch
    float vertices[12] = {
        centerX + p1.m_x, centerY + p1.m_y, // Triangle 1 (Top half)
        centerX + p2.m_x, centerY + p2.m_y,
        centerX + p4.m_x, centerY + p4.m_y, 

        centerX + p1.m_x, centerY + p1.m_y, // Triangle 2 (Bottom half)
        centerX + p4.m_x, centerY + p4.m_y,
        centerX + p3.m_x, centerY + p3.m_y  
    };

    // 6. Send the custom triangle to the GPU
    glUseProgram(m_shaderProgram);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "u_scale"), 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "u_offset"), 0.0f, 0.0f);
    glUniform4f(glGetUniformLocation(m_shaderProgram, "u_color"), 0.1f, 0.9f, 0.1f, 1.0f);

    glBindVertexArray(m_arrowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_arrowVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    
    // Draw 6 vertices (the two connected triangles)
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::renderLidar(const Agent &agent, const Environment &env)
{
    float cellWidth = 2.0f / (float)env.getWidth();
    float cellHeight = 2.0f / (float)env.getHeight();

    Vector2D pos = agent.getPosition();

    // Find the starting point of the beams (The center of the agent)
    float startX = -1.0f + (pos.m_x * cellWidth) + (cellWidth / 2.0f);
    float startY = 1.0f - ((pos.m_y + 1.0f) * cellHeight) + (cellHeight / 2.0f);

    std::vector<Vector2D> hits = agent.getPointCloud();
    if (hits.empty()) return;

// --- 1. Draw the Translucent Fan ---
    std::vector<float> fanVertices;
    fanVertices.push_back(startX); // Center point of the fan
    fanVertices.push_back(startY);

    for (const auto& hit : hits) {
        float endX = -1.0f + (hit.m_x * cellWidth) + (cellWidth / 2.0f);
        float endY = 1.0f - ((hit.m_y + 1.0f) * cellHeight) + (cellHeight / 2.0f);
        fanVertices.push_back(endX);
        fanVertices.push_back(endY);
    }

    glUseProgram(m_shaderProgram);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "u_scale"), 1.0f, 1.0f);
    glUniform2f(glGetUniformLocation(m_shaderProgram, "u_offset"), 0.0f, 0.0f);
    
    // Light Red with 20% Alpha (Transparency)
    glUniform4f(glGetUniformLocation(m_shaderProgram, "u_color"), 1.0f, 0.2f, 0.2f, 0.2f); 

    glBindVertexArray(m_fovVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_fovVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, fanVertices.size() * sizeof(float), fanVertices.data());
    
    // GL_TRIANGLE_FAN connects Center to Point 1, Point 2, Point 3, creating a solid polygon
    glDrawArrays(GL_TRIANGLE_FAN, 0, hits.size() + 1);

    // --- 2. Draw the Solid Laser Lines over the fan ---
    for (size_t i = 0; i < hits.size(); i++) {
        // Darker Red with 100% Alpha (Solid)
        drawLine(startX, startY, fanVertices[(i+1)*2], fanVertices[(i+1)*2+1], 0.8f, 0.0f, 0.0f, 1.0f);
    }
}