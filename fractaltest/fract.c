//#include "loadobj.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "VectorUtils2.h"
#include "assert.h"
//#include <list>

float lookAtPosX = 0;
float lookAtPosY = 0;
float lookAtPosZ = 0;

float cameraPosX = 0;
float cameraPosY = 0.5;
float cameraPosZ = -2;
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
unsigned int vertexID;
unsigned int indexID;
unsigned int numIndices = 36;
unsigned int numVertices = 24;
GLfloat vertices[8][3] = {
    {-0.5,-0.5,-0.5}, 
    {0.5,-0.5,-0.5}, 
    {0.5,0.5,-0.5}, 
    {-0.5,0.5,-0.5}, 
    {-0.5,-0.5,0.5}, 
    {0.5,-0.5,0.5}, 
    {0.5,0.5,0.5}, 
{-0.5,0.5,0.5}};

typedef struct _Cube{
unsigned int vertexArrayObjID;
unsigned int vertexBufferObjID;
unsigned int indexBufferObjID;
    GLfloat v[8][3];
} Cube;

typedef struct _CubeList{
    Cube cube;
    struct CubeList* next;
} CubeList;

CubeList *mylist;

GLubyte cubeIndices[36] = {0,3,2, 0,2,1,
                           2,3,7, 2,7,6,
                           0,4,7, 0,7,3,
                           1,2,6, 1,6,5,
                           4,5,6, 4,6,7,
                           0,1,5, 0,5,4};

void lookAt(GLfloat px, GLfloat py, GLfloat pz,
                    GLfloat lx, GLfloat ly, GLfloat lz,
                    GLfloat vx, GLfloat vy, GLfloat vz,
                    GLfloat *m);
void OnTimer(int value);
void keyboardMovement();

CubeList *list_create(Cube data);
CubeList *list_add(CubeList *node, Cube data);
int list_remove(CubeList *list, CubeList *node);
Cube list_get_current(CubeList *list);
CubeList *list_get_next(CubeList *list);
void createCubes(Cube o, CubeList *list);
void uploadCube(Cube *c);
bool *list_not_empty(CubeList *list);
void drawCubes(const CubeList *l, GLfloat *totalMatrix);

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
	glGenVertexArrays(1, &vertexID);
	glBindVertexArray(vertexID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexID);
	glGenBuffers(1, &indexID);

    // VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
	printError("init vertices");

    //VBO for index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLubyte), cubeIndices, GL_STATIC_DRAW);
    Cube test;
    memcpy(test.v, vertices, 24*sizeof(GLfloat));
    test.vertexArrayObjID = vertexID;
    test.vertexBufferObjID = 12;
    test.indexBufferObjID = 15;
    mylist = list_create(test);
    createCubes(test,mylist);

 /*   mylist = list_get_next(mylist);
    Cube test2 = list_get_current(mylist);
    int i,j;
    for (i=0;i<3;i++)
        for(j=0;j<8;j++)
    printf("\n %f", test2.v[j][i]); */
}

void display(){
	keyboardMovement();

    float t = glutGet(GLUT_ELAPSED_TIME)/1000.0f; //Time variable
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
    // TODO: Rotera ej ljuskällor
    T(0, 0, 0, trans);
    Ry(t,rot);
    Mult(rot, trans, trans);
    Mult(camera, trans, totalMatrix); 
    Mult(projectionMatrix, totalMatrix, totalMatrix);
    
	// Upload matrices
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, totalMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rot);
    glUniform1i(glGetUniformLocation(program, "settexture"), settexture);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_TRUE, trans);

    // Cube
  //  glBindVertexArray(vertexID);			// Select VAO
    glUniform3f(glGetUniformLocation(program, "inColor"), 1.0,1.0,0.0);
 //   glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, NULL);

	drawCubes(mylist, totalMatrix);
	
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
	return 0;
}
/*
CubeList *list_create()
{
	CubeList *node;
	if(!(node=malloc(sizeof(CubeList)))) return NULL;
	node->cube=NULL;
	node->next=NULL;
	return node;
}
*/
/*
nya <=> gamla
5 <=> 0
7 <=> 1
3 <=> 2
1 <=> 3
4 <=> 4
6 <=> 5
2 <=> 6
0 <=> 7
- - -
+ - -
+ + -
- + -
- - +
+ - +
+ + +
- + +
*/
void drawCubes(const CubeList *l, GLfloat *totalMatrix){
	CubeList *tl = l;
	Cube c;
	GLfloat trans[16], color = 0, xspace = 0;	
	while(tl != NULL)
	{
		c = tl->cube;
		T(xspace,0,0, trans);
		Mult(trans,totalMatrix, totalMatrix);
    	glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, totalMatrix);
		glUniform3f(glGetUniformLocation(program, "inColor"), color, color/2.0 ,0.0);
    	glBindVertexArray(c.vertexArrayObjID);    // Select VAO
    	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, 0L);
		tl = tl->next;
		color = color + 0.1;
		xspace = xspace + 0.01;
	}
}

