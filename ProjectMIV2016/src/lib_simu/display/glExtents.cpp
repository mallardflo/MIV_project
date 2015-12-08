
#include "glExtents.h"
#include "glut.h"
#include <GL/glu.h>
#include <math.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define PIf		3.1415926535897932384f

void glexMatMult(float&, float&, float&, float*);
void glexMatMultNorm(float&, float&, float&, float*);
float glexDot(float*, float*);
int glexSign(float);
void glexDrawStencilShadow();


/**
 * Parametrizable sized box
 *
 */
void glexSolidBox(float size_x, float size_y, float size_z) {

	float sx = size_x/2.0f;
	float sy = size_y/2.0f;
	float sz = size_z/2.0f;

	glBegin(GL_QUADS);
	float t = 1.0f/3.0f;

	glNormal3f(0.0f, 0.0f, -1.0f);		//-Z
	glTexCoord2f(2.0f*t, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(2.0f*t, 1.0f*t);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(1.0f*t, 1.0f*t);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(1.0f*t, 0.0f*t);
	glVertex3f(sx, -sy, -sz);

	glNormal3f(0.0f, 0.0f, 1.0f);		//+Z
	glTexCoord2f(2.0f*t, 2.0f*t);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(2.0f*t, 3.0f*t);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(1.0f*t, 3.0f*t);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(1.0f*t, 2.0f*t);
	glVertex3f(-sx, -sy, sz);

	glNormal3f(0.0f, -1.0f, 0.0f);		//-Y
	glTexCoord2f(1.0f*t, 2.0f*t);
	glVertex3f(sx, -sy, -sz);
	glTexCoord2f(1.0f*t, 3.0f*t);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(0.0f*t, 3.0f*t);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(0.0f*t, 2.0f*t);
	glVertex3f(-sx, -sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);		//+Y
	glTexCoord2f(2.0f*t, 1.0f*t);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(2.0f*t, 2.0f*t);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(1.0f*t, 2.0f*t);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(1.0f*t, 1.0f*t);
	glVertex3f(sx, sy, -sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);		//-X
	glTexCoord2f(0.0f*t, 1.0f*t);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(1.0f*t, 1.0f*t);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(1.0f*t, 2.0f*t);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(0.0f*t, 2.0f*t);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);		//+X
	glTexCoord2f(3.0f*t, 2.0f*t);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(2.0f*t, 2.0f*t);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(2.0f*t, 1.0f*t);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(3.0f*t, 1.0f*t);
	glVertex3f(sx, -sy, -sz);

	glEnd();	

}

/**
 * Parametrizable sized box normally textured
 *
 */
void glexSolidBox2(float size_x, float size_y, float size_z) {

	float sx = size_x/2.0f;
	float sy = size_y/2.0f;
	float sz = size_z/2.0f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, -1.0f);		//-Z
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, -sy, -sz);

	glNormal3f(0.0f, 0.0f, 1.0f);		//+Z
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, sz);

	glNormal3f(0.0f, -1.0f, 0.0f);		//-Y
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, -sy, -sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);		//+Y
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, sy, -sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);		//-X
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);		//+X
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, -sy, -sz);

	glEnd();	

}

/*
 * Compute lookup table of cos and sin values forming a cirle
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */

static void fghCircleTable(double **sint,double **cost,const int n)
{
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*PIf/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
      if (*sint) free(*sint);
      if (*cost) free(*cost);
      return;
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}

/**
 * Draws a solid sphere
 *
 */
