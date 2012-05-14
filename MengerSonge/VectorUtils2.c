// New version 120201:
// Defaults to matrices "by the book". Can also be configured to the flipped
// matrices that old OpenGL required (and we all hated).
// This is freshly implemented, limited testing, so there can be bugs.
// But it seems to work just fine on my tests with translation, rotations
// and matrix multiplications.


#include "VectorUtils2.h"

char transposed = 0;

	void CopyVector(Point3D *v, Point3D *dest)
	{
		dest->x = v->x;
		dest->y = v->y;
		dest->z = v->z;
	}

	void SetVector(GLfloat x, GLfloat y, GLfloat z, Point3D *v)
	{
		v->x = x;
		v->y = y;
		v->z = z;
	}

	void VectorSub(Point3D *a, Point3D *b, Point3D *result)
	{
		result->x = a->x - b->x;
		result->y = a->y - b->y;
		result->z = a->z - b->z;
	}

	void VectorAdd(Point3D *a, Point3D *b, Point3D *result)
	{
		result->x = a->x + b->x;
		result->y = a->y + b->y;
		result->z = a->z + b->z;
	}

	void CrossProduct(Point3D *a, Point3D *b, Point3D *result)
	{
		Point3D temp;

		temp.x = a->y*b->z - a->z*b->y;
		temp.y = a->z*b->x - a->x*b->z;
		temp.z = a->x*b->y - a->y*b->x;

		result->x = temp.x;
		result->y = temp.y;
		result->z = temp.z;
	}

	GLfloat DotProduct(Point3D *a, Point3D *b)
	{
		return a->x * b->x + a->y * b->y + a->z * b->z;
	}

	void ScalarMult(Point3D *a, GLfloat s, Point3D *result)
	{
		result->x = a->x * s;
		result->y = a->y * s;
		result->z = a->z * s;
	}

	GLfloat Norm(Point3D *a)
	{
		GLfloat result;

		result = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
		return result;
	}

	void Normalize(Point3D *a)
	{
		GLfloat result;

		result = sqrt(a->x * a->x + a->y * a->y + a->z * a->z);
		a->x = a->x / result;
		a->y = a->y / result;
		a->z = a->z / result;
	}

	void CalcNormalVector(Point3D *a, Point3D *b, Point3D *c, Point3D *result)
	{
		Point3D ab, ac, n;

		VectorSub(a, b, &ab);
		VectorSub(a, c, &ac);
		CrossProduct(&ab, &ac, &n);
		ScalarMult(&n, 1/Norm(&n), &n);
		result->x = n.x;
		result->y = n.y;
		result->z = n.z;
	}

	// Delar v i vn (parallell med n) och vp (vinkelrät). Kräver INTE att n är normerad!
	void SplitVector(Point3D *v, Point3D *n, Point3D *vn, Point3D *vp)
	{
		GLfloat nlen;
		GLfloat nlen2;

		nlen = DotProduct(v, n);
		nlen2 = n->x*n->x+n->y*n->y+n->z*n->z; // längen av n i kvadrat
		if (nlen2 == 0)
		{
			CopyVector(v, vp);
			SetVector(0, 0, 0, vn);
		}
		else
		{
			ScalarMult(n, nlen/nlen2, vn);
			VectorSub(v, vn, vp);
		}
	}

