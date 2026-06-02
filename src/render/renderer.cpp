#include "../../include/render/renderer.hpp"
#include <iostream>

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

    // Set the rendering area
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup GPU Memory and Shaders
    initShaders();
    initQuad();
}

Renderer::~Renderer()
{
    glfwTerminate();
}

bool Renderer::isRunning() const
{
    return !glfwWindowShouldClose(m_window);
}

void Renderer::clearScreen() const
{
    // A dark charcoal background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "uniform vec2 u_offset;\n"
        "uniform vec2 u_scale;\n"
        "void main() {\n"
        "   gl_Position = vec4((aPos * u_scale) + u_offset, 0.0, 1.0);\n"
        "}\0";

    // 2. Fragment Shader: Handles Colors
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "uniform vec3 u_color;\n"
        "void main() {\n"
        "   FragColor = vec4(u_color, 1.0f);\n"
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

void Renderer::initQuad()
{
    // A generic 1x1 square starting at origin (0,0)
    float vertices[] = {
        0.0f,  0.0f, // Bottom Left
        1.0f,  0.0f, // Bottom Right
        1.0f,  1.0f, // Top Right
        0.0f,  0.0f, // Bottom Left
        1.0f,  1.0f, // Top Right
        0.0f,  1.0f  // Top Left
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

// Shaders: Modern GPUs refuse to draw anything unless you give them a program (written in GLSL) telling them exactly how to calculate the geometry and color. We hardcode them here as strings so you don't have to deal with file I/O loading just yet.

// The u_offset and u_scale: Instead of uploading 100 different squares for your grid, we upload ONE generic 1x1 square to the m_VBO. We then use these "uniforms" to tell the GPU to scale and slide that single square across the screen like a rubber stamp.


// The Math and Drawing Logic
// This maps your discrete int grid to the monitor.

void Renderer::drawQuad(float x, float y, float width, float height, float r, float g, float b) {
    glUseProgram(m_shaderProgram);

    // Pass the color to the fragment shader
    int colorLoc = glGetUniformLocation(m_shaderProgram, "u_color");
    glUniform3f(colorLoc, r, g, b);

    // Pass the scale and position to the vertex shader
    int scaleLoc = glGetUniformLocation(m_shaderProgram, "u_scale");
    glUniform2f(scaleLoc, width, height);

    int offsetLoc = glGetUniformLocation(m_shaderProgram, "u_offset");
    glUniform2f(offsetLoc, x, y);

    // Draw the stamped square
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::renderEnvironment(const Environment& env) {
    // 1. Ask the environment for its dynamic dimensions and cast them to floats for decimal math.
    float gridWidth = (float)env.getWidth();
    float gridHeight = (float)env.getHeight();

    // 2. OpenGL's screen goes from -1.0 to 1.0 (a total width/height of 2.0). 
    // We divide 2.0 by our grid dimensions to find out exactly how much screen space ONE cell takes up.
    float cellWidth = 2.0f / gridWidth;
    float cellHeight = 2.0f / gridHeight;

    // 3. Loop through every single row (Y axis) from 0 up to the dynamic height.
    for (int y = 0; y < env.getHeight(); y++) {
        
        // 4. Loop through every single column (X axis) in the current row from 0 up to the dynamic width.
        for (int x = 0; x < env.getWidth(); x++) {
            
            // 5. Map the discrete X grid coordinate (0, 1, 2...) to OpenGL's -1.0 to 1.0 screen coordinate.
            float screenX = -1.0f + (x * cellWidth);
            
            // 6. Map the Y coordinate, but invert it! OpenGL's Y grows UPwards, but standard arrays/grids grow DOWNwards.
            float screenY = 1.0f - ((y + 1) * cellHeight); 

            // 7. Ask the environment object if this specific (x, y) coordinate is a wall.
            if (env.isObstacle(Vector2D(x, y))) {
                
                // 8. It IS a wall. Draw a quad here using a Light Gray color (R:0.7, G:0.7, B:0.7).
                // The 0.95f multiplier shrinks the square by 5% to create a visual gap (grid lines) between cells.
                drawQuad(screenX, screenY, cellWidth * 0.95f, cellHeight * 0.95f, 0.7f, 0.7f, 0.7f);
            
            } else {
                
                // 9. It is NOT a wall (Empty Space). Draw a quad here using a Dark Gray color (R:0.2, G:0.2, B:0.2).
                drawQuad(screenX, screenY, cellWidth * 0.95f, cellHeight * 0.95f, 0.2f, 0.2f, 0.2f);
            }
        }
    }
}

void Renderer::renderAgent(const Agent& agent, const Environment& env) {
    // 1. Ask the environment for its dynamic dimensions so we know how to scale the agent.
    float gridWidth = (float)env.getWidth();
    float gridHeight = (float)env.getHeight();

    // 2. Calculate the exact screen size of a single cell using the 2.0 total OpenGL range.
    float cellWidth = 2.0f / gridWidth;
    float cellHeight = 2.0f / gridHeight;

    // 3. Retrieve the agent's current exact coordinate location on the grid.
    Vector2D pos = agent.getPosition();
    
    // 4. Convert the agent's X coordinate to OpenGL's screen space.
    float screenX = -1.0f + (pos.m_x * cellWidth);
    
    // 5. Convert and invert the agent's Y coordinate to OpenGL's screen space.
    float screenY = 1.0f - ((pos.m_y + 1) * cellHeight);

    // 6. Draw the Agent quad using a Bright Green color (R: 0.2, G: 0.8, B: 0.2).
    // It is shrunk by 0.95f so it fits perfectly inside the cell boundaries we established in renderEnvironment.
    drawQuad(screenX, screenY, cellWidth * 0.95f, cellHeight * 0.95f, 0.2f, 0.8f, 0.2f);
}

void Renderer::renderLidar(const Agent& agent, const Environment& env) {
    float cellWidth = 2.0f / (float)env.getWidth();
    float cellHeight = 2.0f / (float)env.getHeight();

    // Get the point cloud from the agent
    std::vector<Vector2D> hits = agent.getPointCloud();

    for (const auto& hit : hits) {
        float screenX = -1.0f + (hit.m_x * cellWidth);
        float screenY = 1.0f - ((hit.m_y + 1) * cellHeight);

        // Draw small red squares (0.2f multiplier makes them tiny dots)
        drawQuad(screenX, screenY, cellWidth * 0.2f, cellHeight * 0.2f, 1.0f, 0.0f, 0.0f);
    }
}