void glexSolidSphere(double radius, int slices, int stacks) {
    int i,j;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

    /* The top stack is covered with a triangle fan */

    z0 = 1.0;
    z1 = cost2[(stacks>0)?1:0];
    r0 = 0.0;
    r1 = sint2[(stacks>0)?1:0];

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,1);
        glVertex3d(0,0,radius);

        for (j=slices; j>=0; j--)
        {
            glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
            glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
        }

    glEnd();

    /* Cover each stack with a quad strip, except the top and bottom stacks */

    for( i=1; i<stacks-1; i++ )
    {
        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        glBegin(GL_QUAD_STRIP);

            for(j=0; j<=slices; j++)
            {
                glNormal3d(cost1[j]*r1,        sint1[j]*r1,        z1       );
                glVertex3d(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
                glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
                glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
            }

        glEnd();
    }

    /* The bottom stack is covered with a triangle fan */

    z0 = z1;
    r0 = r1;

    glBegin(GL_TRIANGLE_FAN);

        glNormal3d(0,0,-1);
        glVertex3d(0,0,-radius);

        for (j=0; j<=slices; j++)
        {
            glNormal3d(cost1[j]*r0,        sint1[j]*r0,        z0       );
            glVertex3d(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
        }

    glEnd();

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

/**
 *
 *
 */
void glexDefaultLightAndColor() {

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[] = { 0.3f, 0.3f, 0.3f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { -15.0f, 20.0f, 40.0f, 1.0f };
	//float Position[] = { 0.0f, 50.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	//enable 2D texture
	glEnable(GL_TEXTURE_2D);


	// select modulate to mix texture with color for shading
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// when texture area is large, bilinear filter the original
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// the texture wraps over at the edges (repeat)
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

}
void glexSetLightPos() {
	float Position[] = { -15.0f, 20.0f, 40.0f, 1.0f };
	//float Position[] = { 0.0f, 50.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
}

/**
 * Camera from polar coordinates
 *
 */
void glexCameraFromAnglesZoom(float angle_x, float angle_y, float zoom, float c_x, float c_y, float c_z) {

	gluLookAt(cos(angle_x)*cos(angle_y)*zoom + c_x, sin(angle_y)*zoom + c_y, sin(angle_x)*cos(angle_y)*zoom + c_z, c_x, c_y, c_z, 0.0f, 1.0f, 0.0f);

}

/**
 * Drawing text function
 *
 */
void glexDrawString(int x, int y,const char *string) {

	//set projection for 2D rendering mode
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, 0, 0, 1);
	glMatrixMode (GL_MODELVIEW);

	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);

	//draw text
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}

	glEnable(GL_LIGHTING);

}
void glexDrawNumber(int x, int y, double num) {
	std::ostringstream os;
	os.precision(5);
	os << num;
	std::string s = os.str();
	glexDrawString(x, y, s.c_str());
}
void glexDrawNumber(int x, int y, float num) {
	std::ostringstream os;
	os.precision(5);
	os << num;
	std::string s = os.str();
	glexDrawString(x, y, s.c_str());
}
void glexDrawNumber(int x, int y, int num) {
	std::ostringstream os;
	os << num;
	std::string s = os.str();
	glexDrawString(x, y, s.c_str());
}
void glexDrawNumber(int x, int y, long num) {
	std::ostringstream os;
	os << num;
	std::string s = os.str();
	glexDrawString(x, y, s.c_str());
}

unsigned char shadow_mask = 0;
unsigned char prev_shadow_mask = 0;


void glexInitShadowing() {
	shadow_mask = 0x01;
	prev_shadow_mask = 0x01;
}

/**
 * Given a transformation matrix and a light direction, draw solidbox extrusion in stencil buffer
 * with stencil shadow technique
 */
void glexSolidBoxShadow(float size_x, float size_y, float size_z) {

	size_x = size_x/2.0f;
	size_y = size_y/2.0f;
	size_z = size_z/2.0f;

	std::vector<Maths::Vector3> pts(8);
	int i = 0;
	pts[i++] = Maths::Vector3(-size_x, -size_y, size_z); 
	pts[i++] = Maths::Vector3(size_x, -size_y, size_z);
	pts[i++] = Maths::Vector3(size_x, size_y, size_z);
	pts[i++] = Maths::Vector3(-size_x, size_y, size_z);
	
	pts[i++] = Maths::Vector3(-size_x, -size_y, -size_z); 
	pts[i++] = Maths::Vector3(size_x, -size_y, -size_z);
	pts[i++] = Maths::Vector3(size_x, size_y, -size_z);
	pts[i++] = Maths::Vector3(-size_x, size_y, -size_z);

	int indices[] = {	0, 1, 2, 2, 3, 0,
						4, 6, 5, 4, 7, 6,
						0, 3, 7, 0, 7, 4,
						1, 5, 6, 1, 6, 2,
						3, 2, 6, 3, 6, 7,
						0, 5, 1, 0, 4, 5};
	int nb_indices = 3*12;

	//draw shadow for each triangle
	Maths::Vector3 p1, p2, p3;
	for (int h = 0 ; h < nb_indices ; h +=3) {
		p1 = pts[indices[h + 0]]; p2 = pts[indices[h + 1]]; p3 = pts[indices[h + 2]];
		glexDrawTriangleShadow(p1, p2, p3);
	}

}

