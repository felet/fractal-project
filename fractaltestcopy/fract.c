#include "loadobj.h"
#include "GL_utilities.h"
#include <iostream>
#include "LoadTGA.h"
#include "VectorUtils2.h"
#include "assert.h"
#include "Cube.hpp"
#include "AudioPlayer.hpp"
#include <math.h>

// Step for print
int s = 0;

// music
AudioPlayer *music;
#define FFT_WINDOW_SIZE 1024
struct mode_type
{
    int song;
    int lightBeat;
    int cubeScaling;
    int demo;
    int reachedPos;
    int print;
    int wait;
}mode;
// camera things
Point3D lookat, campos;
float drot = 0.0;

// elapsed  time (ms)
float etime = 0.0;
float etimeOld = 0.0;
float worldClock = 0.0;
float clockSpeed = 1.0;
float printtime;
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
                                 {0.0f, 0.0f, 0.4f}, // Blue light
                                 {0.7f, 0.7f, 0.7f} }; // White light

Point3D lightSourcesDirectionsPositions[] = { {-40.0f, 40.0f, 40.0f}, // Red light, positional
                                       {100.0f, 100.0f, 100.0f}, // Green light, positional
                                       {100.0f, 100.0f, 100.0f}, // Blue light along X
                                       {1.0f, 1.0f, 1.0f} }; // White light along Z

GLfloat specularExponent[] = {5.0, 4.0, 20.0, 10.0};
GLint isDirectional[] = {0,0,1,1};


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


GLfloat lengthVector(Point3D v)
{
    return (GLfloat) sqrt((float)DotProduct(&v,&v));  
}

GLfloat sign(GLfloat a)
{
    return (a>0) ? 1:-1;
}

GLfloat max(GLfloat a, GLfloat b){
    return (a<b)?b:a;
}

//Steplength for moveToPoint
GLfloat stepLength = 0.2;
Point3D lookatPoint;

void printPosition(){
    printf("SetVector(%f, %f, %f, &asdf) \n", campos.x, campos.y, campos.z);
}

void printPosition2(int s){
    printf("SetVector(%f, %f, %f, &asdf[%d]); \n", campos.x, campos.y, campos.z, s);
}

void printLookAt(){
    printf("L %f, %f, %f \n", lookat.x, lookat.y, lookat.z);
}

void printLookAt2(int s){
    printf("SetVector(%f, %f, %f, &asdf2[%d]); \n", lookat.x, lookat.y, lookat.z, s);
}

void calcTrans();
void lookAt(GLfloat px, GLfloat py, GLfloat pz,
                    GLfloat lx, GLfloat ly, GLfloat lz,
                    GLfloat vx, GLfloat vy, GLfloat vz,
                    GLfloat *m);

char keymap[256];

#define size 29 // size = antal fasta punkter kameran skall passera = control points
#define numPoints 25 // numpoints = antal punkter som genereras mellan två control points

