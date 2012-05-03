#include <iostream>
#include "Cube.hpp"

const GLfloat Cube::vertices[8][3] = {
    {-0.5,-0.5,-0.5}, 
    {0.5,-0.5,-0.5}, 
    {0.5,0.5,-0.5}, 
    {-0.5,0.5,-0.5}, 
    {-0.5,-0.5,0.5}, 
    {0.5,-0.5,0.5}, 
    {0.5,0.5,0.5}, 
{-0.5,0.5,0.5}};

const GLfloat Cube::normals[8][3] = {
    {-0.58,-0.58,-0.58}, 
    {0.58,-0.58,-0.58}, 
    {0.58,0.58,-0.58}, 
    {-0.58,0.58,-0.58}, 
    {-0.58,-0.58,0.58}, 
    {0.58,-0.58,0.58}, 
    {0.58,0.58,0.58}, 
{-0.58,0.58,0.58}};

const GLubyte Cube::cubeIndices[36] = 
                          {0,3,2, 0,2,1,
                           2,3,7, 2,7,6,
                           0,4,7, 0,7,3,
                           1,2,6, 1,6,5,
                           4,5,6, 4,6,7,
                           0,1,5, 0,5,4};

Cube::Cube(){}

Cube::~Cube(){}

void Cube::init(GLuint programIn)
{
    printError("Pre Cube::init()");

    program = programIn;

    // Upload geometry to the GPU:
    // Allocate and activate Vertex Array Object
    glGenVertexArrays(1, &vertexArrayObjID);
    glBindVertexArray(vertexArrayObjID);

    // Allocate Vertex Buffer Objects
    GLuint vertexBufferObjID;
    glGenBuffers(1, &vertexBufferObjID);

    // VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, NUMB_VER*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    // Allocate Normals, index Buffer Objects
    glGenBuffers(1, &normalsBufferObjID);
	glGenBuffers(1, &indexBufferID);

    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, NUMB_VER*sizeof(GLfloat), &normals, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    // VBO index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUMB_IND*sizeof(GLubyte), cubeIndices, GL_STATIC_DRAW);

    printError("End Cube::init()");
}

void Cube::draw()
{
    printError("Pre Cube::draw()");
    glBindVertexArray(vertexArrayObjID);    // Select VAO
    glDrawElements(GL_TRIANGLES, NUMB_IND, GL_UNSIGNED_BYTE, 0L);
    printError("End Cube::draw()");
}

