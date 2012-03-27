#include "loadobj.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "VectorUtils2.h"
#include "assert.h"

float lookAtPosX = 0;
float lookAtPosY = 9;
float lookAtPosZ = -15;

float cameraPosX = 0;
float cameraPosY = 9;
float cameraPosZ = -16;
float drot = 0;

#define near 1.0
#define far 90.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

GLfloat projectionMatrix[] = {  2.0f*near/(right-left), 0.0f,           (right+left)/(right-left), 0.0f,
                                0.0f, 2.0f*near/(top-bottom),           (top+bottom)/(top-bottom), 0.0f,
                                0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                0.0f, 0.0f,                             -1.0f,                     0.0f };

Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f}, // Red light
                                 {0.0f, 1.0f, 0.0f}, // Green light
                                 {0.0f, 0.0f, 1.0f}, // Blue light
                                 {1.0f, 1.0f, 1.0f} }; // White light

Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional
                                       {0.0f, 5.0f, 10.0f}, // Green light, positional
                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X
                                       {0.0f, 0.0f, -1.0f} }; // White light along Z

GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};
GLint isDirectional[] = {0,0,1,1};

GLuint program;
GLuint *tex; //Texture pointer

//TODO : Create cube 
unsigned int cubeID;
unsigned int numIndices = 6;
GLfloat vertices[] = {
    // Front face  
    -0.5, -0.5,  0.5,  
     0.5, -0.5,  0.5,  
     0.5,  0.5,  0.5,  
    -0.5,  0.5,  0.5,  
      
    // Back face  
    -0.5, -0.5, -0.5,  
    -0.5,  0.5, -0.5,  
     0.5,  0.5, -0.5,  
     0.5, -0.5, -0.5,  
      
    // Top face  
    -0.5,  0.5, -0.5,  
    -0.5,  0.5,  0.5,  
     0.5,  0.5,  0.5,  
     0.5,  0.5, -0.5,  
      
    // Bottom face  
    -0.5, -0.5, -0.5,  
     0.5, -0.5, -0.5,  
     0.5, -0.5,  0.5,  
    -0.5, -0.5,  0.5,  
      
    // Right face  
     0.5, -0.5, -0.5,  
     0.5,  0.5, -0.5,  
     0.5,  0.5,  0.5,  
     0.5, -0.5,  0.5,  
      
    // Left face  
    -0.5, -0.5, -0.5,  
    -0.5, -0.5,  0.5,  
    -0.5,  0.5,  0.5,  
    -0.5,  0.5, -0.5     
};

void lookAt(GLfloat px, GLfloat py, GLfloat pz,
                    GLfloat lx, GLfloat ly, GLfloat lz,
                    GLfloat vx, GLfloat vy, GLfloat vz,
                    GLfloat *m);
void OnTimer(int value);
void keyboardMovement();

void init(void)
{
	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.0,0);
	printError("GL inits");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	//glFrontFace(GL_CW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    initKeymapManager();

	// Load and compile shader
	program = loadShaders("fract.vert", "fract.frag");
	printError("init shader");

    // Load texture
	//LoadTGATextureSimple("name.tga", &tex);
    //printError("loading texture");

	// Load model
    //m1 = LoadModelPlus("name.obj", program, "in_Position", "in_Normal", "inTexCoord");

	// Allocate and activate Vertex Array Objects
	glGenVertexArrays(1, &cubeID);
	glBindVertexArray(cubeID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &cubeID);

    // VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, cubeID);
	//RM glBufferData(GL_ARRAY_BUFFER, 2*9*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
	printError("init vertex buffer object(s)");
}


void display(void)
{
	keyboardMovement();

    //float t = glutGet(GLUT_ELAPSED_TIME)/1000.0f; //Time variable
    int settexture = 0; //Use no texture, settexture = 1 to enable texture

    // clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Transformation matrices
    GLfloat camera[16], rot[16], trans[16], totalMatrix[16];

    lookAt( cameraPosX, cameraPosY, cameraPosZ, // Camera pos
            lookAtPosX, lookAtPosY, lookAtPosZ, // Look at pos
			0.0, 1.0, 0.0,  					// Up vector
            camera);

    printError("pre display");

    // Texture upload
    /*glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(program, "settexture"), settexture);*/
    
	// Upload lightsources
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);

	// Upload camera position (used in specular shading)
    GLfloat camera_position[] = {(GLfloat)cameraPosX, (GLfloat)cameraPosY, (GLfloat)cameraPosZ};
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, camera_position);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	// Initialize matrices
    T(0, 0, 0, trans);
    Mult(camera, trans, totalMatrix); 
    Mult(projectionMatrix, totalMatrix, totalMatrix);
    
	// Upload matrices
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, totalMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rot);
    glUniform1i(glGetUniformLocation(program, "settexture"), settexture);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_TRUE, trans);

    // Cube
    glBindVertexArray(cubeID);			// Select VAO
    glUniform3f(glGetUniformLocation(program, "color"), 0.3,0.0,0.0);
	glDrawArrays(GL_TRIANGLES, 0, numIndices);	// Draw object 
    
    printError("display");
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Fractal test");
	glutDisplayFunc(display); 
	init();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
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

void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}

void keyboardMovement()
{
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
