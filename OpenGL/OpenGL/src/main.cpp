#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if(line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return{ ss[0].str(),ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string source)
{
    unsigned int id = glCreateShader(type); //create an empty shader and returns a non zero value
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);   //sets the source code in shader
    glCompileShader(id);    //compiles the source code in shader

    //Error Handling in source code
    int result;
    //glGetShaderiv returns a specific parameter of a shader
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);  //GL_COMPILE_STATUS returns the compile results of the source code  
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); //GL_INFO_LOG_LENGTH returns the number of characters in the information log  
        char* message = (char*)alloca(length * sizeof(char));   //allocates memory (length * sizeof(char)) in stack for variable message.   
        glGetShaderInfoLog(id, length, &length, message);   //Gets the info log of the shader

        //Printing the error message
        std::cout << "Failed to compile "<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
        std::cout << message << std::endl;

        //Deleting the shader
        glDeleteShader(id);
        return 0;
    }
    else
    {        
        std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " Shader Compiled" << std::endl;        
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();   //creates empty program object and returns non zero value by which it can be referenced
    //Compiling shaders
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);    //attaches vertex shader to program
    glAttachShader(program, fs);    //attaches fragment shader to program

    std::cout << "Shaders attached"<<std::endl;

    glLinkProgram(program);     //Links the shaders in program, and makes a complete GPU executable
    glValidateProgram(program);     //checks the executable could be run with current OpenGL state

    std::cout << "Program Linked and Validated"<<std::endl;

    //Deleting the created shaders as they are already in program
    glDeleteShader(vs);
    glDeleteShader(fs);

    std::cout << "Shaders deleted"<<std::endl;

    return program;

}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
  

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //Initializing Glew
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error: GLEW not OK" << std::endl;
    }
    else
    {
        std::cout << "GOOD: GLEW OK" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }


    //Make a square now
    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };


    //Drawing Triangle using modern OpenGL
    unsigned int buffer;    
    glGenBuffers(1, &buffer);   //Creating buffer object
    glBindBuffer(GL_ARRAY_BUFFER, buffer);  //Binding the buffer obj to a target, target is GL_ARRAY_BUFFER    
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);    //Adding data to the buffer
    
    glEnableVertexAttribArray(0);   //Activates the Vertex attribute array in the specific index
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);  //Specifying the format of the data - Vertex Attribute

    glBindBuffer(GL_ARRAY_BUFFER, 0);   //unbinding the buffer so it's not modified in future

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    /*std::cout << "VERTEX SOURCE" << std::endl;
    std::cout << source.VertexSource << std::endl;

    std::cout << "FRAGMENT SOURCE" << std::endl;
    std::cout << source.FragmentSource << std::endl;*/

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    std::cout << "Program Running..." << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        
        //Drawing Traingle using Legacy OpenGL
        /*glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);
        glEnd();*/


        //Drawing using modern OpenGL
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}