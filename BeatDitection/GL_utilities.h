#ifndef _GL_UTILITIES_
#define _GL_UTILITIES_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
//	#include <GLUT/glut.h>
	#include "MicroGlut.h"
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif

void printError(const char *functionName);
GLuint loadShaders(const char *vertFileName, const char *fragFileName);
void dumpInfo(void);

void initKeymapManager();
char keyIsDown(unsigned char c);

#ifdef __cplusplus
}
#endif

#endif