// Matrisoperationer på 4x4-matriser!
// Alla matriser är arrays av GLfloat!

	void IdentityMatrix(GLfloat *m)
	{
		int i;

		for (i = 0; i <= 15; i++)
			m[i] = 0;
		for (i = 0; i <= 3; i++)
			m[i * 5] = 1; // 0,5,10,15
	}

	void Rx(GLfloat a, GLfloat *m)
	{
		IdentityMatrix(m);
		m[5] = cos(a);
		if (transposed)
			m[9] = -sin(a);
		else
			m[9] = sin(a);
		m[6] = -m[9]; //sin(a);
		m[10] = m[5]; //cos(a);
	}

	void Ry(GLfloat a, GLfloat *m)
	{
		IdentityMatrix(m);
		m[0] = cos(a);
		if (transposed)
			m[8] = -sin(a);
		else
			m[8] = sin(a);
		m[2] = -m[8]; //sin(a);
		m[10] = m[0]; //cos(a);
	}

	void Rz(GLfloat a, GLfloat *m)
	{
		IdentityMatrix(m);
		m[0] = cos(a);
		if (transposed)
			m[4] = -sin(a);
		else
			m[4] = sin(a);
		m[1] = -m[4]; //sin(a);
		m[5] = m[0]; //cos(a);
	}

	void T(GLfloat tx, GLfloat ty, GLfloat tz, GLfloat *m)
	{
		IdentityMatrix(m);
		if (transposed)
		{
			m[12] = tx;
			m[13] = ty;
			m[14] = tz;
		}
		else
		{
			m[3] = tx;
			m[7] = ty;
			m[11] = tz;
		}
	}

	void S(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat *m)
	{
		IdentityMatrix(m);
		m[0] = sx;
		m[5] = sy;
		m[10] = sz;
	}

	void Mult(/*GLfloat *dest,*/ GLfloat *a, GLfloat *b, GLfloat *dest) // m = a * b
	{
		GLfloat	m[16];

		int x, y, i;
		for (x = 0; x <= 3; x++)
			for (y = 0; y <= 3; y++)
				if (transposed)
					m[x*4 + y] =	a[y+4*0] * b[0+4*x] +
								a[y+4*1] * b[1+4*x] +
								a[y+4*2] * b[2+4*x] +
								a[y+4*3] * b[3+4*x];
				else
					m[y*4 + x] =	a[y*4+0] * b[0*4+x] +
								a[y*4+1] * b[1*4+x] +
								a[y*4+2] * b[2*4+x] +
								a[y*4+3] * b[3*4+x];
		for (i = 0; i <= 15; i++)
			dest[i] = m[i];
	}

	// Matris gånger vektor
	// Vektorn 3D (ej 4D) gör att man får fuska till homogena koordinaten
	void MatrixMultPoint3D(GLfloat *a, Point3D *b, Point3D *result) // result = a * b
	{
		Point3D r;

		if (!transposed)
		{
			r.x = a[0]*b->x + a[1]*b->y + a[2]*b->z + a[3];
			r.y = a[4]*b->x + a[5]*b->y + a[6]*b->z + a[7];
			r.z = a[8]*b->x + a[9]*b->y + a[10]*b->z + a[11];
		}
		else
		{
			r.x = a[0]*b->x + a[4]*b->y + a[8]*b->z + a[12];
			r.y = a[1]*b->x + a[5]*b->y + a[9]*b->z + a[13];
			r.z = a[2]*b->x + a[6]*b->y + a[10]*b->z + a[14];
		//r.h = a[3]*b->x + a[7]*b->y + a[11]*b->z + a[15]*b->h;
		}

		*result = r;
	}


	void CopyMatrix(GLfloat *src, GLfloat *dest)
	{
		int i;
		for (i = 0; i <= 15; i++)
			dest[i] = src[i];
	}


// Added for lab 3

	// Orthonormalization of Matrix4D. Assumes rotation only, translation/projection ignored
	void OrthoNormalizeMatrix(GLfloat *R)
	{
		Point3D x, y, z;

		if (transposed)
		{
			SetVector(R[0], R[1], R[2], &x);
			SetVector(R[4], R[5], R[6], &y);
//		SetVector(R[8], R[9], R[10], &z);
			// Kryssa fram ur varandra
			// Normera
			CrossProduct(&x, &y, &z);
			Normalize(&z);
			Normalize(&x);
			CrossProduct(&z, &x, &y);
			R[0] = x.x;
			R[1] = x.y;
			R[2] = x.z;
			R[4] = y.x;
			R[5] = y.y;
			R[6] = y.z;
			R[8] = z.x;
			R[9] = z.y;
			R[10] = z.z;

			R[3] = 0.0;
			R[7] = 0.0;
			R[11] = 0.0;
			R[12] = 0.0;
			R[13] = 0.0;
			R[14] = 0.0;
			R[15] = 1.0;
		}
		else
		{
		// NOT TESTED
			SetVector(R[0], R[4], R[8], &x);
			SetVector(R[1], R[5], R[9], &y);
//		SetVector(R[2], R[6], R[10], &z);
			// Kryssa fram ur varandra
			// Normera
			CrossProduct(&x, &y, &z);
			Normalize(&z);
			Normalize(&x);
			CrossProduct(&z, &x, &y);
			R[0] = x.x;
			R[4] = x.y;
			R[8] = x.z;
			R[1] = y.x;
			R[5] = y.y;
			R[9] = y.z;
			R[2] = z.x;
			R[6] = z.y;
			R[10] = z.z;

			R[3] = 0.0;
			R[7] = 0.0;
			R[11] = 0.0;
			R[12] = 0.0;
			R[13] = 0.0;
			R[14] = 0.0;
			R[15] = 1.0;
		}
	}


	// Transponerar enbart rotationsdelen! Kopierar till ny matris.
	void Transpose(GLfloat *m, GLfloat *a)
	{
		a[0] = m[0]; a[4] = m[1]; a[8] = m[2];      a[12] = m[12];
		a[1] = m[4]; a[5] = m[5]; a[9] = m[6];      a[13] = m[13];
		a[2] = m[8]; a[6] = m[9]; a[10] = m[10];    a[14] = m[14];

		a[3] = m[3]; a[7] = m[7]; a[11] = m[11];    a[15] = m[15];
	}


