#include "loadobj.h"
#include "GL_utilities.h"
#include <iostream>
#include "LoadTGA.h"
#include "VectorUtils2.h"
#include "assert.h"
#include "Cube.hpp"
#include "AudioPlayer.hpp"
#include <math.h>

// music
AudioPlayer *music;
#define FFT_WINDOW_SIZE 1024
struct mode_type
{
    int  song, lightBeat, cubeScaling, scalingTimeSpeed;
}mode;
// camera things
Point3D lookat, campos;
float drot = 0.0;

// elapsed  time (ms) 
float etime = 0.0;

//View frustum
#define near 1.0
#define far 900.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

//Texture constants
#define texOnly 0
#define texLight 1
#define texGenerated 2

GLfloat projectionMatrix[] = {  2.0f*near/(right-left), 0.0f,           (right+left)/(right-left), 0.0f,
                                0.0f, 2.0f*near/(top-bottom),           (top+bottom)/(top-bottom), 0.0f,
                                0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                0.0f, 0.0f,                             -1.0f,                     0.0f };

Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f}, // Red light
                                 {0.0f, 0.0f, 0.0f}, // Green light
                                 {0.0f, 0.0f, 1.0f}, // Blue light
                                 {1.0f, 1.0f, 1.0f} }; // White light

Point3D lightSourcesDirectionsPositions[] = { {15.0f, 15.0f, 15.0f}, // Red light, positional
                                       {-300.0f, 50.0f, 500.0f}, // Green light, positional
                                       {500.0f, 100.0f, 500.0f}, // Blue light along X
                                       {100.0f, 100.0f, 100.0f} }; // White light along Z

GLfloat specularExponent[] = {5.0, 400.0, 20.0, 10.0};
GLint isDirectional[] = {0,0,0,1};

GLuint program;
GLuint tex1,tex2; //Texture pointer

unsigned int vertexArrayID;
unsigned int vertexBufferID;
unsigned int indexBufferID;
unsigned int numIndices = 36;
unsigned int numVertices = 24;
unsigned int colorBufferObjID;
GLfloat vertices[8][3] = {
    {-0.5,-0.5,-0.5}, 
    {0.5,-0.5,-0.5}, 
    {0.5,0.5,-0.5}, 
    {-0.5,0.5,-0.5}, 
    {-0.5,-0.5,0.5}, 
    {0.5,-0.5,0.5}, 
    {0.5,0.5,0.5}, 
{-0.5,0.5,0.5}};

GLfloat colors[8][3] = {
    {1.0,0.0,0.0}, 
    {0.0,1.0,0.0}, 
{0.0,0.0,1.0}, 
{0.0,0.0,0.0}, 
{1.0,1.0,1.0}, 
{1.0,1.0,0.0}, 
{1.0,0.0,1.0}, 
{0.0,1.0,1.0}};

GLfloat normals[8][3] = {
    {-0.58,-0.58,-0.58}, 
    {0.58,-0.58,-0.58}, 
    {0.58,0.58,-0.58}, 
    {-0.58,0.58,-0.58}, 
    {-0.58,-0.58,0.58}, 
    {0.58,-0.58,0.58}, 
    {0.58,0.58,0.58}, 
{-0.58,0.58,0.58}};

GLubyte cubeIndices[36] = {0,3,2, 0,2,1,
                           2,3,7, 2,7,6,
                           0,4,7, 0,7,3,
                           1,2,6, 1,6,5,
                           4,5,6, 4,6,7,
                           0,1,5, 0,5,4};

Cube cube;
#define DIM  81
#define MAX_LEVEL 4
int spongelvl = MAX_LEVEL;
int dim = DIM;
GLfloat translationTA[DIM][DIM][DIM][16]; 
bool draw[DIM][DIM][DIM];
GLfloat color[DIM][DIM][DIM][3];
Model *skybox;
void calcTrans();
void lookAt(GLfloat px, GLfloat py, GLfloat pz,
                    GLfloat lx, GLfloat ly, GLfloat lz,
                    GLfloat vx, GLfloat vy, GLfloat vz,
                    GLfloat *m);

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

    if (key == '2')
    {
        // Change cube scaling 
        mode.cubeScaling = (mode.cubeScaling+1) % 3;
    }
    else if (key == '3')
    {
        // Change light mode
        mode.lightBeat= (mode.lightBeat+1) % 2;
    }
    else if (key == '5')
    {   // Change song
        mode.song = (mode.song+1) % 4;
        delete music;

        const char *number[] =  {"0", "1", "2", "3"};

        // Create music
        std::string fileName = std::string("sound")
            + std::string(number[mode.song])
            + std::string(".wav");
        std::cout << "Now playing: " <<  fileName << std::endl;
        music = new AudioPlayer(fileName.c_str(), FFT_WINDOW_SIZE);

        // Play music
        music->play();
    }
    else if (key=='+')
    {
        mode.scalingTimeSpeed++;
    }
    else if (key=='-' && mode.scalingTimeSpeed > 0)
    {
        mode.scalingTimeSpeed--;
    }
    else if (key=='b')
    {
        calcTrans();
    }
    else if (key=='m' && spongelvl < MAX_LEVEL)
    {
        spongelvl++;
        printf("spongelevel: %d \n",spongelvl);
        dim = int(pow(3, spongelvl));
        calcTrans();
    }
    else if (key=='n' && spongelvl >= 0)
    {
        spongelvl--;
        printf("spongelevel: %d \n",spongelvl);
        dim = int(pow(3,spongelvl));
        calcTrans();
    }
    else if (key==27)
    {
        throw "EXIT MAIN LOOP";
    }



	keymap[(unsigned int)key] = 1;
}