void glexSolidSphereShadow(float radius, int slices, int stacks) {
    int i,j;

	Maths::Vector3 p1, p2, p3;

    /* Adjust z and radius as stacks are drawn. */

    double z0,z1;
    double r0,r1;

    /* Pre-computed circle */

    double *sint1,*cost1;
    double *sint2,*cost2;

    fghCircleTable(&sint1,&cost1,-slices);
    fghCircleTable(&sint2,&cost2,stacks*2);

    /* The top stack is covered with a triangle fan */

    z0 = 1.0;
    z1 = cost2[(stacks>0)?1:0];
    r0 = 0.0;
    r1 = sint2[(stacks>0)?1:0];

    //glBegin(GL_TRIANGLE_FAN);

	p1 = Maths::Vector3(0, 0, radius);
    //glVertex3d(0,0,radius);
	p2 = Maths::Vector3(cost1[slices]*r1*radius, sint1[slices]*r1*radius, z1*radius);
    for (j = slices - 1; j>=0; j--) {        
        p3 = Maths::Vector3(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
		glexDrawTriangleShadow(p1, p2, p3);
		p2 = p3;
    }

    //glEnd();

    /* Cover each stack with a quad strip, except the top and bottom stacks */
	Maths::Vector3 p4;
    for( i=1; i<stacks-1; i++ ) {

        z0 = z1; z1 = cost2[i+1];
        r0 = r1; r1 = sint2[i+1];

        //glBegin(GL_QUAD_STRIP);

		p1 = Maths::Vector3(cost1[0]*r1*radius, sint1[0]*r1*radius, z1*radius);
		p2 = Maths::Vector3(cost1[0]*r0*radius, sint1[0]*r0*radius, z0*radius);

        for(j = 1; j <= slices; j++) {
            p3 = Maths::Vector3(cost1[j]*r1*radius, sint1[j]*r1*radius, z1*radius);
            p4 = Maths::Vector3(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
			glexDrawTriangleShadow(p1, p2, p3);
			glexDrawTriangleShadow(p3, p2, p4);
			p1 = p3;
			p2 = p4;
        }

        //glEnd();
    }

    /* The bottom stack is covered with a triangle fan */

    z0 = z1;
    r0 = r1;

    //glBegin(GL_TRIANGLE_FAN);

	p1 = Maths::Vector3(0, 0, -radius);
    //glVertex3d(0,0,radius);
	p2 = Maths::Vector3(cost1[0]*r0*radius, sint1[0]*r0*radius, z0*radius);
    for (j=1; j<=slices - 1; j++) {
		p3 = Maths::Vector3(cost1[j]*r0*radius, sint1[j]*r0*radius, z0*radius);
		glexDrawTriangleShadow(p1, p2, p3);
		p2 = p3;
    }

    //glEnd();

    /* Release sin and cos tables */

    free(sint1);
    free(cost1);
    free(sint2);
    free(cost2);
}

//void glexSolidBoxShadow(float size_x, float size_y, float size_z, float* mat, float* light_dir) {
//
//	size_x = size_x/2.0f;
//	size_y = size_y/2.0f;
//	size_z = size_z/2.0f;
//
//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	//set render state
//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//	glDepthMask(GL_FALSE);
//	glDisable(GL_CULL_FACE);
//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(0.0f, 100.0f);
//
//	glEnable(GL_STENCIL_TEST);
//	glStencilFunc(GL_NOTEQUAL, 0xfe, 0xff); //
//	//unsigned char inv_mask = ~shadow_mask;
//	//glStencilFunc(GL_EQUAL, 0x00, inv_mask); //
//	//glStencilMask(shadow_mask);
//	glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
//	prev_shadow_mask = shadow_mask;
//	shadow_mask *= 2;//shadow_mask << 1;
//	if (shadow_mask == 0) shadow_mask = 1;
//
//	//vertex indices
//	static int indices[16*2] = {0, 1, 1, 2, 2, 3, 3, 0, 5, 4, 6, 5, 7, 6, 4, 7, 3, 7, 6, 2, 1, 5, 4, 0, 2, 1, 0, 3, 5, 6, 7, 4};
//
//	//edges normals (for silhouete detection)
//	float normals[12*6] = {
//		0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
//		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
//		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
//		0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
//		0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f,
//		0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f,
//		0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//		0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f,
//		-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
//		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
//		1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
//		-1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f
//	};
//
//
//	//extrude each edge of the cube
//	static float vertices[8*3];
//	int i = 0;
//	vertices[i++] = -size_x; vertices[i++] = -size_y; vertices[i++] = size_z;
//	vertices[i++] = size_x; vertices[i++] = -size_y; vertices[i++] = size_z;
//	vertices[i++] = size_x; vertices[i++] = size_y; vertices[i++] = size_z;
//	vertices[i++] = -size_x; vertices[i++] = size_y; vertices[i++] = size_z;
//	vertices[i++] = -size_x; vertices[i++] = -size_y; vertices[i++] = -size_z;
//	vertices[i++] = size_x; vertices[i++] = -size_y; vertices[i++] = -size_z;
//	vertices[i++] = size_x; vertices[i++] = size_y; vertices[i++] = -size_z;
//	vertices[i++] = -size_x; vertices[i++] = size_y; vertices[i++] = -size_z;
//
//	//extrude each edge
//	int nb_edges = 12;
//	int e_lenght = 20;
//	float x1, y1, z1;
//	float x2, y2, z2;
//	float nx1, ny1, nz1;
//	float nx2, ny2, nz2;
//	float s1, s2;
//	float x1e, y1e, z1e;
//	float x2e, y2e, z2e;
//	for (int  b = 0 ; b < 1 ; b++) {
//
//		//glEnable(GL_CULL_FACE);
//		//if (b == 0) {
//		//	glCullFace(GL_BACK);
//		//	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
//		//} else {
//		//	glCullFace(GL_FRONT);
//		//	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
//		//}
//
//		for(int e = 0 ; e < nb_edges ; e++) {
//			int h = e*2;
//			//get edge normals
//			nx1 = normals[e*6 + 0]; nx2 = normals[e*6 + 3];
//			ny1 = normals[e*6 + 1]; ny2 = normals[e*6 + 4];
//			nz1 = normals[e*6 + 2]; nz2 = normals[e*6 + 5];
//			//transform with given matrix
//			glexMatMultNorm(nx1, ny1, nz1, mat);
//			glexMatMultNorm(nx2, ny2, nz2, mat);
//			//compute angle with dot product
//			s1 = nx1*light_dir[0] + ny1*light_dir[1] + nz1*light_dir[2];
//			s2 = nx2*light_dir[0] + ny2*light_dir[1] + nz2*light_dir[2];
//			if (glexSign(s1) != glexSign(s2) || false) {
//				//get vertex coordinates from index
//				x1 = vertices[indices[h + 0]*3 + 0]; x2 = vertices[indices[h + 1]*3 + 0];
//				y1 = vertices[indices[h + 0]*3 + 1]; y2 = vertices[indices[h + 1]*3 + 1];
//				z1 = vertices[indices[h + 0]*3 + 2]; z2 = vertices[indices[h + 1]*3 + 2];
//				//transform with given matrix
//				glexMatMult(x1, y1, z1, mat);
//				glexMatMult(x2, y2, z2, mat);
//				//creates extrusion
//				x1e = x1 + light_dir[0]*e_lenght;
//				y1e = y1 + light_dir[1]*e_lenght;
//				z1e = z1 + light_dir[2]*e_lenght;
//				x2e = x2 + light_dir[0]*e_lenght;
//				y2e = y2 + light_dir[1]*e_lenght;
//				z2e = z2 + light_dir[2]*e_lenght;
//				//draw extrusion
//				glBegin(GL_QUADS);
//				glVertex3f(x1, y1, z1);
//				glVertex3f(x1e, y1e, z1e);
//				glVertex3f(x2e, y2e, z2e);
//				glVertex3f(x2, y2, z2);
//				glEnd();
//			}
//		}
//
//	}
//	//restor render state
//	glDisable(GL_POLYGON_OFFSET_FILL);
//	glEnable(GL_CULL_FACE);
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	glDepthMask(GL_TRUE);
//	glDisable(GL_STENCIL_TEST);
//	glStencilMask(0xff);
//	glCullFace(GL_BACK);
//
//	glexDrawStencilShadow();
//
//}


/**
 * 
 *
 */
void glexDrawShadow(float darkness) {

	//enable stencil test: a pixel must be equal to 1 to be drawn...
	glEnable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);			//erase all active bits.

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//draw a big shadow rect
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);

	glColor4f(0.0f, 0.0f, 0.0f, darkness);
	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 1);
		glVertex2i(1, 1);
		glVertex2i(1, 0);
	glEnd();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
}

