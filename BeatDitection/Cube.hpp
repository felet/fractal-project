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
        #define NUMB_VER 36
        GLuint program;
        GLuint vertexArrayObjID;
        static const GLfloat vertices[NUMB_VER*3];
        static const GLfloat normals[NUMB_VER*3];
};

#endif

