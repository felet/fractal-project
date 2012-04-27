#include "assert.h"

#define near 1.0
#define far 90.0
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
float lookAtPosZ = 0;

float cameraPosX = 0;
float cameraPosY = 0;
float cameraPosZ = -5;
float drot = 0;

void moveCamera(float &cameraPosX, float &cameraPosY, float &cameraPosZ){

    #define SCALE 10
    #define ROTATION 0.05
    float tempX = (lookAtPosX - cameraPosX) / SCALE;
    float tempY = (lookAtPosY - cameraPosY) / SCALE;
    float tempZ = (lookAtPosZ - cameraPosZ) / SCALE;

    if (keyIsDown('w')){

        cameraPosX += tempX;
        cameraPosY += tempY;
        cameraPosZ += tempZ;

        lookAtPosX += tempX;
        lookAtPosY += tempY;
        lookAtPosZ += tempZ;
    }
    else if (keyIsDown('s')){

        cameraPosX -= tempX;
        cameraPosY -= tempY;
        cameraPosZ -= tempZ;

        lookAtPosX -= tempX;
        lookAtPosY -= tempY;
        lookAtPosZ -= tempZ;
    }

    if (keyIsDown('a')){

        drot += ROTATION;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);

    }
    else if (keyIsDown('d')){

        drot -= ROTATION;
        lookAtPosX = cameraPosX + sin(drot);
        lookAtPosZ = cameraPosZ + cos(drot);
    }

    if (keyIsDown('p')){

        lookAtPosY += ROTATION;
    }
    else if (keyIsDown('l')){
        lookAtPosY -= ROTATION;
    }


    if (keyIsDown('q'))
    {
       throw "END_MAIN_LOOP";
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

    Mult(r, t, m);
}

