#include "GL_utilities.h"
#include "VectorUtils2.h"

#include "Cube.hpp"
#include "AudioPlayer.hpp"
#include "camera.h"

// Reference to shader program
GLuint program;

/* Light variables START */
GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};

GLint isDirectional[] = {0, 0, 1, 1};

Point3D lightSourcesColorsArr[] =
{
    {1.0f, 0.0f, 0.0f}, // Red light
    {0.0f, 1.0f, 0.0f}, // Green light
    {0.0f, 0.0f, 1.0f}, // Blue light
    {1.0f, 1.0f, 1.0f}  // White light
};

Point3D lightSourcesDirectionsPositions[] =
{
    {10.0f, 5.0f, 0.0f}, // Red light, positional
    {0.0f, 5.0f, 10.0f}, // Green light, positional
    {-1.0f, 0.0f, 0.0f}, // Blue light along X
    {0.0f, 0.0f, -1.0f}  // White light along Z

};
/* Light variables END */

// Create cube
Cube firstCube;

// Create music
AudioPlayer music(1024);

void init(void)
{
    printError("pre init");

    dumpInfo();

    // GL inits
    glClearColor(0.3, 0.3, 0.6, 0);
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

    // Init keyboard functions
    initKeymapManager();

    // Load and play music
    music.loadWave("sound3.wav");
    music.play();

    printError("init");
}

void display(void)
{
    printError("pre display");

    float amplitude;
    GLfloat tranMatrix[16], rotMatrix[16];//, totalMatrix[16];
    
    moveCamera(cameraPosX, cameraPosY, cameraPosZ);

    lookAt( cameraPosX, cameraPosY, cameraPosZ, // Camera pos
            lookAtPosX, lookAtPosY, lookAtPosZ, // Look at pos
            0.0,        1.0,        0.0,        // Up vector
            camMatrix);

    Rx(0, rotMatrix);
    
    //Mult(projectionMatrix, camMatrix, totalMatrix);

    //glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, totalMatrix);
    music.doFFT();
/*****************************************/
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);

    printError("2");
    GLfloat camera_position[] = {(GLfloat)cameraPosX, (GLfloat)cameraPosY, (GLfloat)cameraPosZ};
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, camera_position);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    
    
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotMatrix"), 1, GL_TRUE, rotMatrix);
/*****************************************/

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(int i = 0; (i+1) < music.getNumberFrequencies(); i++)
    {
        T(i, 0, 0, tranMatrix);
        glUniformMatrix4fv(glGetUniformLocation(program, "tranMatrix"), 1, GL_TRUE, tranMatrix);
        //amplitude = log2( music.getFrequencyBandBetween(i, i+1))/20 ;
        amplitude = 4 * music.getFrequencyBandBetween(i, (i+1)) / 10000000;
        //amplitude = 4 * music.getFrequencyBandBetween(i*16, (i+1)*16) / 1000000;
        glUniform3f(glGetUniformLocation(program, "amplitude"), amplitude, amplitude, amplitude);
        firstCube.draw();
        printf("%f\n", amplitude);
    }
    glutSwapBuffers();
    printError("display");
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

    try
    {
        glutMainLoop();
    }
    catch(const char* msg){}

    music.freeWave();

    return EXIT_SUCCESS;
}