void glexBeginFBShadowing() {
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(0.0f, 100.0f);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0xff, 0xff);
}
void glexEndFBShadowing() {
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
	glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);
}

//shadow variables
Maths::Matrix3 shadow_trans;
Maths::Matrix3 inv_shadow_trans;
Maths::Vector3 shadow_light_dir;
Maths::Vector3 shadow_light_dir_trans;
void glexSetShadowTrans(float* mat) {
	
	//shadow_trans = Maths::Matrix3(mat[M4_11], mat[M4_12], mat[M4_13],
	//									mat[M4_21], mat[M4_22], mat[M4_23],
	//									mat[M4_31], mat[M4_32], mat[M4_33]);
	shadow_trans = Maths::Matrix3(	mat[M4_11], mat[M4_21], mat[M4_31],
									mat[M4_12], mat[M4_22], mat[M4_32],
									mat[M4_13], mat[M4_23], mat[M4_33]);

	//shadow_trans = Maths::Matrix4(	mat[M4_11], mat[M4_21], mat[M4_31], mat[M4_41],
	//								mat[M4_12], mat[M4_22], mat[M4_32], mat[M4_42],
	//								mat[M4_13], mat[M4_23], mat[M4_33], mat[M4_43],
	//								mat[M4_14], mat[M4_24], mat[M4_34], mat[M4_44]);

	//shadow_trans = Maths::Matrix4(	mat[M4_11], mat[M4_12], mat[M4_13], mat[M4_14],
	//								mat[M4_21], mat[M4_22], mat[M4_23], mat[M4_24],
	//								mat[M4_31], mat[M4_32], mat[M4_33], mat[M4_34],
	//								mat[M4_41], mat[M4_42], mat[M4_43], mat[M4_44]);

	inv_shadow_trans = shadow_trans.Transpose();
	shadow_light_dir_trans = inv_shadow_trans*shadow_light_dir;
	shadow_light_dir_trans.normalise();
	shadow_light_dir_trans = shadow_light_dir_trans*200.0f;

}

