#include "loadobj.h"
#include "GL_utilities.h"
#include "LoadTGA.h"
#include "VectorUtils2.h"
#include "assert.h"
#include <math.h>

//#include <list>

// camera things
Point3D lookat,campos,dir;
GLfloat ang = 0.05;
//Rotation
float xrot = 0;
float yrot = 0;
float angle = 0;
float drot = 0;

//Previous mouse position
float lastx, lasty;

//Camera speed
float cSpeed = 0.5;

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
                                 {0.0f, 1.0f, 0.0f}, // Green light
                                 {0.0f, 0.0f, 1.0f}, // Blue light
                                 {1.0f, 1.0f, 1.0f} }; // White light

Point3D lightSourcesDirectionsPositions[] = { {-235.0f, 250.0f, -200.0f}, // Red light, positional
                                       {-235.0f, 250.0f, 250.0f}, // Green light, positional
                                       {-220.0f, 220.0f, -150.0f}, // Blue light along X
                                       {0.0f, 0.0f, -1.0f} }; // White light along Z

GLfloat specularExponent[] = {10.0, 15.0, 20.0, 10.0};
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

typedef struct _Cube{
    unsigned int vertexArrayObjID;
    unsigned int vertexBufferObjID;
    unsigned int indexBufferObjID;
    unsigned int normalBufferObjID;
    GLfloat v[8][3];
	GLfloat n[8][3];
} Cube;

typedef struct _CubeList{
    Cube cube;
    struct _CubeList* next;
} CubeList;

CubeList *mylist;

GLubyte cubeIndices[36] = {0,3,2, 0,2,1,
                           2,3,7, 2,7,6,
                           0,4,7, 0,7,3,
                           1,2,6, 1,6,5,
                           4,5,6, 4,6,7,
                           0,1,5, 0,5,4};
/*
const int spongelvl = 4;
const int dim = (int) pow(3,spongelvl);
*/
int spongelvl = 3;
int dim = 27; // 3^4
#define DIM 27 // ersätt 81 med dim..
GLfloat mengerTA[DIM][DIM][DIM][16]; 
bool draw[DIM][DIM][DIM];
GLfloat color[DIM][DIM][DIM][3];
GLfloat color2[DIM][DIM][DIM][3];
Model *skybox;

void lookAt(GLfloat px, GLfloat py, GLfloat pz,
                    GLfloat lx, GLfloat ly, GLfloat lz,
                    GLfloat vx, GLfloat vy, GLfloat vz,
                    GLfloat *m);
void OnTimer(int value);
void keyboardMovement(unsigned char key, int x, int y);
//void mouseMovement(int x, int y);
//void camera();
CubeList *list_create(Cube data);
CubeList *list_add(CubeList *node, Cube data);
int list_remove(CubeList *list, CubeList *node);
Cube list_get_cube(CubeList *list);
CubeList *list_get_next(CubeList *list);
void uploadCube(Cube *c);
bool list_not_empty(CubeList *list);
void drawObject(CubeList *l, GLfloat *tm, int offset, GLfloat *color);
void init(void)
{
	// initialize things related to the camera..
	SetVector(20.0f,20.0f,20.0f,&lookat);
	SetVector(-5.0f,20.0f,-10.0f,&campos);
	VectorSub(&lookat, &campos, &dir);
	Normalize(&dir);
	dumpInfo();
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
  //  LoadTGATextureSimple("SkyBox512.tga", &tex);
    LoadTGATextureSimple("SkyBox512.tga",&tex1);
    LoadTGATextureSimple("awesome.tga",&tex2);
    printError("init texture");

    glUniform1i(glGetUniformLocation(program, "texUnit"), 0);

	// Load model
    skybox = LoadModelPlus("skybox.obj", program, "in_Position", "in_Normal", "inTexCoord");


	// Allocate and activate Vertex Array Objects
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &indexBufferID);

    // VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));
	printError("init vertices");

    //VBO for index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLubyte), cubeIndices, GL_STATIC_DRAW);

    //Color allocation
    glGenBuffers(1, &colorBufferObjID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, 24*sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Color"));
    printError("init colors");

	//Add a cube to object list
    Cube obj;
    memcpy(obj.v, vertices, numVertices*sizeof(GLfloat));
	memcpy(obj.n, normals, numVertices*sizeof(GLfloat));
    obj.vertexArrayObjID = vertexArrayID;
    obj.vertexBufferObjID = vertexBufferID;
    obj.indexBufferObjID = indexBufferID;
    uploadCube(&obj);
    mylist = list_create(obj);

    //createCube(obj,mylist);
    CubeList *tl = mylist;
    int listlength = 0;
    while (tl!=NULL){
        listlength++;
        tl = (CubeList *) tl->next;
    }
    printf("\n vi har : %d stycken kuber\n",listlength);
    GLfloat length = list_get_cube(mylist).v[7][0] - list_get_cube(mylist).v[6][0];
	length = length > 0.0 ? length : -1*length;
	int j,k,l;

	for(j=0;j<dim;j++)
	{
        for(k=0;k<dim;k++)
		{
            for(l=0;l<dim;l++)
			{
				draw[j][k][l] = true;
				for (int m=0;m<spongelvl;m++)
					{
						if (((j/(int)pow(3,m))%3==1 && (k/(int)pow(3,m))%3==1)
                         || ((l/(int)pow(3,m))%3==1 && (k/(int)pow(3,m))%3==1)
                         || ((l/(int)pow(3,m))%3==1 && (j/(int)pow(3,m))%3==1))
						{
							draw[j][k][l] = false;
						}
					}
				if (draw)
				{
					T(length*j,length*k,length*l,mengerTA[j][k][l]);
				}
            }
        }
    }


}

