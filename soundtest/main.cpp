#include "assert.h"

#include "GL_utilities.h"
#include "VectorUtils2.h"

#include "Cube.hpp"
#include "AudioPlayer.hpp"


// Reference to shader program
GLuint program;

Cube firstCube;

AudioPlayer music(20);

/* movecamera start*/
#define near 1.0
#define far 90.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5
GLfloat projectionMatrix[] = {  2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                0.0f, 0.0f, -1.0f, 0.0f };

float lookAtPosX = 0;
float lookAtPosY = 9;
float lookAtPosZ = -15;

float cameraPosX = 0;
float cameraPosY = 9;
float cameraPosZ = -16;
float drot = 0;

void moveCamera(float cameraPosX, float cameraPosY, float cameraPosZ){

    if (keyIsDown('w')){
        float tempX = lookAtPosX - cameraPosX;
        float tempY = lookAtPosY - cameraPosY;
        float tempZ = lookAtPosZ - cameraPosZ;

        cameraPosX += tempX;
        cameraPosY += tempY;
        cameraPosZ += tempZ;

        lookAtPosX += tempX;
        lookAtPosY += tempY;
        lookAtPosZ += tempZ;

    }
    else if (keyIsDown('s')){
        float tempX = lookAtPosX - cameraPosX;
        float tempY = lookAtPosY - cameraPosY;
        float tempZ = lookAtPosZ - cameraPosZ;

        cameraPosX -= tempX;
        cameraPosY -= tempY;
        cameraPosZ -= tempZ;

        lookAtPosX -= tempX;
        lookAtPosY -= tempY;
        lookAtPosZ -= tempZ;
    }

    if (keyIsDown('a')){

        drot += 0.1;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);

    }
    else if (keyIsDown('d')){

        drot -= 0.1;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);
    }
    if (keyIsDown('p')){

        lookAtPosY += 0.05;
    }
    else if (keyIsDown('l')){
        lookAtPosY -= 0.05;
    }
}

void lookAt(GLfloat px, GLfloat py, GLfloat pz,
            GLfloat lx, GLfloat ly, GLfloat lz,
            GLfloat vx, GLfloat vy, GLfloat vz,
            GLfloat *m)
{
    Point3D p, l, n, v, u;
    GLfloat r[16], t[16];

    SetVector(px, py, pz, &p);
    SetVector(lx, ly, lz, &l);
    SetVector(vx, vy, vz, &v);

    VectorSub(&l, &p, &n);
    assert(n.x != 0.0 || n.y != 0.0 || n.z != 0.0);

    Normalize(&n);
    n.x = -n.x;
    n.y = -n.y;
    n.z = -n.z;

    CrossProduct(&v, &n, &u);
    assert(u.x != 0.0 || u.y != 0.0 || u.z != 0.0);
    Normalize(&u);

    CrossProduct(&n,&u,&v);

    r[0] = u.x;
    r[1] = u.y;
    r[2] = u.z;
    r[3] = 0;

    r[4] = v.x;
    r[5] = v.y;
    r[6] = v.z;
    r[7] = 0;

    r[8] = n.x;
    r[9] = n.y;
    r[10] = n.z;
    r[11] = 0;

    r[12] = 0;
    r[13] = 0;
    r[14] = 0;
    r[15] = 1;

    // ----------

    t[0] = 1;
    t[1] = 0;
    t[2] = 0;
    t[3] = -p.x;

    t[4] = 0;
    t[5] = 1;
    t[6] = 0;
    t[7] = -p.y;

    t[8] = 0;
    t[9] = 0;
    t[10] = 1;
    t[11] = -p.z;

    t[12] = 0;
    t[13] = 0;
    t[14] = 0;
    t[15] = 1;

    Mult(r,t,m);
}
/* movecamera end */

void init(void)
{
    printError("pre init");

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

    // Init keyboard functions
    initKeymapManager();

    // Load and play music
    music.loadWave("sound.wav");
    music.play();

    printError("init");
}

void display(void)
{
    printError("pre display");
    GLfloat camera[16], totalMatrix[16];

    lookAt( cameraPosX, cameraPosY, cameraPosZ, // Camera pos
            lookAtPosX, lookAtPosY, lookAtPosZ,  // look at pos
            0.0, 1.0, 0.0,  // Up vector
            camera);

    printError("display 1");

    T(0, 0, 0, totalMatrix);
    //TODO SLÅ IHOP DESSA
    Mult(camera, totalMatrix, totalMatrix);
    Mult(projectionMatrix, totalMatrix, totalMatrix);

    printError("display 2");
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, totalMatrix);
    printError("display 3");
    glUniform3f(glGetUniformLocation(program, "color"), 0.3,0.0,0.0);

    printError("display 4");

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    firstCube.draw();

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
    glutMainLoop();
    //music.freeWave();
    return 0;
}

