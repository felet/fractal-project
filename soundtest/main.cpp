#include <unistd.h>
#include <assert.h>
#include <iostream>

#include "GL_utilities.h"
#include "VectorUtils2.h"

#include "Cube.hpp"
#include "AudioPlayer.hpp"

#define PI 3.141592653589793238462

// Reference to shader program
GLuint program;

GLfloat tranMatrix[16], sizeMatrix[16], rotMatrix[16], modelMatrix[16];

/* Light variables START */
GLfloat specularExponent[] = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0};
GLint isDirectional[] = {0, 0, 0, 0, 0, 0};

Point3D lightSourcesColorsArr[] =
{
    {1.0f, 1.0f, 1.0f},  // White light
    {1.0f, 1.0f, 1.0f},  // White light
    {1.0f, 1.0f, 1.0f},  // White light
    {1.0f, 1.0f, 1.0f},  // White light
    {1.0f, 1.0f, 1.0f},  // White light
    {1.0f, 1.0f, 1.0f}  // White light
};

Point3D lightSourcesDirectionsPositions[] =
{
    {0.0f, 0.0f, 1.0f},  // White light
    {0.0f, 1.0f, 0.0f},  // White light
    {1.0f, 0.0f, 0.0f},  // White light
    {0.0f, 0.0f,-1.0f},  // White light
    {0.0f,-1.0f, 0.0f},  // White light
    {-1.0f, 0.0f, 0.0f}  // White light

};
/* Light variables END */


// Create cube
Cube firstCube;

// music
AudioPlayer *music;

/* Keyboard actions START */
struct mode_type
{
    int phong, cubeDim, transformation, song, amplitude, cubeColor, beat;
}mode;

char keymap[256];

char keyIsDown(unsigned char c)
{
    return keymap[(unsigned int)c];
}

void keyUp(unsigned char key, int x, int y)
{
	keymap[(unsigned int)key] = 0;
}

void keyDown(unsigned char key, int x, int y)
{
	keymap[(unsigned int)key] = 1;

    if (key == '1')
    {   // Change transformation mode
        mode.transformation = (mode.transformation+1) % 3;
        glUniform1i(glGetUniformLocation(program, "modeTransformation"), mode.transformation);
    }
    else if (key == '2')
        // Change amplitude mode
        mode.amplitude = (mode.amplitude+1) % 3;
    else if (key == '3')
        // Change beat mode
        mode.beat = (mode.beat+1) % 3;
    else if (key == '4')
    {   // Change cube color mode
        mode.cubeColor = (mode.cubeColor+1) % 2;
        glUniform1i(glGetUniformLocation(program, "modeCubeColor"), mode.cubeColor);
    }
    else if (key == '5')
    {   // Change song
        mode.song = (mode.song+1) % 4;
        delete music;

        const char *number[] =  {"0", "1", "2", "3"};
        
        // Create music
        std::string fileName = std::string("sound") + std::string(number[mode.song]) + std::string(".wav");
        std::cout << fileName << std::endl;
        music = new AudioPlayer(fileName.c_str(), 1024*2);

        // Play music
        music->play();
    }
    else if (key == '6')
    {
        // Change Phong 
        mode.phong = (mode.phong+1) % 2;
        glUniform1i(glGetUniformLocation(program, "modePhong"), mode.phong);
    }
    else if (key == '+')
    {   // Increase dimensions on cube mode
        mode.cubeDim++;
        glUniform1i(glGetUniformLocation(program, "modeCubeDim"), mode.cubeDim);
    }
    else if (key == '-' && mode.cubeDim > 1)
    {   // Decrease dimensions on cube mode
        mode.cubeDim--;
        glUniform1i(glGetUniformLocation(program, "modeCubeDim"), mode.cubeDim);
    }
    else if (key == 27) // Exit main loop
       throw "END_MAIN_LOOP";
}

void initKeymapManager()
{
	for (int i = 0; i < 256; i++)
        keymap[i] = 0;

	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
    std::cout << std::endl
        << "1.\tChange transformation mode" << std::endl
        << "2.\tChange amplitude mode" << std::endl
        << "3.\tChange beat mode" << std::endl
        << "4.\tChange cube color mode" << std::endl
        << "5.\tChange song " << std::endl
        << "5.\tChange Phong" << std::endl
        << "+.\tDecrease dimensions on cube mode" << std::endl
        << "-.\tIncrease dimensions on cube mode" << std::endl
        << "Esc.\tQuit" << std::endl;
}

/* Keyboard actions END */

/* Camera START */
#define near 1.0
#define far 900.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

GLfloat camMatrix[16];

