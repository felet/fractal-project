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
        GLuint program;
        unsigned int vertexArrayObjID;
        static const GLfloat vertices[9];
};

#endif