void glexBeginFrontShadowing() {
	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
}

void glexBeginBackShadowing() {
	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
}

void glexSetShadowLightDir(Maths::Vector3& light_dir) {
	shadow_light_dir = light_dir;
	shadow_light_dir_trans = inv_shadow_trans*shadow_light_dir;
}

/**
 * Draw a triangle volume shadow for a specific triangle
 *
 */
void glexDrawTriangleShadow(Maths::Vector3& p1, Maths::Vector3& p2, Maths::Vector3& p3) {

	//check if the triangle faces thes light
	Maths::Vector3 n = (p3 - p2).crossProduct(p3 - p1);
	if (n.dotProduct(shadow_light_dir_trans) <= 0) return;

	//draws the volume
	Maths::Vector3 p1e = p1 + shadow_light_dir_trans;
	Maths::Vector3 p2e = p2 + shadow_light_dir_trans;
	Maths::Vector3 p3e = p3 + shadow_light_dir_trans;
	
	glBegin(GL_TRIANGLES);

	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p1e.x, p1e.y, p1e.z);
	glVertex3f(p2.x, p2.y, p2.z);

	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p1e.x, p1e.y, p1e.z);
	glVertex3f(p2e.x, p2e.y, p2e.z);

	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p2e.x, p2e.y, p2e.z);
	glVertex3f(p3.x, p3.y, p3.z);

	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p2e.x, p2e.y, p2e.z);
	glVertex3f(p3e.x, p3e.y, p3e.z);

	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p3e.x, p3e.y, p3e.z);
	glVertex3f(p1.x, p1.y, p1.z);

	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p3e.x, p3e.y, p3e.z);
	glVertex3f(p1e.x, p1e.y, p1e.z);

	glEnd();
}

