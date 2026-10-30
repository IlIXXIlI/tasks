#include <glad/glad.h>
#include <cstddef>
#include <iostream>
#include <exception>
#include <memory>
#include <fstream>
#include <sstream>


#include <GLFW/glfw3.h>

const int SZX = 600;
const int SZY = 600;

const char* SHADERVERT = "shaders/simplest.vert";
const char* SHADERFRAG = "shaders/interesting.frag";

struct glfw_error : public std::runtime_error {
    glfw_error(const char *s) : std::runtime_error(s) {}
};


void error_callback(int, const char *s) { throw glfw_error(s); }

void framebuffer_size_callback(GLFWwindow *windows, int width, int height) {
    glViewport(0, 0, width, height);
}

GLFWwindow *initialize_window() {
    GLFWwindow *window;
    glfwSetErrorCallback(error_callback);
    glfwInit();
    window = glfwCreateWindow(SZX, SZY, "Hello world", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) 
        throw glfw_error("Failed to load GLAD");
    return window;
}

GLfloat Vertices[] = {
    -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.4f, 0.0f, 0.0f, 0.2f,
    0.5f, -0.5f, 0.7f, 0.9f, 0.5f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f    
};

std::string readfile(const char *path) {
    std::string code;
    std::ifstream shader_file;
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    shader_file.open(path);
    std::stringstream shader_stream;
    shader_stream << shader_file.rdbuf();
    code  = shader_stream.str();
    return code;
}

GLuint install_shader(std::string shader_code, GLenum shader_type) {
    GLuint shader_id = glCreateShader(shader_type);

    const char *code = shader_code.c_str();
    glShaderSource(shader_id, 1, &code, NULL);
    glCompileShader(shader_id);
        std::cout << code << std::endl;
    GLint success = 0;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) 
        throw glfw_error(std::string("Failed to compile shader: " + std::to_string(shader_id)).c_str());
    return shader_id;
}

GLuint link_shader() {
    GLuint prog_id = glCreateProgram();
    GLuint vertex_id = install_shader(readfile(SHADERVERT), GL_VERTEX_SHADER);
    GLuint fragment_id = install_shader(readfile(SHADERFRAG), GL_FRAGMENT_SHADER);
    glAttachShader(prog_id, vertex_id);
    glAttachShader(prog_id, fragment_id);
    glLinkProgram(prog_id);
    GLint success = 0; 
    glGetProgramiv(prog_id, GL_LINK_STATUS, &success);
    
    if (!success) {
       throw  glfw_error("link error");
    }

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
    return prog_id;
}

void set_float_matrix(GLuint prog_id, const char* name, const float *vals) {
    GLuint loc =  glGetUniformLocation(prog_id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, vals);
}
void set_float(GLuint prog_id, const char* name, const float val) {
    GLuint loc =  glGetUniformLocation(prog_id, name);
    glUniform1f(loc, val);
}



void do_render(GLuint VAO, GLuint prog_id) {
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(prog_id);
    set_float(prog_id, "time", glfwGetTime());

    glBindVertexArray(VAO);
    glDrawArrays(GL_QUADS, 0, 4);
}

int main() try {
    auto Cleanup = [](GLFWwindow*) {glfwTerminate();};
    using UWnd = std::unique_ptr<GLFWwindow, decltype(Cleanup)>;
    UWnd window(initialize_window(), Cleanup);
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE
                        , 6 * sizeof(GLuint)
                        , nullptr);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE
                        , 6 * sizeof(GLuint)
                        , reinterpret_cast<void*> (3 * sizeof(GLuint)));
    glEnableVertexAttribArray(1);
    GLuint prog_id = link_shader();
    while (!glfwWindowShouldClose(window.get())) {
        do_render(VAO, prog_id);
        glfwSwapBuffers(window.get());
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    return 0;
} catch (glfw_error& e) {
    std::cerr << "GLFW error: " << e.what() << std::endl;
} catch(std::exception &e) {
    std::cerr << "Standard error: " << e.what() << std::endl;
} catch(...) {
    std::cerr << "Unknown error" << std::endl;
}