// Rotation kring godtycklig axel (enbart rotationen)
void ArbRotate(Point3D *axis, GLfloat fi, GLfloat *m)
{
	Point3D x, y, z;//, a -> behˆvs ej?
	GLfloat R[16], Rt[16], Raxel[16], RtRx[16];

// Kolla också om parallell med Z-axel!
	if (axis->x < 0.0000001) // Under någon tillräckligt liten gräns
	if (axis->x > -0.0000001)
	if (axis->y < 0.0000001)
	if (axis->y > -0.0000001) {
		if (axis->z > 0)
		{
			Rz(fi, m);
			return;
		}
		else
		{
			Rz(-fi, m);
			return;
		}
	}

	x = *axis;
	Normalize(&x); // |x|
	SetVector(0,0,1, &z); // Temp z
	CrossProduct(&z, &x, &y);
	Normalize(&y); // y' = z^ x x'
	CrossProduct(&x, &y, &z); // z' = x x y

	if (transposed)
	{
		R[0] = x.x; R[4] = x.y; R[8] = x.z;  R[12] = 0.0;
		R[1] = y.x; R[5] = y.y; R[9] = y.z;  R[13] = 0.0;
		R[2] = z.x; R[6] = z.y; R[10] = z.z;  R[14] = 0.0;

		R[3] = 0.0; R[7] = 0.0; R[11] = 0.0;  R[15] = 1.0;
	}
	else
	{
		R[0] = x.x; R[1] = x.y; R[2] = x.z;  R[3] = 0.0;
		R[4] = y.x; R[5] = y.y; R[6] = y.z;  R[7] = 0.0;
		R[8] = z.x; R[9] = z.y; R[10] = z.z;  R[11] = 0.0;

		R[12] = 0.0; R[13] = 0.0; R[14] = 0.0;  R[15] = 1.0;
	}

	Transpose(R, Rt); // Transpose = Invert -> felet ej i Transpose, och det är en ortonormal matris

	Rx(fi, Raxel); // Rotate around x axis

	// m := Rt * Rx * R
	Mult(Rt, Raxel, RtRx);
	Mult(RtRx, R, m);
}


// KLART HIT


// Inte testad mycket. Hoppas jag inte vänt på den.
void CrossMatrix(Point3D *a, GLfloat *m) // Skapar matris för kryssprodukt
{
	m[0] =    0; m[4] =-a->z; m[8] = a->y; m[12] = 0.0;
	m[1] = a->z; m[5] =    0; m[9] =-a->x; m[13] = 0.0;
	m[2] =-a->y; m[6] = a->x; m[10]=    0; m[14] = 0.0;
	m[3] =  0.0; m[7] =  0.0; m[11]=  0.0; m[15] = 0.0;
	// OBS! 0.0 i homogena koordinaten. Därmed kan matrisen
	// inte användas generellt, men duger för matrisderivatan.
}

void MatrixAdd(GLfloat *a, GLfloat *b, GLfloat *dest)
{
	int i;
	for (i = 0; i < 16; i++)
		dest[i] = a[i] + b[i];
}


void SetTransposed(char t)
{
	transposed = t;
}
