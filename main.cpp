#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// window dimention settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    out vec3 FragPos;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        FragPos = aPos;
    }
)glsl";

const char *fragmentShaderSource = R"glsl(
    #version 330 core
    in vec3 FragPos;
    out vec4 FragColor;

    void main() {
    if (FragPos.x < -0.5) { // Check if we are in the left triangle
            
            // Calculate the barycentric coordinates for the left triangle
            float alpha = 1.0 + FragPos.x; // Red intensity increases as we go right to left
            float beta = -FragPos.x - FragPos.y; // Green intensity increases as we go left to right and bottom to top
            float gamma = FragPos.y; // Blue intensity increases as we go bottom to top
            
            // Ensure that none of the intensities go below 0
            alpha = max(alpha, 0.0);
            beta = max(beta, 0.0);
            gamma = max(gamma, 0.0);
            
            // Normalize so that the sum of alpha, beta, and gamma is 1
            float sum = alpha + beta + gamma;
            alpha /= sum;
            beta /= sum;
            gamma /= sum;
            
            // Interpolate between the colors based on the barycentric coordinates
            vec3 color = alpha * vec3(1, 0, 0) + beta * vec3(0, 1, 0) + gamma * vec3(0, 0, 1);
            FragColor = vec4(color, 1.0);
        } else {
            // Right triangle
            vec3 blue = vec3(0.0, 0.0, 1.0);
            vec3 red = vec3(1.0, 0.0, 0.0);
            vec3 green = vec3(0.0, 1.0, 0.0);
            float alpha = 1.0 - abs(FragPos.y); 
            float beta = FragPos.y + FragPos.x;  
            float gamma = 1.0 - beta - alpha;    
            vec3 color = alpha * red + beta * green + gamma * blue;
            FragColor = vec4(color, 1.0);
        }
    }
)glsl";


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Triangles - Module 2 - Eric Florence", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data and configure vertex attributes
    // ------------------------------------------------------------------

    float vertices[] = {
        // First triangle (left triangle, pointing up)
        -1.0f, 0.0f, 0.0f, // left-bottom (moved up)
        -0.5f, 0.0f, 0.0f, // right-bottom (moved up)
        -1.0f, 1.0f, 0.0f, // left-top (moved up)

        // Second triangle (right triangle, already upside down)
        0.0f, 0.0f, 0.0f,  // left-top
        -0.5f, 0.0f, 0.0f, // right-top
        0.0f, -1.0f, 0.0f, // bottom
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer, and then configure vertex attributes.
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // draw the first triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // draw the second triangle
        glDrawArrays(GL_TRIANGLES, 3, 3);

        // glfw: swap buffers
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}