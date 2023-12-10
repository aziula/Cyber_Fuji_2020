#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Math.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <vector>
#include <cmath>

void OnResize(GLFWwindow* window, int width, int height);
void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const size_t SZ_FLOAT3 = 3 * sizeof(float);

GLuint CreateShader(GLint type, const char* path)
{
    GLuint shader = 0;
    try
    {
        // Load text file
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path);

        // Interpret the file as a giant string
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        // Verify shader type matches shader file extension
        const char* ext = strrchr(path, '.');
        switch (type)
        {
            case GL_VERTEX_SHADER:
                assert(strcmp(ext, ".vert") == 0);
                break;

            case GL_FRAGMENT_SHADER:
                assert(strcmp(ext, ".frag") == 0);
                break;
        default:
            assert(false, "Invalid shader type");
            break;
        }

        // Compile text as a shader
        std::string str = stream.str();
        const char* src = str.c_str();
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader failed to compile: \n" << infoLog << std::endl;
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Shader (" << path << ") not found: " << e.what() << std::endl;
        // Alternatively, you can replace the above line with "throw;" to crash your program if a shader fails to load
        //throw;
    }
    return shader;
}

GLuint CreateProgram(GLuint vs, GLuint fs)
{
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        shaderProgram = GL_NONE;
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return shaderProgram;
}

struct Vertex
{
    Vector3 pos;
    Vector3 col;
};

using Vertices = std::vector<Vertex>;

// Returns a colour from a rainbow palette based on an index
Vector3 RainbowColorGenerator(int index)
{
    static const std::vector<Vector3> colors = {
        {1.0f, 0.0f, 0.0f}, // Red
        {1.0f, 0.5f, 0.0f}, // Orange
        {1.0f, 1.0f, 0.0f}, // Yellow
        {0.0f, 1.0f, 0.0f}, // Green
        {0.0f, 0.0f, 1.0f}, // Blue
        {0.3f, 0.0f, 0.5f}, // Indigo
        {0.5f, 0.0f, 0.5f}  // Violet
    };
    return colors[index % colors.size()];
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OnResize);
    glfwSetKeyCallback(window, OnKey);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vsDefault = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/Default.vert");
    GLuint fsDefault = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/Default.frag");
    GLuint shaderProgram = CreateProgram(vsDefault, fsDefault);

    Vertices square1(4);
    square1[0].pos = { -1.0f, 1.0f, 0.0f };
    square1[1].pos = {  1.0f, 1.0f, 0.0f };
    square1[2].pos = {  1.0f, -1.0f, 0.0f };
    square1[3].pos = { -1.0f, -1.0f, 0.0f };
    Vertices square2 = square1;
    for (size_t i = 0; i < square1.size(); i++)
    {
        // Using Lerp(), find a point between the start and end points and set it as 'square2's position
        Vector3 squareStartPoint = square1[i].pos;
        Vector3 squareEndPoint = square1[(i + 1) % 4].pos;
        square2[i].pos = Lerp(squareStartPoint, squareEndPoint, 0.5f);

        // Assigns colors to the vertices of both squares
        square1[i].col = RainbowColorGenerator(0);
        square2[i].col = RainbowColorGenerator(1);
    }

    const size_t squareCount = 100;   // Number of squares to draw (changeable)
    const size_t vertexCount = 4;   // Number of vertices in a square (DO NOT CHANGE)!
    Vertices vertices(squareCount * vertexCount);

    // TODO -- generate vertices (replace the memcpy's with a variable version of the above)
    // memcpy(vertices.data(), square1.data(), sizeof Vertex * 4);
    // memcpy(vertices.data() + 4, square2.data(), sizeof Vertex * 4);
    memcpy(vertices.data(), square1.data(), sizeof(Vertex) * 4);

    // Potential vertex-generation loop:
    //for (size_t i = 0; i < squareCount * vertexCount; i += vertexCount)
    //{
    //    for (size_t j = 0; j < vertexCount; j++)
    //    {
    //        vertices[i + j] = previous_square_midpoints[j]
    //        vertices[i + j] = random_colour
    //    }
    //}

    // Vertex-generation loop
    for (size_t i = 1; i < squareCount; i++)
    {
        for (size_t j = 0; j < vertexCount; j++)
        {
            // Get the starting and ending points of the square's edge
            Vector3 squareStartPoint = vertices[(i - 1) * vertexCount + j].pos;
            Vector3 squareEndPoint = vertices[(i - 1) * vertexCount + (j + 1) % vertexCount].pos;

            // Finds an interpolated point along the edge, and its position changes 
            float interpolationValue = 0.1f * (1.0f + sinf((float)(i + j) * 0.1f));
            Vector3 interpolatedPoint = Lerp(squareStartPoint, squareEndPoint, interpolationValue);

            // Assign the calculated position and color to the current vertex
            vertices[i * vertexCount + j].pos = interpolatedPoint;
            vertices[i * vertexCount + j].col = RainbowColorGenerator(static_cast<int>(i));
        }
    }


    GLuint vaoLines, vboLines;
    glGenVertexArrays(1, &vaoLines);
    glGenBuffers(1, &vboLines);
    glBindVertexArray(vaoLines);
    glBindBuffer(GL_ARRAY_BUFFER, vboLines);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * SZ_FLOAT3, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * SZ_FLOAT3, (void*)(SZ_FLOAT3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glUseProgram(shaderProgram);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(vaoLines);
        for (size_t i = 0; i < vertices.size(); i += 4)
            glDrawArrays(GL_LINE_LOOP, i, 4);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void OnStateSwitch(bool& state, double& t)
{
    state = !state;
    t = 0.0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void OnResize(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
