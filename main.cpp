#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#include <Eigen/Core>

#include "shader.hpp"
#include "modelreader.hpp"
using namespace std;
using namespace glm;


// Vertex array object
GLuint myVAO;

// shader filenames
string vsFilename = "shader/geo-pass.vs.glsl";
string fsFilename = "shader/geo-pass.fs.glsl";

string lVsFilename = "shader/light-pass.vs.glsl";
string lFsFilename = "shader/light-pass.fs.glsl";

string onVsFilename = "shader/oren-nayar.vs.glsl";
string onFsFilename = "shader/oren-nayar.fs.glsl";

string spwVsFilename = "shader/single-pass-wireframe.vs.glsl";
string spwGsFilename = "shader/single-pass-wireframe.gs.glsl";
string spwFsFilename = "shader/single-pass-wireframe.fs.glsl";

string ssaoVsFilename = "shader/ssao-shader.vs.glsl";
string ssaoFsFilename = "shader/ssao-shader.fs.glsl";

string ssaoBlurVsFilename = "shader/ssao-shader.vs.glsl";
string ssaoBlurFsFilename = "shader/ssao-blur-shader.fs.glsl";

float sigma = 0.f;

Shader *normShader;
Shader *lightPassShader;
Shader *geoShader;
Shader *phongShader;
Shader *orenNayarShader;
Shader *ssaoShader;
Shader *ssaoBlurShader;

Shader *spwShader;

// Transformation matrices
mat4 rotation = mat4(1.0f);
mat4 scaling = mat4(1.0f);
mat4 translation = mat4(1.0f);


float scalar = 1.0f;

bool toggle_ambient_pass{};
bool toggle_ssao{};


// shader could not be loaded
#ifndef  VERTEX_DATA
#define VERTEX_DATA 0
#endif
#ifndef VERTEX_NORMAL
#define VERTEX_NORMAL 1

#endif
unsigned int quadVBO;

unsigned int quadVAO;


void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 1.f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 1.f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (4 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// Called to draw scene
int width, height;

// This function does any needed initialization on the rendering
// context.
void setupRenderingContext() {
    // Background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);


    // Create the shader program and bind locations for the vertex
    // attributes before linking. The linking process can also generate errors

    normShader = createShader(vsFilename, fsFilename);
    phongShader = createShader(lVsFilename, lFsFilename);
    orenNayarShader = createShader(onVsFilename, onFsFilename);
    spwShader = createShader(spwVsFilename, spwGsFilename, spwFsFilename);

    ssaoShader = createShader(ssaoVsFilename, ssaoFsFilename);
    ssaoBlurShader = createShader(ssaoBlurVsFilename, ssaoBlurFsFilename);
    // Now setup the geometry in a vertex buffer object

    // setup the vertex state array object. All subsequent buffers will
    // be bound to it.
//	glGenVertexArrays(1, &myVAO);
//	glBindVertexArray(myVAO);
}


/**
 * Keyboard callback function.
 */
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
        rotation = rotate(mat4(1.0f), 0.1f, vec3(1.0f, 0.0f, 0.0f)) * rotation;
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
        rotation = rotate(mat4(1.0f), 0.1f, vec3(-1.0f, 0.0f, 0.0f)) * rotation;

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        rotation = rotate(mat4(1.0f), 0.1f, vec3(0.0f, 1.0f, 0.0f)) * rotation;
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        rotation = rotate(mat4(1.0f), 0.1f, vec3(0.0f, -1.0f, 0.0f)) * rotation;
    if (key == GLFW_KEY_9 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (sigma < 20.0f) sigma += 0.05;
        cout << "sigma" << sigma << endl;
    }
    if (key == GLFW_KEY_0 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (sigma > 0.0f) sigma -= 0.05;
        cout << "sigma" << sigma << endl;
    }
    if (key == GLFW_KEY_O && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPassShader = orenNayarShader;
    }
    if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        lightPassShader = phongShader;
    }


    if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        toggle_ambient_pass = !toggle_ambient_pass;
    }
    if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        toggle_ssao = !toggle_ssao;
    }



}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        scalar = 1.01f;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        scalar = 1.0f;

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        scalar = 1.0f / 1.01f;
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        scalar = 1.0f;
}


/**
 * Error callback function
 */
static void error_callback(int error, const char *description) {
    fputs(description, stderr);
}


