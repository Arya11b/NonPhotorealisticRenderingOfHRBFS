#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

static void error_callback(int error, const char *description) {
    fputs(description, stderr);
}

int main(int argc, char *argv[])
{
    GLFWwindow *window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Anti-aliasing - This will have smooth polygon edges
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(800, 800, "OBJ Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // TODO set key callbacks
    // TODO set Mouse callbacks
//    glfwSetKeyCallback(window, key_callback);
//    glfwSetMouseButtonCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        //Problem: glewInit failed, something is seriously wrong.
        printf("glewInit failed, aborting.\n");
        exit(EXIT_FAILURE);
    }
    printf("OpenGL version: %s\n", (const char *) glGetString(GL_VERSION));
    printf("GLSL version: %s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    while(!glfwWindowShouldClose(window)){

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

}