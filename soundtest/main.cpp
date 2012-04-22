#include "GL_utilities.h"
#include "Cube.hpp"

#include "AudioPlayer.hpp"

// Reference to shader program
GLuint program;

Cube firstCube;

//AudioPlayer music(20);

void init(void)
{
    dumpInfo();

    // GL inits
    glClearColor(0.2, 0.2, 0.5, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printError("GL inits");

    // Load and compile shader
    program = loadShaders("vertex", "fragment");
    printError("init shader");

    // Load Cube
    firstCube.init(program);
    printError("init cube");

    //music.loadWave("sound.wav");
    //music.play();
}


void display(void)
{
    printError("pre display");

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    firstCube.draw();

    printError("display");

    glutSwapBuffers();
}

void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow ("Sound test");
    glutDisplayFunc(display);
    init();
	glutTimerFunc(20, &OnTimer, 0);
    glutMainLoop();
    //music.freeWave();
    return 0;
}
