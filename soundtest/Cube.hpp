#ifndef CUBE_HPP
#define CUBE_HPP

#include "GL_utilities.h"
#include "loadobj.h"

class Cube
{
    public:
        Cube();
        ~Cube();
        void init(GLuint);
        void draw();

    private:
        GLuint program;
        static Model *model;
};

#endif

