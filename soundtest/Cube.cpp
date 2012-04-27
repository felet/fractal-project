#include <iostream>
#include "Cube.hpp"

Model *Cube::model;

Cube::Cube(){}

Cube::~Cube(){}

void Cube::init(GLuint programIn)
{
    printError("Pre Cube::init()");

    program = programIn;

    model = LoadModelPlus(  (char*)"cubeplus.obj",
                            program,
                            (char*)"vert_Position",
                            (char*)"vert_Normal",
                            (char*)"vert_TexCoord");

    printError("End Cube::init()");
}

void Cube::draw()
{
    printError("Pre Cube::draw()");

    DrawModel(model);

    printError("End Cube::draw()");
}

