#pragma once
#include "OGLRenderer.h"

#define SHADER_VERTEX 0
#define SHADER_FRAGMENT 1
#define	SHADER_GEOMETRY 2

using namespace std;

class Shader {
public:
	Shader(string vertex, string fragment, string geometry = "");
	~Shader(void);
	GLuint GetProgram() { return program; }
	bool LinkProgram();

protected:
	GLuint objects[3], program;
	bool loadFailed;

	void SetDefaultAttributes();
	bool LoadShaderFile(string from, string& into);
	GLuint GenerateShader(string from, GLenum type);
};