void display(){
    // float t = glutGet(GLUT_ELAPSED_TIME)/1000.0f; //Time variable

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    int j,k,l;
	// Transformation matrices
    GLfloat camera[16], rot[16], trans[16], totalMatrix[16], skyboxmatrix[16];

    lookAt( campos.x, campos.y, campos.z, // Camera pos
            lookat.x, lookat.y, lookat.z, // Look at pos
			0.0, 1.0, 0.0, // Up vector
            camera);

    printError("pre display");

    // Texture upload
    /*glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
    glUniform1i(glGetUniformLocation(program, "setTexture"), setTexture);*/

	// Upload lightsources
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);

	// Upload camera position (used in specular shading)
    GLfloat camera_position[] = {(GLfloat)campos.x, (GLfloat)campos.y, (GLfloat)campos.z};
    glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, camera_position);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	// Initialize matrices
    // TODO: Rotera ej ljuskällor
    T(0, 0, 0, trans);

    //skybox
    int setTexture = 0; 
    glUniform1i(glGetUniformLocation(program, "setTexture"), setTexture);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    for (int j = 0; j < 16; j++)
    {
        skyboxmatrix[j] = camera[j];
    }
    skyboxmatrix[3]=0;
    skyboxmatrix[7]=0;
    skyboxmatrix[11]=0;
    Mult(projectionMatrix, skyboxmatrix, skyboxmatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, skyboxmatrix);

    // Texture upload
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    DrawModel(skybox);
    printError("Skybox");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Texture upload for cubes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex2);
    setTexture = 1;
    glUniform1i(glGetUniformLocation(program, "setTexture"), setTexture);

    for(j=0;j<dim;j++)
	{
        for(k=0;k<dim;k++)
		{
            for(l=0;l<dim;l++)
			{
				if (draw[j][k][l])
				{
					Mult(projectionMatrix, mengerTA[j][k][l], trans);
    				glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_TRUE, trans);
					Mult(camera, mengerTA[j][k][l],totalMatrix);
					Mult(projectionMatrix, totalMatrix, totalMatrix);
					drawObject(mylist,totalMatrix,0,color[j][k][l]);
				}
            }
        }
    }


	GLfloat AM[16];
int i;
for(i=0;i<12;i++)
	for(j=0;j<dim;j++)
	{
        for(k=0;k<dim;k++)
		{
            for(l=0;l<dim;l++)
			{
				if (draw[j][k][l])
				{
					T(27*i,0,0,AM);
					Mult(AM,mengerTA[j][k][l],AM);
					Mult(projectionMatrix, AM, trans);
    				glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_TRUE, trans);
					Mult(camera, AM,totalMatrix);
					Mult(projectionMatrix, totalMatrix, totalMatrix);
					drawObject(mylist,totalMatrix,0,color2[j][k][l]);
				}
            }
        }
    }

    printError("display");
    glutSwapBuffers();

}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800,800);
	glutCreateWindow("Fractal test");
	glutDisplayFunc(display);
	init();
	glutTimerFunc(20, &OnTimer, 0);

    //glutPassiveMotionFunc(mouseMovement);
    glutKeyboardFunc(keyboardMovement);

	glutMainLoop();
	return 0;
}
void drawObject(CubeList *l, GLfloat* tm, int offset, GLfloat *color){
	CubeList *tl = l;
	Cube c;
    int i;

    //Step to the right offset
    for (i=0;i<offset;i++)
    {
        if(tl != NULL)
            tl = (CubeList *) tl->next;
    }

    //Draw object
	c = list_get_cube(tl);
    //Upload to shader
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, tm);
   // glUniform3f(glGetUniformLocation(program, "inColor"), color[0], color[1] ,color[2]);
    glBindVertexArray(c.vertexArrayObjID);    // Select VAO
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, 0L);
}