int step[2]={0,0};
Point3D path[size][numPoints];
Point3D lpath[size][numPoints];
void createMovement(Point3D cp[size], Point3D ret[size][numPoints])
{
	int i,j;
	Point3D dist;
	for(i=0;i<size;i++)
	{
		if (i!=size-1)
		{	
			VectorSub(&cp[i+1],&cp[i],&dist);
			for (j=0;j<numPoints;j++)
			{
				ret[i][j].x = cp[i].x + j*(dist.x/numPoints);
				ret[i][j].y = cp[i].y + j*(dist.y/numPoints);
				ret[i][j].z = cp[i].z + j*(dist.z/numPoints);					 
			}
		}
		else
		{
			for (j=0;j<numPoints;j++)
			{
				ret[i][j].x = cp[i].x;
				ret[i][j].y = cp[i].y;
				ret[i][j].z = cp[i].z;					 
			}
		}
	}
}

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
    else if (key==32)
    {
        clockSpeed = 0.0;
    }
    else if (key=='b')
    {
        calcTrans();
    }
    else if (key=='n' && spongelvl < MAX_LEVEL)
    {
        spongelvl++;
        printf("spongelevel: %d \n",spongelvl);
        dim = int(pow(3, spongelvl));
        calcTrans();
    }
    else if (key=='m' && spongelvl >= 0)
    {
        spongelvl--;
        printf("spongelevel: %d \n",spongelvl);
        dim = int(pow(3,spongelvl));
        calcTrans();
    }
    else if (key=='c')
    {
        s=0;
        if(mode.demo != 1)
            mode.demo = 1;
        else
            mode.demo = 0;
    }
    else if(key=='z')
    {
        printPosition2(s);
        s++;
    }
    else if(key=='x')
    {
        printLookAt2(s);
        s++;
    }
     else if (key=='i')
    {
        if(mode.print != 1)
            mode.print = 1;
        else
            mode.print = 0;
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
        << "n.\tIncrease recalc sponge" << std::endl
        << "m.\tDecrease dimensions on cube" << std::endl
        << "Space.\tPause clock" << std::endl
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

    #define TIME_INCREASE_STEP 0.2
    if (keyIsDown('+'))
    {
        clockSpeed += TIME_INCREASE_STEP;
    }
    else if (keyIsDown('-'))
    {
        clockSpeed -= TIME_INCREASE_STEP;
        if (clockSpeed < 0.0)
            clockSpeed = 0.0;
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

	Point3D asdf[size];
    SetVector(18.719795, 13.199673, -80.321762, &asdf[0]); 
    SetVector(17.320351, 13.199806, -52.356796, &asdf[1]); 
    SetVector(16.120827, 13.199921, -28.386826, &asdf[2]); 
    SetVector(15.221184, 13.200006, -10.409348, &asdf[3]); 
    SetVector(14.321541, 13.200092, 7.568130, &asdf[4]); 
    SetVector(14.021660, 13.200121, 13.560623, &asdf[5]); 
    SetVector(14.021660, 13.200121, 13.560623, &asdf[6]); 
    SetVector(14.021660, 13.200121, 13.560623, &asdf[7]); 
    SetVector(14.021660, 13.200121, 13.560623, &asdf[8]); 
    SetVector(-7.973577, 13.200226, 14.018110, &asdf[9]); 
    SetVector(-35.967537, 13.200359, 14.600367, &asdf[10]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[11]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[12]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[13]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[14]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[15]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[16]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[17]); 
    SetVector(-55.963264, 13.200455, 15.016264, &asdf[18]);
    SetVector(15.963264, 13.200455, 45.016264, &asdf[19]);
	SetVector(55.963264, 13.200455, 15.016264, &asdf[20]);
	SetVector(55.963264, 13.200455, -45.016264, &asdf[21]);
	SetVector(15, 15, -5, &asdf[22]);
	SetVector(15, 15, 14, &asdf[23]);
	SetVector(15, 15, -15, &asdf[24]);
	SetVector(15, 15, -30, &asdf[25]);
	SetVector(15, 15, -45, &asdf[26]);
	SetVector(40, 40, -80, &asdf[27]);
	SetVector(40, 40, -100, &asdf[28]);

	Point3D asdf2[size];
    SetVector(18.669815, 13.199677, -79.323013, &asdf2[0]); 
    SetVector(17.270370, 13.199811, -51.358047, &asdf2[1]); 
    SetVector(16.070847, 13.199925, -27.388077, &asdf2[2]); 
    SetVector(15.171204, 13.200011, -9.410599, &asdf2[3]); 
    SetVector(14.271561, 13.200097, 8.566879, &asdf2[4]); 
    SetVector(13.726140, 13.200126, 14.515960, &asdf2[5]); 
    SetVector(13.340021, 13.200126, 14.292312, &asdf2[6]); 
    SetVector(13.108896, 13.200126, 13.969110, &asdf2[7]); 
    SetVector(13.021876, 13.200126, 13.581418, &asdf2[8]); 
    SetVector(-8.973361, 13.200231, 14.038905, &asdf2[9]); 
    SetVector(-36.967323, 13.200364, 14.621161, &asdf2[10]); 
    SetVector(-56.926823, 13.200459, 15.283763, &asdf2[11]); 
    SetVector(-56.527908, 13.200459, 15.841599, &asdf2[12]); 
    SetVector(-55.813828, 13.200459, 16.005035, &asdf2[13]); 
    SetVector(-55.211983, 13.200459, 15.676247, &asdf2[14]); 
    SetVector(-54.977814, 13.200459, 15.186232, &asdf2[15]); 
    SetVector(-54.970551, 13.200459, 15.136767, &asdf2[16]); 
    SetVector(-54.270551, 13.200459, 15.10, &asdf2[17]); 
    SetVector(15, 15, 15, &asdf2[18]); 
    SetVector(15, 15, 15, &asdf2[19]); 
    SetVector(15, 15, 15, &asdf2[20]); 
    SetVector(15, 15, 15, &asdf2[21]); 
    SetVector(15, 15, 15, &asdf2[22]); 
    SetVector(15, 15, 15, &asdf2[23]); 
    SetVector(15, 15, 15, &asdf2[24]); 
    SetVector(40, 40, 40, &asdf2[25]); 
    SetVector(40, 40, 40, &asdf2[26]); 
    SetVector(40, 40, 40, &asdf2[27]); 
    SetVector(40, 40, 40, &asdf2[28]); 

    mode.demo = 1;

	createMovement(asdf,path);
	createMovement(asdf2,lpath);
    mode.song = 0;
    mode.cubeScaling = 0;
    mode.lightBeat = 0;
    clockSpeed = 1.0;

	// Initialize variables related to the camera
	SetVector(20.0f, 20.0f, -20.0f, &campos);
    drot = 0.0;
	SetVector(campos.x + sin(drot), 20.0f, campos.z + cos(drot), &lookat);

	// GL inits
	glClearColor(0.0, 0.0, 0.0, 1.0);
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
    dim = int(pow(3, MAX_LEVEL-1));
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
    return 0.5 * (music->getFrequencyBandBetween(0, 2) / 10000000.0);
}

#define NUMB_AMP 2
float preAmp[NUMB_AMP] =  {0};
int currentAmp = 0;
float oldLightBeat = 0;

void display(){
    printError("pre display");

    music->doFFT();

    if(mode.demo == 0)
        moveCamera();
    else if(mode.wait==0)
    {
        if (step[1] != size)
		{
            campos = path[step[1]][step[0]];
            lookat = lpath[step[1]][step[0]];
        //printf("step: %d,%d x: %f y: %f z: %f \n",step[1],step[0],path[step[1]][step[0]].x,path[step[1]][step[0]].y,path[step[1]][step[0]].z);
            step[0]++;
            if (step[0]==numPoints)
            {
                step[1]++;
                step[0]=0;
            //printf("---\n");
            }
            if(step[1]==16)
            {
                mode.lightBeat = 1;
                if(s <30)
                    mode.wait=1;
            }
            if(step[1]==27)
            {
                mode.cubeScaling = 2;
            }
            if(step[1]==25)
            {
               spongelvl = 4;
               dim = int(pow(3, spongelvl));
               calcTrans();
            }
        }
        else
        {
           mode.cubeScaling = 2; 
        }
    }
    else if(s == 30)
    {
        mode.lightBeat=0;
        mode.cubeScaling = 1;
    }
    else if(s == 50)
    {
        mode.wait=0;
        mode.cubeScaling = 2;
    }

    // Transformation matrices
    GLfloat camera[16], trans[16], skyboxMatrix[16], scaling[16];

    lookAt( campos.x, campos.y, campos.z, // Camera pos
            lookat.x, lookat.y, lookat.z, // Look at pos
			0.0,      1.0,      0.0, // Up vector
            camera);

    etime = glutGet(GLUT_ELAPSED_TIME); //Time variable
    glUniform1f(glGetUniformLocation(program, "time"), etime);

    if(printtime < floor(etime/1000))
    {

        s++;
        printtime = floor(etime/1000);
        if(mode.print ==1){
        printPosition2(s);
        printLookAt2(s);
        }
    }

    worldClock += clockSpeed * (etime - etimeOld) / 10000.0;

    // Light beat
    float lightBeat;
    if (mode.lightBeat == 1)
    {
        lightBeat = getBeat();
        lightBeat = (lightBeat < 0.05) ? 0.05 : lightBeat;
    }
    else
        lightBeat = 1.0;

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
    glUniform1f(glGetUniformLocation(program, "lightBeat"), lightBeat);
    oldLightBeat = lightBeat;

    // Cube Scaling
    GLfloat scale;
    if (mode.cubeScaling == 1)
        scale = 1.0 - getBeat();
    else if (mode.cubeScaling == 2)
        scale = fabs(sin(worldClock));
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
    //save old etime
    etimeOld = etime;
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