void initKeymapManager()
{
	for (int i = 0; i < 256; i++) keymap[i] = 0;

	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

    std::cout << std::endl
        << "2.\tChange cube scaling" << std::endl
        << "3.\tChange light beat" << std::endl
        << "5.\tChange song" << std::endl
        << "+.\tIncrease scaling time speed" << std::endl
        << "-.\tDecrease scaling time speed" << std::endl
        << "m.\tIncrease recalc sponge" << std::endl
        << "n.\tDecrease dimensions on cube" << std::endl
        << "Esc.\tQuit" << std::endl;
}


void moveCamera(){

    #define SCALE 2.0
    #define ROTATION 0.05
    float tempX = (lookat.x - campos.x) * SCALE;
    float tempY = (lookat.y - campos.y) * SCALE;
    float tempZ = (lookat.z - campos.z) * SCALE;

    if (keyIsDown('w'))
    {
        campos.x += tempX;
        campos.y += tempY;
        campos.z += tempZ;

        lookat.x += tempX;
        lookat.y += tempY;
        lookat.z += tempZ;
    }
    else if (keyIsDown('s'))
    {
        campos.x -= tempX;
        campos.y -= tempY;
        campos.z -= tempZ;

        lookat.x -= tempX;
        lookat.y -= tempY;
        lookat.z -= tempZ;
    }

    if (keyIsDown('a'))
    {
        drot += ROTATION;
        lookat.x = campos.x + sin(drot);
        lookat.z = campos.z + cos(drot);

    }
    else if (keyIsDown('d'))
    {
        drot -= ROTATION;
        lookat.x = campos.x + sin(drot);
        lookat.z = campos.z + cos(drot);
    }

    if (keyIsDown('p'))
        lookat.y += ROTATION;
    else if (keyIsDown('l'))
        lookat.y -= ROTATION;
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
void init(void)
{
	dumpInfo();

    mode.song = 0;
    mode.cubeScaling = 0;
    mode.lightBeat = 0;
    mode.scalingTimeSpeed = 1;

	// Initialize variables related to the camera
	SetVector(20.0f, 20.0f, -20.0f, &campos);
    drot = 0.0;
	SetVector(campos.x + sin(drot), 20.0f, campos.z + cos(drot), &lookat);

	// GL inits
	glClearColor(0.3,0.3,0.3,0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	printError("GL inits");

    initKeymapManager();

	// Load and compile shader
	program = loadShaders("fract.vert", "fract.frag");
    printError("init shader");

    // Load texture
    // LoadTGATextureSimple("SkyBox512.tga", &tex);
    LoadTGATextureSimple((char*)"SkyBox512.tga",&tex1);
    LoadTGATextureSimple((char*)"awesome.tga",&tex2);
    printError("init texture");

    glUniform1i(glGetUniformLocation(program, "texUnit"), 0);

	// Load model
    skybox = LoadModelPlus((char*)"skybox.obj", program, (char*)"in_Position",(char*) "in_Normal", (char*)"inTexCoord");

    // Upload lightsources
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);

	// Upload camera position (used in specular shading)
    GLfloat camera_position[] = {(GLfloat)campos.x, (GLfloat)campos.y, (GLfloat)campos.z};
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, camera_position);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projectionMatrix);
    // Init Cube
    cube.init(program);

    // Calculate transformation matrices for translation sponge
    calcTrans();

    // Create music
    music = new AudioPlayer((char *)"sound0.wav", FFT_WINDOW_SIZE);

    // Play music
    music->play();
}