void ChangeSize(int w, int h) {
    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;




int main(int argc, char **argv) {

    auto mr = readModel("./models/bunny.obj");

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
        if (normShader) {
            glDeleteProgram(normShader->getID());
            glDeleteVertexArrays(1, &myVAO);
        }
        if (phongShader) {
            glDeleteProgram(phongShader->getID());
            glDeleteVertexArrays(1, &myVAO);
        }
        if (orenNayarShader) {
            glDeleteProgram(orenNayarShader->getID());
            glDeleteVertexArrays(1, &myVAO);
        }
        if (spwShader) {
            glDeleteProgram(spwShader->getID());
            glDeleteVertexArrays(1, &myVAO);
        }
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        //Problem: glewInit failed, something is seriously wrong.
        printf("glewInit failed, aborting.\n");
        exit(EXIT_FAILURE);
    }

    // These two lines will print out the version of OpenGL and GLSL
    // that are being used so that problems due to version differences
    // can easily be identified.

    printf("OpenGL version: %s\n", (const char *) glGetString(GL_VERSION));
    printf("GLSL version: %s\n", (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Do any necessary initializations (enabling buffers, setting up
    // shaders, geometry etc., before entering the main loop.)
    // This is done by calling the function setupRenderingContext().
//    auto mr = readModel("./models/dragon.obj");


    setupRenderingContext();
    mr->bufferData();
    scaling = mr->getFitScale();
    translation = mr->getFitTranslate();

    glfwGetFramebufferSize(window, &width, &height);

    // initializing frame buffer
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec, gWireframe;
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // SSAO addition: This ensures we don't accidentally oversample position/depth values in screen-space outside the texture's default coordinate region.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // albedoSpecular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    glGenTextures(1, &gWireframe);
    glBindTexture(GL_TEXTURE_2D, gWireframe);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gWireframe, 0);

    unsigned int attachments[4] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3
    };

    glDrawBuffers(4, attachments);
    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //SSAO

    // also create framebuffer to hold SSAO processing stage
    // -----------------------------------------------------
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    int kernelSize{64};
    float radius{.5f};
    float bias{.025f};
    for (unsigned int i = 0; i < kernelSize; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,randomFloats(generator));
        //glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0,
        //               randomFloats(generator) * 2.0 - 1.0,
        //               randomFloats(generator) * 2.0 - 1.0);
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / (float) kernelSize;

        // scale samples s.t. they're more aligned to center of kernel
        scale = [](float a, float b, float f) -> float { return a + f * (b - a); }(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<glm::vec3> ssaoNoise;
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                        0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //SSAO end


    geoShader = spwShader;

    orenNayarShader->use();
    glUniform1i(glGetUniformLocation(orenNayarShader->getID(), "gPosition"), 0);
    glUniform1i(glGetUniformLocation(orenNayarShader->getID(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(orenNayarShader->getID(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(orenNayarShader->getID(), "gWireframe"), 3);

    phongShader->use();
    glUniform1i(glGetUniformLocation(phongShader->getID(), "gPosition"), 0);
    glUniform1i(glGetUniformLocation(phongShader->getID(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(phongShader->getID(), "gAlbedoSpec"), 2);
    glUniform1i(glGetUniformLocation(phongShader->getID(), "gWireframe"), 3);
    glUniform1i(glGetUniformLocation(phongShader->getID(), "ssao"), 4);

    ssaoShader->use();
    glUniform1i(glGetUniformLocation(ssaoShader->getID(), "gPosition"), 0);
    glUniform1i(glGetUniformLocation(ssaoShader->getID(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(ssaoShader->getID(), "texNoise"), 2);
    glUniform1i(glGetUniformLocation(ssaoShader->getID(), "kernelSize"), kernelSize);
    glUniform1i(glGetUniformLocation(ssaoShader->getID(), "radius"), radius);
    glUniform1i(glGetUniformLocation(ssaoShader->getID(), "bias"), bias);


    ssaoBlurShader->use();
    glUniform1i(glGetUniformLocation(ssaoBlurShader->getID(), "ssaoInput"), 0);

    lightPassShader = phongShader;


    if (geoShader && lightPassShader && ssaoShader && ssaoBlurShader) {
        while (!glfwWindowShouldClose(window)) {
            auto currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 1. geometry pass: render scene's geometry/color data into gbuffer
            // ----------------------------------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            //glClearColor(0.f,1.f,1.f,1.f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //Scale based on input
            scaling = scale(mat4(1.0f), vec3(scalar)) * scaling;
            //Create and pass model view matrix
            mat4 view = lookAt(vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
            mat4 model = rotation * scaling * translation;
            //Create and pass projection matrix
            mat4 proj = perspective(45.0f, (float) width / (float) height, 0.1f, 100.0f);
            geoShader->use();

            glUniformMatrix4fv(glGetUniformLocation(geoShader->getID(), "m_matrix"), 1, GL_FALSE, value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(geoShader->getID(), "v_matrix"), 1, GL_FALSE, value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(geoShader->getID(), "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

            mr->draw(VERTEX_DATA, VERTEX_NORMAL);
            //
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 2. generate SSAO texture
            // ------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            ssaoShader->use();
            // Send kernel + rotation
            for (unsigned int i = 0; i < 64; ++i)
                glUniform3f(glGetUniformLocation(ssaoShader->getID(),
                                                 ("samples[" + std::to_string(i) + "]").c_str()), ssaoKernel[i].x,
                            ssaoKernel[i].y, ssaoKernel[i].z);
            glUniformMatrix4fv(glGetUniformLocation(ssaoShader->getID(), "projection"), 1, GL_FALSE, value_ptr(proj));
            glUniformMatrix4fv(glGetUniformLocation(ssaoShader->getID(), "projection"), 1, GL_FALSE, value_ptr(proj));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2); // add extra SSAO texture to lighting pass
            glBindTexture(GL_TEXTURE_2D, noiseTexture);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // 3. blur SSAO texture to remove noise
            // ------------------------------------
            glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
            glClear(GL_COLOR_BUFFER_BIT);
            ssaoBlurShader->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            //Lighting Pass
            // -----------------------------------------------------------------------------------------------------------------------
            //glClearColor(0.f,0.f,1.f,1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            lightPassShader->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, gWireframe);
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            // send light relevant uniforms
            glUniform3f(glGetUniformLocation(lightPassShader->getID(), "light_pos"), 1.0f, 1.0f, -1.0f);
            glUniform1f(glGetUniformLocation(lightPassShader->getID(), "roughness"), sigma);
            glUniform1f(glGetUniformLocation(lightPassShader->getID(), "toggle_ambient_pass"), toggle_ambient_pass);
            glUniform1i(glGetUniformLocation(lightPassShader->getID(), "toggle_ssao"), toggle_ssao);
            // finally render quad
            renderQuad();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

//
            glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Note that buffer swapping and polling for events is done here
            // so please don't do it in the function used to draw the scene.
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

