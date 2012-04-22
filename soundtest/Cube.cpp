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
                            (char*)"in_Position",
                            (char*)"in_Normal",
                            (char*)"in_TexCoord");

    printError("End Cube::init()");
}

void Cube::draw()
{
    DrawModel(model);
}

