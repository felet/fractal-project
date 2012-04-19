#include "GL_utilities.h"
#include "Cube.hpp"

#include "AudioPlayer.hpp"

// Reference to shader program
GLuint program;

Cube firstCube;

AudioPlayer music(20);

void init(void)
{
    dumpInfo();

    // GL inits
    glClearColor(0.2, 0.2, 0.5, 0);
    glEnable(GL_DEPTH_TEST);
    printError("GL inits");

    // Load and compile shader
    program = loadShaders("vertex", "fragment");
    printError("init shader");

    // Load Cube
    firstCube.init(program);
    printError("init cube");

    music.loadWave("sound.wav");
    music.play();
}


void display(void)
{
    printError("pre display");

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    firstCube.draw();

    printError("display");

    glFlush();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutCreateWindow ("");
    glutDisplayFunc(display);
    init();
    glutMainLoop();
}
