#ifndef CUBE_HPP
#define CUBE_HPP

#include "GL_utilities.h"

class Cube
{
    public:
        Cube();
        ~Cube();
        void init(GLuint);
        void draw();

    private:
        #define NUMB_VER 24
        #define NUMB_IND 36 
        GLuint program;
        GLuint vertexArrayObjID;
        GLuint normalsBufferObjID;
        GLuint vertexBufferObjID;
        GLuint indexBufferID;
        static const GLfloat vertices[8][3];
        static const GLfloat normals[8][3];
        static const GLubyte cubeIndices[NUMB_IND]; 
};

#endif