void createCubes(Cube o, CubeList *list){
double length = abs(o.v[7][0] - o.v[6][0]);
double radius = length/6.0;

GLfloat middle[3];
middle[0] = o.v[7][0] + radius;
middle[1] = o.v[7][1] + radius;
middle[2] = o.v[7][2] + radius;
Cube new;

new.v[0][0] = middle[0] - radius;
new.v[0][1] = middle[1] - radius;
new.v[0][2] = middle[2] - radius;

new.v[1][0] = middle[0] + radius;
new.v[1][1] = middle[1] - radius;
new.v[1][2] = middle[2] - radius;

new.v[2][0] = middle[0] + radius;
new.v[2][1] = middle[1] + radius;
new.v[2][2] = middle[2] - radius;

new.v[3][0] = middle[0] - radius;
new.v[3][1] = middle[1] + radius;
new.v[3][2] = middle[2] - radius;

new.v[4][0] = middle[0] - radius;
new.v[4][1] = middle[1] - radius;
new.v[4][2] = middle[2] + radius;

new.v[5][0] = middle[0] + radius;
new.v[5][1] = middle[1] - radius;
new.v[5][2] = middle[2] + radius;

new.v[6][0] = middle[0] + radius;
new.v[6][1] = middle[1] + radius;
new.v[6][2] = middle[2] + radius;

new.v[7][0] = middle[0] - radius;
new.v[7][1] = middle[1] + radius;
new.v[7][2] = middle[2] + radius;
/*
printf("\n mittpunkt x: y: z: %f", new.v[j][i]);
int i,j;
    for (i=0;i<3;i++)
        for(j=0;j<8;j++)
    printf("\n %f", new.v[j][i]);
 */
uploadCube(&new);
list_add(list,new);

Cube morecubes[26];
int i, trans[16];
for (i=0;i<26;i++)
{
	morecubes[i] = new;
	uploadCube(&morecubes[i]);
	list_add(list,morecubes[i]);
}
}

void uploadCube(Cube *c){
    glGenVertexArrays(1, &c->vertexArrayObjID);
    glGenBuffers(1, &c->vertexBufferObjID);
    glGenBuffers(1, &c->indexBufferObjID);
  //  glGenBuffers(1, &c.normalBufferObjID);
    
    glBindVertexArray(c->vertexArrayObjID);

    // VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, c->vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, 24*3*sizeof(GLfloat), c->v, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    // VBO for normal data
  /*  glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "inNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "inNormal"));
   */ 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->indexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint), cubeIndices, GL_STATIC_DRAW);

    printf("\n array id: %p \n", c->vertexArrayObjID);
}
bool *list_not_empty(CubeList *list){
 return (list->next != NULL);
}
CubeList *list_get_next(CubeList *list){
	return list->next;
}
CubeList *list_create(Cube data)
{
	CubeList *node;
    	node = malloc(sizeof(CubeList));
	//node->cube = (Cube) malloc(sizeof(Cube));
    	node->cube = data;
	node->next=NULL;
	return node;
}

CubeList *list_add(CubeList *node, Cube data)
{
	CubeList *newnode;
        newnode=list_create(data);
        newnode->next = node->next;
        node->next = newnode;
	return newnode;
}

int list_remove(CubeList *list, CubeList *node)
{
	while(list->next && list->next!=node) list=list->next;
	if(list->next) {
		list->next=node->next;
        free(node);
		return 0;		
	} else return -1;
}
Cube list_get_current(CubeList *list){
    return list->cube;
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

//TODO: fixa ordentlig kamera
void keyboardMovement()
{
    if (keyIsDown('w')){
        float tempX = lookAtPosX - cameraPosX;
        float tempY = lookAtPosY - cameraPosY;
        float tempZ = lookAtPosZ - cameraPosZ;
		tempX= 0.05;
		tempY= 0.05;
		tempZ= 0.05;
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
		tempX= 0.05;
		tempY= 0.05;
		tempZ= 0.05;

        cameraPosX -= tempX;
        cameraPosY -= tempY;
        cameraPosZ -= tempZ;

        lookAtPosX -= tempX;
        lookAtPosY -= tempY;
        lookAtPosZ -= tempZ;
    }

    if (keyIsDown('a')){

        drot += 0.05;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);

    }
    else if (keyIsDown('d')){

        drot -= 0.05;
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