void calcTrans()
{
    GLfloat length = 1.0;
	for(int j = 0; j < dim; j++)
	{
        for(int k = 0; k < dim; k++)
		{
            for(int l = 0; l < dim; l++)
			{
				draw[j][k][l] = true;
				for (int m = 0; m < spongelvl; m++)
                {
                    if (((j/(int)pow(3,m))%3==1 && (k/(int)pow(3,m))%3==1)
                     || ((l/(int)pow(3,m))%3==1 && (k/(int)pow(3,m))%3==1)
                     || ((l/(int)pow(3,m))%3==1 && (j/(int)pow(3,m))%3==1))
                    {
                        draw[j][k][l] = false;
                    }
                }
				if (draw[j][k][l])
				{
					T(length*j+1, length*k+1, length*l+1, translationTA[j][k][l]);
                    /*
                   // Transform based on time
                   int i = (int) 100.0*sin(etime/100.0);
                   if(i!=0)
                        if ( k%i==1 || l%i==1)
                           T(j+etime, k+etime, 0, translationTA[j][k][l]);
                        else if(k%(i+6)==1 || l%(i+6)==1)
                           T(j-etime, k-etime, 5, translationTA[j][k][l]);
                        else if(k%(i+12)==1 || l%(i+12)==1)
                           T(j+etime, k-etime, 10, translationTA[j][k][l]);
                           */
                }
            }
        }
    }

}

GLfloat getBeat()
{
    return music->getFrequencyBandBetween(0, 0) / 1000000.0;
}

void display(){

    printError("pre display");

    moveCamera();

    // Transformation matrices
    GLfloat camera[16], trans[16], skyboxMatrix[16], scaling[16];

    lookAt( campos.x, campos.y, campos.z, // Camera pos
            lookat.x, lookat.y, lookat.z, // Look at pos
			0.0,      1.0,      0.0, // Up vector
            camera);

    music->doFFT();
    etime = glutGet(GLUT_ELAPSED_TIME); //Time variable
    glUniform1f(glGetUniformLocation(program, "time"), etime);

    // Light beat
    float lightBeat;
    if (mode.lightBeat == 1)
        lightBeat = getBeat();
    else
        lightBeat = 1.0;
    glUniform1f(glGetUniformLocation(program, "lightBeat"), lightBeat);

    // Cube Scaling
    GLfloat scale;
    if (mode.cubeScaling == 1)
        scale = getBeat();
    else if (mode.cubeScaling == 2)
        scale = fabs(sin(float(mode.scalingTimeSpeed) * etime / 10000.0));
    else
        scale = 1.0;
    scale = (scale > 1.0) ? 1.0 : scale;
    S(scale, scale, scale, scaling);


	// Initialize matrices
    T(0, 0, 0, trans);

    // Skybox
    int setTexture = 2;
    glUniform1i(glGetUniformLocation(program, "setTexture"), setTexture);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Remove translation from the camera matrix
    for (int j = 0; j < 16; j++)
    {
        skyboxMatrix[j] = camera[j];
    }
    skyboxMatrix[3]=0;
    skyboxMatrix[7]=0;
    skyboxMatrix[11]=0;

    glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_TRUE, skyboxMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "translation"), 1, GL_TRUE, trans);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Texture upload
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(program, "modeScale"), 0);
    DrawModel(skybox);
    printError("Skybox");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Texture upload for cubes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex2);
    setTexture = 1;
    glUniform1i(glGetUniformLocation(program, "setTexture"), setTexture);

    glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_TRUE, camera);
    // Draw cubes
    glUniform1i(glGetUniformLocation(program, "modeScale"), 1);
    glUniformMatrix4fv(glGetUniformLocation(program, "scaling"), 1, GL_TRUE, scaling);
    for(int j=0;j<dim;j++)
	{
        for(int k=0;k<dim;k++)
		{
            for(int l=0;l<dim;l++)
			{
				if (draw[j][k][l])
				{
                    glUniformMatrix4fv(glGetUniformLocation(program, "translation"), 1, GL_TRUE, translationTA[j][k][l]);
                    cube.draw();
				}
            }
        }
    }
/*
    //Draw several translation sponge
	GLfloat AM[16];
    int i;
    for(int i=0;i<12;i++)
        for(int j=0;j<DIM;j++)
        {
            for(int k=0;k<DIM;k++)
            {
                for(int l=0;l<DIM;l++)
                {
                    if (draw[j][k][l])
                    {
                        T(27*i,0,0,AM);
                        Mult(AM,translationTA[j][k][l],AM);
                        Mult(projectionMatrix, AM, trans);
                        glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_TRUE, trans);
                        Mult(camera, AM,totalMatrix);
                        Mult(projectionMatrix, totalMatrix, totalMatrix);
                    }
                }
            }
        }
*/
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
	glutInitWindowSize(900, 900);
	glutCreateWindow("Fractal test");
	glutDisplayFunc(display);
	init();
	glutTimerFunc(20, &OnTimer, 0);

    try
    {
        glutMainLoop();
    }
    catch(const char* msg)
    {
        /* ESC was pressed, exit main loop */
    }

    delete music;
	return EXIT_SUCCESS;
}