void uploadCube(Cube *c){
    glGenVertexArrays(1, &c->vertexArrayObjID);
    glGenBuffers(1, &c->vertexBufferObjID);
    glGenBuffers(1, &c->indexBufferObjID);
    glGenBuffers(1, &c->normalBufferObjID);

    glBindVertexArray(c->vertexArrayObjID);

    // VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, c->vertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, 8*3*sizeof(GLfloat), c->v, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, c->normalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, 8*3*sizeof(GLfloat), &normals, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, c->indexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(GLuint), cubeIndices, GL_STATIC_DRAW);
}

bool list_not_empty(CubeList *list){
 return (list->next != NULL);
}

CubeList *list_get_next(CubeList *list){
	return (CubeList *) list->next;
}

CubeList *list_create(Cube data)
{
	CubeList *node;
    	node = (CubeList*) malloc(sizeof(CubeList));
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
	while(list->next && (CubeList *) list->next!=node) list= (CubeList *)list->next;
	if(list->next) {
		list->next=node->next;
        free(node);
		return 0;
	} else return -1;
}

Cube list_get_cube(CubeList *list){
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


void keyboardMovement (unsigned char key, int x, int y) {
	if (key == 'w')
	{	
	 VectorSub(&lookat, &campos, &dir);
	 Normalize(&dir);
	 campos.x = campos.x+1.0f*dir.x;
	 campos.y = campos.y+1.0f*dir.y;
	 campos.z = campos.z+1.0f*dir.z;
	 lookat.x = lookat.x+1.0f*dir.x;
	 lookat.y = lookat.y+1.0f*dir.y;
	 lookat.z = lookat.z+1.0f*dir.z;
	 SetVector(campos.x,campos.y,campos.z,&campos);
	}
	 
	else if (key=='s')
	 {	
	 VectorSub(&lookat, &campos, &dir);
	 Normalize(&dir);
	 campos.x = campos.x-1.0f*dir.x;
	 campos.y = campos.y-1.0f*dir.y;
	 campos.z = campos.z-1.0f*dir.z;
	 lookat.x = lookat.x-1.0f*dir.x;
	 lookat.y = lookat.y-1.0f*dir.y;
	 lookat.z = lookat.z-1.0f*dir.z;
	 SetVector(campos.x,campos.y,campos.z,&campos);
	 }
	  
	 if (key=='d')
	 {
	 VectorSub(&lookat, &campos, &dir);
	 Normalize(&dir);

	 lookat.x = (float)(campos.x+cos(ang)*dir.x - sin(ang)*dir.z);
	 lookat.z = (float)(campos.z+sin(ang)*dir.x + cos(ang)*dir.z);
	 }
	 
	else if (key=='a')
	 {
	 VectorSub(&lookat, &campos, &dir);
	 Normalize(&dir);
	 lookat.x = (float)(campos.x+cos(-ang)*dir.x - sin(-ang)*dir.z);
	 lookat.z = (float)(campos.z+sin(-ang)*dir.x + cos(-ang)*dir.z);
	 }

    if (key=='p')
        lookat.y += 0.3;
    else if (key=='l')
        lookat.y -= 0.3;

    if (key==27)
    {
        exit(0);
    }
}
/*
void mouseMovement(int x, int y) {
    int diffx=x-lastx;
    int diffy=y-lasty;
    lastx=x;
    lasty=y;
    xrot += (float) diffy; //set the xrot to xrot with the addition
    yrot += (float) diffx;    //set the xrot to yrot with the addition
}

void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);
    glRotatef(yrot,0.0,1.0,0.0);
}*/
