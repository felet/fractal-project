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
float cameraPosZ = -10;
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
unsigned int vertexArrayID;
unsigned int vertexBufferID;
unsigned int indexBufferID;
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
Cube list_get_cube(CubeList *list);
CubeList *list_get_next(CubeList *list);
void createCube(Cube o, CubeList *list);
void uploadCube(Cube *c);
bool *list_not_empty(CubeList *list);
void drawObject(const CubeList *l, GLfloat *cm, GLfloat *pm, GLfloat *tm, int offset, GLfloat *color);
void init(void)
{
	dumpInfo();

	// GL inits
	glClearColor(0.5,0.6,0.0,0);
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
	//LoadTGATextureSimple("name.tga", &tex);
    //printError("loading texture");

	// Load model
    //m1 = LoadModelPlus("name.obj", program, "in_Position", "in_Normal", "inTexCoord");

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
	
	//Add a cube to object list
    Cube obj;
    memcpy(obj.v, vertices, numVertices*sizeof(GLfloat));
    obj.vertexArrayObjID = vertexArrayID;
    obj.vertexBufferObjID = vertexBufferID;
    obj.indexBufferObjID = indexBufferID;
    mylist = list_create(obj);
    createCube(obj,mylist);
    CubeList *tl = mylist;
    int listlength = 0;
    while (tl!=NULL){
        listlength++;
        tl = tl->next;
    }
    printf("\n vi har : %d stycken kuber\n",listlength);
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
    GLfloat spacing[3] = {0, 0, 0};
    GLfloat color[3][3][3][3];
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
	//Ry(t,rot);
	//Mult(rot, trans, trans);
    //Mult(camera, trans, totalMatrix); 
    //Mult(projectionMatrix, totalMatrix, totalMatrix);
    
	// Upload matrices
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, totalMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rot);
    glUniform1i(glGetUniformLocation(program, "settexture"), settexture);
    glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_TRUE, trans);

    // Cube
    //glBindVertexArray(vertexArrayID);			// Select VAO
    glUniform3f(glGetUniformLocation(program, "inColor"), 0.0,0.0,0.0);
    //glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, NULL);
    GLfloat length = abs(list_get_cube(mylist).v[7][0] - list_get_cube(mylist).v[6][0]);
    int m,j,k,l,i;
    int spongelvl = 2;
    int dim = pow(3,spongelvl);

    for (j=0;j<3;j++)
        for(k=0;k<3;k++)
            for(l=0;l<3;l++)
                for(i=0;i<3;i++)
                     color[i][l][j][k] = 0.05*(j+k+l+i);

    GLfloat colorstep[3]={0.0,0.0,0.0};

    colorstep[0] =  0.05;
  //  length = length/pow(3,spongelvl);
    for(j=0;j<dim;j++){
        for(k=0;k<dim;k++){
            for(l=0;l<dim;l++){
                if (!(k==1 && l==1
                         || j==1 && l==1
                         || k==1 && j==1)){
                    T(length*j,length*k,length*l,trans);
                    drawObject(mylist,camera,projectionMatrix,trans,0,color[j][k][l]);
                }
            }
        }
    }

  /*  spacing[0] = abs(list_get_cube(mylist).v[7][0] - list_get_cube(mylist).v[6][0])/3.0;
	drawCubes(mylist, spacing, camera, projectionMatrix, trans, 1, 3, color);
    T(0.0, spacing[0], 0.0, trans);
    color[0] = 0.2;
    drawCubes(mylist,spacing,camera,projectionMatrix, trans, 1, 3, color);
    T(j*spacing[0], 2*spacing[0], 0.0, trans);
    color[1] = 0.2;
    drawCubes(mylist,spacing,camera,projectionMatrix, trans, 1, 3, color);
    */
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
	glutTimerFunc(50, &OnTimer, 0);
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
void drawObject(const CubeList *l, GLfloat *cm, GLfloat *pm, GLfloat* tm, int offset, GLfloat *color){
	CubeList *tl = l;
	Cube c;
    int i,j;
	GLfloat total_m[16], camera_m[16], projection_m[16], translation_m[16], local_trans[16];
    memcpy(camera_m, cm, 16*sizeof(GLfloat));
    memcpy(projection_m, pm, 16*sizeof(GLfloat));


    //Step to the right offset
    for (i=0;i<offset;i++)
    {
        if(tl != NULL)
            tl = tl->next;  
    }

    //Draw object
	c = list_get_cube(tl);
            
    //Transformations
    memcpy(translation_m, tm, 16*sizeof(GLfloat));
    Mult(camera_m, translation_m, total_m);
    Mult(projection_m, total_m, total_m);

            //Upload to shader
    glUniformMatrix4fv(glGetUniformLocation(program, "totalMatrix"), 1, GL_TRUE, total_m);
    glUniform3f(glGetUniformLocation(program, "inColor"), color[0], color[1] ,color[2]);
    glBindVertexArray(c.vertexArrayObjID);    // Select VAO
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_BYTE, 0L);
}

