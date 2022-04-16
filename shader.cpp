//
// Created by Arya on 3/17/22.
//

#include "Shader.hpp"
#include <fstream>
#include <iostream>

bool Shader::loadShaderFile(const char *filename, GLuint shader) {
	GLint shaderLength = 0;
	FILE *fp;

	// Open the shader file
	fp = fopen(filename, "r");
	if(fp != NULL) {
		// See how long the file is
		while (fgetc(fp) != EOF)
			shaderLength++;

		// Go back to beginning of file
		rewind(fp);

		// Read the whole file in
		fread(shaderText, 1, shaderLength, fp);

		// Make sure it is null terminated and close the file
		shaderText[shaderLength] = '\0';
		fclose(fp);
	}
	else {
		return false;
	}

	// Load the string into the shader object
	GLchar* fsStringPtr[1];
	fsStringPtr[0] = (GLchar *)((const char*)shaderText);
	glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL );

	return true;
}

void Shader::setupShader(std::string vsFilename, std::string fsFilename){
	// First setup the shaders
	//Now, let's setup the shaders
	GLuint shaderProgram = (GLuint)NULL;
	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLint testVal;

	if( !loadShaderFile(vsFilename.c_str(), hVertexShader) ) {
		glDeleteShader( hVertexShader );
		glDeleteShader( hFragmentShader );
		std::cout << "The shader " << vsFilename << " could not be found." << std::endl;
	}

	if( !loadShaderFile(fsFilename.c_str(), hFragmentShader) ) {
		glDeleteShader( hVertexShader );
		glDeleteShader( hFragmentShader );
		std::cout << "The shader " << fsFilename << " could not be found." << std::endl;
	}

	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if( testVal == GL_FALSE ) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource( hVertexShader, 8192, NULL, source );
		glGetShaderInfoLog( hVertexShader, 8192, NULL, infoLog);
		std::cout << "The shader: " << std::endl << (const char*)source << std::endl << " failed to compile:" << std::endl;
		fprintf( stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if( testVal == GL_FALSE ) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource( hFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog( hFragmentShader, 8192, NULL, infoLog);
		std::cout << "The shader: " << std::endl << (const char*)source << std::endl << " failed to compile:" << std::endl;
		fprintf( stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, hVertexShader);
	glAttachShader(shaderProgram, hFragmentShader);

	glBindAttribLocation( shaderProgram, VERTEX_DATA, "position" );
	//glBindAttribLocation( shaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation( shaderProgram, VERTEX_NORMAL, "normal" );

	glLinkProgram( shaderProgram );
	glDeleteShader( hVertexShader );
	glDeleteShader( hFragmentShader );
	glGetProgramiv( shaderProgram, GL_LINK_STATUS, &testVal );
	if( testVal == GL_FALSE ) {
		char infoLog[1024];
		glGetProgramInfoLog( shaderProgram, 1024, NULL, infoLog);
		std::cout << "The shader program (" << vsFilename << " + " << fsFilename << ") failed to link:" << std::endl << (const char*)infoLog << std::endl;
		glDeleteProgram(shaderProgram);
		shaderProgram = (GLuint)NULL;
	}

	ID = shaderProgram;

}

Shader::Shader(std::string vsFilename, std::string fsFilename) {
	setupShader(vsFilename,fsFilename);
}

void Shader::use() {
	glUseProgram( ID );
}

GLuint Shader::getID(){
	return ID;
}

Shader::Shader(std::string vsFilename, std::string gsFilename, std::string fsFilename) {
	setupShader(vsFilename,gsFilename,fsFilename);
}

void Shader::setupShader(std::string vsFilename, std::string gsFilename, std::string fsFilename) {
	// First setup the shaders
	//Now, let's setup the shaders
	GLuint shaderProgram = (GLuint)NULL;
	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hGeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GLint testVal;

	if( !loadShaderFile(vsFilename.c_str(), hVertexShader) ) {
		glDeleteShader( hVertexShader );
		glDeleteShader( hGeometryShader );
		glDeleteShader( hFragmentShader );
		std::cout << "The shader " << vsFilename << " could not be found." << std::endl;
	}

	if( !loadShaderFile(gsFilename.c_str(), hGeometryShader) ) {
		glDeleteShader( hVertexShader );
		glDeleteShader( hGeometryShader );
		glDeleteShader( hFragmentShader );
		std::cout << "The shader " << gsFilename << " could not be found." << std::endl;
	}

	if( !loadShaderFile(fsFilename.c_str(), hFragmentShader) ) {
		glDeleteShader( hVertexShader );
		glDeleteShader( hGeometryShader );
		glDeleteShader( hFragmentShader );
		std::cout << "The shader " << fsFilename << " could not be found." << std::endl;
	}

	glCompileShader(hVertexShader);
	glCompileShader(hGeometryShader);
	glCompileShader(hFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if( testVal == GL_FALSE ) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource( hVertexShader, 8192, NULL, source );
		glGetShaderInfoLog( hVertexShader, 8192, NULL, infoLog);
		std::cout << "The shader: " << std::endl << (const char*)source << std::endl << " failed to compile:" << std::endl;
		fprintf( stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader( hGeometryShader );
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hGeometryShader, GL_COMPILE_STATUS, &testVal);
	if( testVal == GL_FALSE ) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource( hGeometryShader, 8192, NULL, source );
		glGetShaderInfoLog( hGeometryShader, 8192, NULL, infoLog);
		std::cout << "The shader: " << std::endl << (const char*)source << std::endl << " failed to compile:" << std::endl;
		fprintf( stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader( hGeometryShader );
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if( testVal == GL_FALSE ) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource( hFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog( hFragmentShader, 8192, NULL, infoLog);
		std::cout << "The shader: " << std::endl << (const char*)source << std::endl << " failed to compile:" << std::endl;
		fprintf( stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader( hGeometryShader );
		glDeleteShader(hFragmentShader);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, hVertexShader);
	glAttachShader(shaderProgram, hGeometryShader);
	glAttachShader(shaderProgram, hFragmentShader);

	glBindAttribLocation( shaderProgram, VERTEX_DATA, "position" );
	//glBindAttribLocation( shaderProgram, VERTEX_COLOUR, "vColor" );
	glBindAttribLocation( shaderProgram, VERTEX_NORMAL, "normal" );

	glLinkProgram( shaderProgram );
	glDeleteShader( hVertexShader );
	glDeleteShader( hGeometryShader );
	glDeleteShader( hFragmentShader );
	glGetProgramiv( shaderProgram, GL_LINK_STATUS, &testVal );
	if( testVal == GL_FALSE ) {
		char infoLog[1024];
		glGetProgramInfoLog( shaderProgram, 1024, NULL, infoLog);
		std::cout << "The shader program (" << vsFilename << " + " << gsFilename << " + " << fsFilename << ") failed to link:" << std::endl << (const char*)infoLog << std::endl;
		glDeleteProgram(shaderProgram);
		shaderProgram = (GLuint)NULL;
	}

	ID = shaderProgram;
}


Shader* createShader(std::string vert, std::string frag){
		Shader* shader = new Shader(vert, frag);
		return shader;
}

Shader *createShader(std::string vert, std::string geo, std::string frag) {
	Shader* shader = new Shader(vert, geo, frag);
	return shader;
}