GLfloat projectionMatrix[] =
{
    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
    0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
    0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
    0.0f, 0.0f, -1.0f, 0.0f
};

float lookAtPosX = 0;
float lookAtPosY = 0;
float lookAtPosZ = -4;

float cameraPosX = 0;
float cameraPosY = 0;
float cameraPosZ = -5;
float drot = 0;

void moveCamera(float &cameraPosX, float &cameraPosY, float &cameraPosZ){

    #define SCALE 1.0
    #define ROTATION 0.02
    float tempX = (lookAtPosX - cameraPosX) * SCALE;
    float tempY = (lookAtPosY - cameraPosY) * SCALE;
    float tempZ = (lookAtPosZ - cameraPosZ) * SCALE;

    if (keyIsDown('w'))
    {
        cameraPosX += tempX;
        cameraPosY += tempY;
        cameraPosZ += tempZ;

        lookAtPosX += tempX;
        lookAtPosY += tempY;
        lookAtPosZ += tempZ;
    }
    else if (keyIsDown('s'))
    {
        cameraPosX -= tempX;
        cameraPosY -= tempY;
        cameraPosZ -= tempZ;

        lookAtPosX -= tempX;
        lookAtPosY -= tempY;
        lookAtPosZ -= tempZ;
    }

    if (keyIsDown('a'))
    {
        drot += ROTATION;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);

    }
    else if (keyIsDown('d'))
    {
        drot -= ROTATION;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);
    }

    if (keyIsDown('p'))
        lookAtPosY += ROTATION;
    else if (keyIsDown('l'))
        lookAtPosY -= ROTATION;
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

    r[0] = u.x;     r[1] = u.y;     r[2] = u.z;     r[3] = 0;
    r[4] = v.x;     r[5] = v.y;     r[6] = v.z;     r[7] = 0;
    r[8] = n.x;     r[9] = n.y;     r[10] = n.z;    r[11] = 0;
    r[12] = 0;      r[13] = 0;      r[14] = 0;      r[15] = 1;

    t[0] = 1;       t[1] = 0;       t[2] = 0;       t[3] = -p.x;
    t[4] = 0;       t[5] = 1;       t[6] = 0;       t[7] = -p.y;
    t[8] = 0;       t[9] = 0;       t[10] = 1;      t[11] = -p.z;
    t[12] = 0;      t[13] = 0;      t[14] = 0;      t[15] = 1;

    Mult(r, t, m);
}
/* Camera END */

void init(void)
{
    printError("pre init");

    // Set mode values
    mode.transformation = mode.song = mode.amplitude = mode.phong = mode.cubeColor = mode.beat = 0;
    mode.cubeDim = 5;

    glUniform1i(glGetUniformLocation(program, "modeCubeDim"), mode.cubeDim);
    glUniform1i(glGetUniformLocation(program, "modeCubeColor"), mode.cubeColor);
    glUniform1i(glGetUniformLocation(program, "modePhong"), mode.phong);

    dumpInfo();
    // GL inits
    glClearColor(0.0, 0.0, 0.0, 0);
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

    // Create music
    music = new AudioPlayer((char *)"sound0.wav", 1024*2);

    // Play music
    music->play();

    // Load light
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 6, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 6, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 6, specularExponent);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 6, isDirectional);

    // Load projection matrix
    glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix);

    printError("init");
}