void createCube(Cube o, CubeList *list){
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

    //printf("\n array id: %p \n", c->vertexArrayObjID);
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
    glutTimerFunc(50, &OnTimer, value);
}

//TODO: fixa ordentlig kamera
/*
void keyboardMovement()
{
    if (keyIsDown('w')){
        float tempX = (lookAtPosX - cameraPosX)/4;
        float tempY = (lookAtPosY - cameraPosY)/4;
        float tempZ = (lookAtPosZ - cameraPosZ)/4;
        
        cameraPosX += tempX;
        cameraPosY += tempY;
        cameraPosZ += tempZ;

        lookAtPosX += tempX;
        lookAtPosY += tempY;
        lookAtPosZ += tempZ;

    }
    else if (keyIsDown('s')){
        float tempX = (lookAtPosX - cameraPosX)/4;
        float tempY = (lookAtPosY - cameraPosY)/4;
        float tempZ = (lookAtPosZ - cameraPosZ)/4;

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
    */
// TODO  sätt längden i w/s ist för a/d så att avståndet till punkten vi snurrar runt är konstant
void keyboardMovement()
{
    if (keyIsDown('w')){
        float tempX = (lookAtPosX - cameraPosX)/4;
        float tempY = (lookAtPosY - cameraPosY)/4;
        float tempZ = (lookAtPosZ - cameraPosZ)/4;
        
        cameraPosX += tempX;
        cameraPosY += tempY;
        cameraPosZ += tempZ;

        lookAtPosX += tempX;
        lookAtPosY += tempY;
        lookAtPosZ += tempZ;

    }
    else if (keyIsDown('s')){
        float tempX = (lookAtPosX - cameraPosX)/4;
        float tempY = (lookAtPosY - cameraPosY)/4;
        float tempZ = (lookAtPosZ - cameraPosZ)/4;

        cameraPosX -= tempX;
        cameraPosY -= tempY;
        cameraPosZ -= tempZ;

        lookAtPosX -= tempX;
        lookAtPosY -= tempY;
        lookAtPosZ -= tempZ;
    }

    if (keyIsDown('a')){

        drot += 0.05;
        Point3D camera, lookAt, temp;
        SetVector(cameraPosX,cameraPosY,cameraPosZ,&camera);
        SetVector(lookAtPosX,lookAtPosY,lookAtPosZ,&lookAt);
        VectorSub(&camera,&lookAt,&temp);
        float length =  DotProduct(&temp,&temp);
        length =  sqrt(length);
        cameraPosX =  lookAtPosX + length*sin(drot);
        cameraPosZ =  lookAtPosZ + length*cos(drot);

    }
    else if (keyIsDown('d')){

        drot -= 0.05;
        Point3D camera, lookAt, temp;
        SetVector(cameraPosX,cameraPosY,cameraPosZ,&camera);
        SetVector(lookAtPosX,lookAtPosY,lookAtPosZ,&lookAt);
        VectorSub(&camera,&lookAt,&temp);
        float length =  DotProduct(&temp,&temp);
        length =  sqrt(length);
        cameraPosX =  lookAtPosX + length*sin(drot);
        cameraPosZ =  lookAtPosZ + length*cos(drot);
    }
    if (keyIsDown('p')){

        lookAtPosY += 0.1;
    }
    else if (keyIsDown('l')){
        lookAtPosY -= 0.1;
    }

}
