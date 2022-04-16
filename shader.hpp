//
// Created by Arya on 3/17/22.
//

#ifndef HW4_SKELETON_SHADER_H
#define HW4_SKELETON_SHADER_H

#include <GL/glew.h>
#include <string>


// Constants to help with location bindings
#ifndef  VERTEX_DATA
#define VERTEX_DATA 0
#endif
#ifndef VERTEX_NORMAL
#define VERTEX_NORMAL 1
#endif

class Shader {
public:
	Shader(std::string vsFilename, std::string fsFilename);
	Shader(std::string vsFilename, std::string gsFilename, std::string fsFilename);
	void use();
	GLuint getID();
private:
	GLuint ID;
	GLubyte shaderText[8192];
	bool loadShaderFile(const char *filename, GLuint shader);
	void setupShader(std::string vsFilename, std::string fsFilename);
	void setupShader(std::string vsFilename, std::string gsFilename, std::string fsFilename);
};

Shader* createShader(std::string vert, std::string frag);
Shader* createShader(std::string vert, std::string geo, std::string frag);


#endif //HW4_SKELETON_SHADER_H