void transformAndDrawCubes()
{
    #define DIM mode.cubeDim
    float amplitude;
    for (int i = 0; i < music->getNumberFrequencies() || (mode.transformation == 0 && i < 6*DIM*DIM); i++)
    {
        int f = i % music->getNumberFrequencies();
        switch (mode.amplitude)
        {
            case 0: amplitude = music->getFrequencyBandBetween(f, f) / 1000000;
                    break;
            case 1: amplitude = 4 * music->getFrequencyBandBetween(f, f) / 1000000;
                    break;
            case 2: amplitude = log2(music->getFrequencyBandBetween(f, f) / music->getNumberFrequencies());
                    break;
        }
        if (amplitude < 0.01){
            amplitude = 0.01;
        }

        if (mode.transformation == 0)
        {
            if (i < DIM*DIM)
                Rz(0.0, rotMatrix);
            else if (i < 2*DIM*DIM)
                Rz(-PI/2, rotMatrix);
            else if (i < 3*DIM*DIM)
                Rz(-PI, rotMatrix);
            else if (i < 4*DIM*DIM)
                Rz(PI/2, rotMatrix);
            else if (i < 5*DIM*DIM)
                Rx(PI/2, rotMatrix);
            else if (i < 6*DIM*DIM)
                Rx(-PI/2, rotMatrix);
            else
                break;

            T(DIM/2 - i%DIM, 0.5 + amplitude/2 + DIM/2, DIM/2 - int((i%(DIM*DIM))/DIM), tranMatrix);

            glUniform1i(glGetUniformLocation(program, "cubeSide"), int(i / (DIM*DIM)));

            T(-0.5 + float(DIM)/2.0 - i%DIM, amplitude/2 + float(DIM)/2.0,-0.5 + float(DIM)/2.0 - int((i%(DIM*DIM))/DIM), tranMatrix);
            Mult(rotMatrix, tranMatrix, modelMatrix);
        }
        else if (mode.transformation == 1)
        {
            T(i%DIM+0.5, amplitude/2, i/DIM, tranMatrix);
            Rz(-PI/10*int(i/DIM), rotMatrix);
            Mult(rotMatrix, tranMatrix, modelMatrix);
        }
        else if (mode.transformation == 2)
        {
            T(i, amplitude/2, 0.0, tranMatrix);
            Rz(0, rotMatrix);
            Mult(rotMatrix, tranMatrix, modelMatrix);
        }
        else
            break;

        // Change hight on cube
        S(1.0, amplitude, 1.0, sizeMatrix);

        Mult(modelMatrix, sizeMatrix, modelMatrix);
        glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_TRUE, modelMatrix);
        //glUniform3f(glGetUniformLocation(program, "amplitude"), amplitude/10, 2.0-amplitude/10, 0*amplitude/10);
        glUniform1fv(glGetUniformLocation(program, "amplitude"), 1, &amplitude);
        firstCube.draw();
    }
}

#define NUMB_AMP 2
float preAmp[NUMB_AMP] =  {0};
int currentAmp = 0;
float oldLightBeat = 0;

void display(void)
{
    printError("pre display");
    //usleep(70000);

    moveCamera(cameraPosX, cameraPosY, cameraPosZ);

    /* Camera Matrix START */
    lookAt( cameraPosX, cameraPosY, cameraPosZ, // Camera pos
            lookAtPosX, lookAtPosY, lookAtPosZ, // Look at pos
            0.0,        1.0,        0.0,        // Up vector
            camMatrix);

    GLfloat camera_position[] = {(GLfloat)cameraPosX, (GLfloat)cameraPosY, (GLfloat)cameraPosZ};
    glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix);
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, camera_position);
    /* Camera Matrix END */


    music->doFFT();


    /* Beat START */
    float lightBeat;
    if (mode.beat == 0)
    {
        float bgColor = music->getFrequencyBandBetween(0, 5) / 10000000 / 6 -0.12;
        lightBeat = bgColor > 0.0 ? 3*bgColor : 0.0;
        glClearColor(lightBeat, lightBeat, lightBeat, 0);
    }
    else if (mode.beat == 1)
    {
        if (mode.phong == 0)
            lightBeat = music->getFrequencyBandBetween(0, 5) / 10000000/6 -0.12;
        else
            lightBeat = music->getFrequencyBandBetween(0, 5) / 10000000 -0.12;

        if (lightBeat < 0.1)
            lightBeat = 0.1;
        glClearColor(0.0, 0.0, 0.0, 0.0);
    }
    else
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        lightBeat = 1.0; 
    }

    /* Filter START */ 
    preAmp[currentAmp] = lightBeat;
    currentAmp = (currentAmp+1) % NUMB_AMP;
    lightBeat = 0;
    for(int i = 0; i < NUMB_AMP; i++)
    {
        lightBeat += preAmp[i];
    }
    lightBeat /= NUMB_AMP;
    oldLightBeat -= 0.05;
    lightBeat = (lightBeat > oldLightBeat ? lightBeat : oldLightBeat);
    //TODO:std::cout << lightBeat << std::endl;
    oldLightBeat = lightBeat;
    /* Filter END */
    
    if (mode.beat == 0)
        glUniform1f(glGetUniformLocation(program, "lightBeat"), 1.0);
    else
        glUniform1f(glGetUniformLocation(program, "lightBeat"), lightBeat);
    
    /* Beat END */
    
    
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    transformAndDrawCubes();

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
    glutInitWindowSize(900,900);
    glutCreateWindow ("Sound test");

    glutDisplayFunc(display);
    init();

    glutTimerFunc(20, &OnTimer, 0);

    try
    {
        glutMainLoop();
    }
    catch(const char* msg)
    {
        /* Q was pressed, exit main loop */
    }

    delete music;
    return EXIT_SUCCESS;
}