/**
 * Fix existing shadow
 *
 */
void glexDrawStencilShadow() {

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	int n;
	glGetIntegerv(GL_STENCIL_BITS, &n);
	//enable stencil test: a pixel must be equal to 1 to be drawn...
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_LEQUAL, 0xfe, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			//erase all active bits.

	//draw a big shadow rect
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 0, 1);

	glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 1);
		glVertex2i(1, 1);
		glVertex2i(1, 0);
	glEnd();

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
}

/**
 * Vector 3 dor product
 *
 */
inline float glexDot(float* v1, float* v2) {
	return (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}

/**
 * get sign of a number
 *
 */
inline int glexSign(float n) {
	return (n < 0)?-1:1;
}

/**
 * Matrix multiplication
 *
 */
void glexMatMult(float& x, float& y, float& z, float* mat) {
	
	static float w = 1.0f;

	//float fx = mat[0]*x + mat[1]*y + mat[2]*z + mat[3]*w;
	//float fy = mat[4]*x + mat[5]*y + mat[6]*z + mat[7]*w;
	//float fz = mat[8]*x + mat[9]*y + mat[10]*z + mat[10]*w;

	float fx = mat[0]*x + mat[4]*y + mat[8]*z + mat[12]*w;
	float fy = mat[1]*x + mat[5]*y + mat[9]*z + mat[13]*w;
	float fz = mat[2]*x + mat[6]*y + mat[10]*z + mat[14]*w;

	x = fx; y = fy; z = fz;
}

void glexMatMultNorm(float& x, float& y, float& z, float* mat) {
	
	float fx = mat[0]*x + mat[4]*y + mat[8]*z;
	float fy = mat[1]*x + mat[5]*y + mat[9]*z;
	float fz = mat[2]*x + mat[6]*y + mat[10]*z;

	x = fx; y = fy; z = fz;
}

/**
 * Disabling vertical synchronization
 *
 */
#ifdef WIN32

#include <windows.h>

//function pointer typdefs
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);

//declare functions
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT					= NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT			= NULL;
 
//init VSync func
void initVSync() {

	//get extensions of graphics card
	char* extensions = (char*)glGetString(GL_EXTENSIONS);

	//is WGL_EXT_swap_control in the string? VSync switch possible?
	if (strstr(extensions,"WGL_EXT_swap_control")) {
		//get address's of both functions and save them
		wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)
		wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)
		wglGetProcAddress("wglGetSwapIntervalEXT");
	}
}

void glexSetVSyncState(bool enable) {
	static bool vsync_init = false;
	if (!vsync_init) initVSync();
	vsync_init = true;
	if (enable)
		wglSwapIntervalEXT(1); //set interval to 1 enable
	else
		wglSwapIntervalEXT(0); //disable
}

#else	//WIN32

void glexSetVSyncState(bool enable) {
	//do nothing for the moment !
}

#endif