#include <iostream>
#include "Cube.hpp"

const GLfloat Cube::vertices[NUMB_VER*3] =
{    // Vertices to FRONT
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     // BACK
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     // TOP
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
                       
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
     // BOTTOM
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
                       
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     // SIDE
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
                  
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     // SIDE
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
                  
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
};

const GLfloat Cube::normals[NUMB_VER*3] =
{    // Normals to FRONT vertices
     0.0 ,  0.0, -0.58,
     0.0 ,  0.0, -0.58,
     0.0 ,  0.0, -0.58,
     0.0 ,  0.0, -0.58,
     0.0 ,  0.0, -0.58,
     0.0 ,  0.0, -0.58,
     // BACK 
     0.0 ,  0.0,  0.58,
     0.0 ,  0.0,  0.58,
     0.0 ,  0.0,  0.58,
     0.0 ,  0.0,  0.58,
     0.0 ,  0.0,  0.58,
     0.0 ,  0.0,  0.58,
     // TOP 
     0.0 ,  0.58,  0.0,
     0.0 ,  0.58,  0.0,
     0.0 ,  0.58,  0.0,
     0.0 ,  0.58,  0.0,
     0.0 ,  0.58,  0.0,
     0.0 ,  0.58,  0.0,
     // BOTTOM 
     0.0 , -0.58,  0.0,
     0.0 , -0.58,  0.0,
     0.0 , -0.58,  0.0,
     0.0 , -0.58,  0.0,
     0.0 , -0.58,  0.0,
     0.0 , -0.58,  0.0,
     // SIDE
     0.58 ,  0.0,  0.0,
     0.58 ,  0.0,  0.0,
     0.58 ,  0.0,  0.0,
     0.58 ,  0.0,  0.0,
     0.58 ,  0.0,  0.0,
     0.58 ,  0.0,  0.0,
     // SIDE
    -0.58 ,  0.0,  0.0,
    -0.58 ,  0.0,  0.0,
    -0.58 ,  0.0,  0.0,
    -0.58 ,  0.0,  0.0,
    -0.58 ,  0.0,  0.0,
    -0.58 ,  0.0,  0.0
};

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
    glBufferData(GL_ARRAY_BUFFER, NUMB_VER*3*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "vert_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "vert_Position"));

    // Allocate Normals Buffer Objects
    GLuint normalsBufferObjID;
    glGenBuffers(1, &normalsBufferObjID);
    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, NUMB_VER*3*sizeof(GLfloat), &normals, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "vert_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "vert_Normal"));

    printError("End Cube::init()");
}

void Cube::draw()
{
    printError("Pre Cube::draw()");

    glBindVertexArray(vertexArrayObjID);    // Select VAO
    //glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, 0L);
    glDrawArrays(GL_TRIANGLES, 0, NUMB_VER);
    printError("End Cube::draw()");
}

