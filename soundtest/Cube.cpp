#include <iostream>
#include "Cube.hpp"

const GLfloat Cube::vertices[9] = { -0.5f, -0.5f, 0.0f,
                                    -0.5f,  0.5f, 0.0f,
                                     0.5f, -0.5f, 0.0f};

Cube::Cube(){}

Cube::~Cube(){}

void Cube::init(GLuint programIn)
{

    program = programIn;

    // two vertex buffer objects, used for uploading
    unsigned int vertexBufferObjID;
    //unsigned int colorBufferObjID;

    // Upload geometry to the GPU:

    // Allocate and activate Vertex Array Object
    glGenVertexArrays(1, &vertexArrayObjID);
    glBindVertexArray(vertexArrayObjID);
    // Allocate Vertex Buffer Objects
    glGenBuffers(1, &vertexBufferObjID);

    // VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    // End of upload of geometry

    printError("init arrays");
}

void Cube::draw()
{
    // Select VAO
    glBindVertexArray(vertexArrayObjID);
    // draw object
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

