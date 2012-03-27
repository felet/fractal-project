#ifndef loadobj_h
#define loadobj_h

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif

typedef struct
{
  GLfloat* vertexArray;
  GLfloat* normalArray;
  GLfloat* texCoordArray;
  GLfloat* colorArray; // Rarely used
  GLuint* indexArray;
  int numVertices;
  int numIndices;
  
  // Space for saving VBO and VAO IDs
  GLuint vao; // VAO
  GLuint vb, ib, nb, tb; // VBOs
} Model;

Model* LoadModel(char* name);

// NEW:

void DrawModel(Model *m);
Model* LoadModelPlus(char* name,
			GLuint program,
			char* vertexVariableName,
			char* normalVariableName,
			char* texCoordVariableName);

#endif
