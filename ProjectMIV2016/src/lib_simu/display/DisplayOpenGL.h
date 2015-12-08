
#ifndef DISPLAY_OPEN_GL_H
#define DISPLAY_OPEN_GL_H

//OpenGL
#include "glut.h"
#include <GL/glu.h>
#include "glExtents.h"

#include <math.h>
#include "AllMath.h"

/*! \class Display
 * \brief Access to hardware accelerated 3D drawing functions
 *
 * This class defines a bsic access to either OpenGL or Direct3D primitive functions.
 * So far, only the OpenGL version has been implemented
 *
 */
class Display {


private:

	static void* object_instance;
	static void (*renderText)(void*, int, int, std::string, int);

public:

	//----------------------------------------------------------------------------------------------------------
	//Rendering library specific--------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------
	
	/*
	 *initialisations
	 *
	 *
	 */
	static void initDisplay(int win_id, int width, int height) {

		//set environmant atmosphere
		glexDefaultLightAndColor();

		// Setup default render states
		glClearColor(0, 0, 0.0, 1.0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);

		//other options
		//glexSetVSyncState(false);
		setDisplaySize(width, height);

	}

	static void Display::setDisplaySize(int width, int height) {
		render_width = width;
		render_height = height;
		glViewport(0, 0, width, height);
	}

	//Projection and camera
	static void Display::setProjection(float fov, float near_plane, float far_plane) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, (float)render_width/(float)render_height, near_plane, far_plane);
		glMatrixMode(GL_MODELVIEW);
	}

	static void Display::setCameraFromSphericalCoords(float view_angle_x, float view_angle_y, float view_zoom, Maths::Vector3& view_center) {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glexCameraFromAnglesZoom(view_angle_x, view_angle_y, view_zoom, view_center.x, view_center.y, view_center.z);
		glexSetLightPos();
	}

	/**
	 * Transformation managment
	 *
	 */
	inline static void setViewIdentity() {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	inline static void pushMatrix() { glPushMatrix(); }
	inline static void popMatrix() { glPopMatrix(); }
	inline static void multMatrix(float* mat) { glMultMatrixf(mat); }
	inline static void setMatrix(float* mat) {glLoadMatrixf(mat); }

	inline static void setDrawColor(float r, float g, float b) { glColor3f(r, g, b); }

	inline static void drawLine(float x1, float y1, float z1, float x2, float y2, float z2) {
		glBegin(GL_LINES);
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y2, z2);
		glEnd();
	}

	inline static void drawLine(const Maths::Vector3& p1, const Maths::Vector3& p2) {
		drawLine(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
	}

	inline static void drawPoint(Maths::Vector3& p, float size) {
		drawPoint(p.x, p.y, p.z, size);
	}

	inline static void setPointSize(Maths::Real size) {
		glPointSize(size);
	}

	inline static void drawPoint(Maths::Vector3& p) {
		glBegin(GL_POINTS);
		glVertex3f(p.x, p.y, p.z);
		glEnd();
	}

	inline static void drawPoint(float x, float y, float z, float size) {
		drawLine(x - size, y, z, x + size, y, z);
		drawLine(x, y - size, z, x, y + size, z);
		drawLine(x, y, z - size, x, y, z + size);
	}

	inline static void setRenderTextFunction(void* instance, void (*renderTextFnc)(void*, int, int, std::string, int)) { 
		object_instance = instance;
		renderText = renderTextFnc;
	}

	inline static void drawText(float x, float y, std::string text, float size) {
		if (renderText != NULL)	renderText(object_instance, x, y, text.c_str(), size);
	}

	//clear
	inline static void clearAllBuffers() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); }

	inline static void clearDepth() { glClear(GL_DEPTH_BUFFER_BIT); }

	inline static void setOffsetFill(bool enable, float dist = -10000.0f, float scale = 1.0f) {
		if (enable) glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(scale, dist);
		if (!enable) glDisable(GL_POLYGON_OFFSET_FILL);
	}

	inline static void setDepthTest(bool enable) { 
		if (enable) glEnable(GL_DEPTH_TEST);
		if (!enable) glDisable(GL_DEPTH_TEST);
	}

	inline static void setLighting(bool enable) {
		if (enable) glEnable(GL_LIGHTING);
		if (!enable) glDisable(GL_LIGHTING);
	}

	/**
	 * Render state
	 *
	 */
	inline static void setFog(bool enable, fogType type, float density, float start, float end, rgbColor color = rgbColor(0.0f, 0.0f, 0.0f)) {
		if (!enable) {
			glDisable(GL_FOG);
			return;
		}
		if (type == FOG_LINEAR) glFogi(GL_FOG_MODE, GL_LINEAR);
		if (type == FOG_EXP) glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, density);
		glFogf(GL_FOG_START, start);
		glFogf(GL_FOG_END, end);
		glEnable(GL_FOG);

	}

	/**
	 * Projection
	 *
	 */
	inline static void unProject(int xi, int yi, float depth, Maths::Vector3& v) {

		//get the transformations
		GLint viewPort[4];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		glGetIntegerv(GL_VIEWPORT, viewPort);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

		yi = viewPort[3] - yi - 1;
		GLdouble wx, wy, wz;

		//unproject
		gluUnProject((GLdouble)xi, (GLdouble)yi, (GLdouble)depth, modelMatrix, projMatrix, viewPort, &wx, &wy, &wz);
		
		v.set((Maths::Real)wx, (Maths::Real)wy, (Maths::Real)wz);

	}

	/**
	 * Drawing functions
	 *
	 */
	inline static void solidBox(float size_x, float size_y, float size_z) { glexSolidBox(size_x, size_y, size_z); }
	inline static void solidBox2(float size_x, float size_y, float size_z) { glexSolidBox2(size_x, size_y, size_z); }
	inline static void solidSphere(float diameter) { glexSolidSphere(diameter/2.0f); }
	inline static void solidBoxShadow(float size_x, float size_y, float size_z) { glexSolidBoxShadow(size_x, size_y, size_z); }
	inline static void solidSphereShadow(float diameter) { glexSolidSphereShadow(diameter/2.0f); }

	inline static void setNormal(float dx, float dy, float dz) { glNormal3f(dx, dy, dz); }
	inline static void setVertex(float x, float y, float z) { glVertex3f(x, y, z); }
	inline static void drawTriangle(float x1, float y1, float z1, float nx1, float ny1, float nz1,
									float x2, float y2, float z2, float nx2, float ny2, float nz2,
									float x3, float y3, float z3, float nx3, float ny3, float nz3) { 
		glBegin(GL_TRIANGLES);
		glNormal3f(nx1, ny1, nz1);
		glVertex3f(x1, y1, z1);
		glNormal3f(nx2, ny2, nz2);
		glVertex3f(x2, y2, z2);
		glNormal3f(nx3, ny3, nz3);
		glVertex3f(x3, y3, z3);
		glEnd();
	}

	inline static void drawTriangle(Maths::Vector3& p1, Maths::Vector3& n1,
									Maths::Vector3& p2, Maths::Vector3& n2,
									Maths::Vector3& p3, Maths::Vector3& n3) {
		Display::drawTriangle(	p1.x, p1.y, p1.z, n1.x, n1.y, n1.z,
								p2.x, p2.y, p2.z, n2.x, n2.y, n2.z,
								p3.x, p3.y, p3.z, n3.x, n3.y, n3.z);
	}

	inline static void drawTriangle(float x1, float y1, float z1, rgbColor c1,
									float x2, float y2, float z2, rgbColor c2,
									float x3, float y3, float z3, rgbColor c3) { 
		glBegin(GL_TRIANGLES);
		glColor3f(c1.r, c1.g, c1.b);
		glVertex3f(x1, y1, z1);
		glColor3f(c2.r, c2.g, c2.b);
		glVertex3f(x2, y2, z2);
		glColor3f(c3.r, c3.g, c3.b);
		glVertex3f(x3, y3, z3);
		glEnd();
	}

	inline static void beginFBShadowing() { glexBeginFBShadowing(); }
	inline static void endFBShadowing() { glexEndFBShadowing(); }
	inline static void setShadowTrans(float* mat) { glexSetShadowTrans(mat); }
	inline static void setShadowLightDir(Maths::Vector3& light_dir) { glexSetShadowLightDir(light_dir); }
	inline static void drawTriangleShadow(Maths::Vector3& p1, Maths::Vector3& p2, Maths::Vector3& p3) { glexDrawTriangleShadow(p1, p2, p3); }
	inline static void beginFrontShadowing() { glexBeginFrontShadowing(); }
	inline static void beginBackShadowing() { glexBeginBackShadowing(); }

	inline static void initShadowing() { glexInitShadowing(); }
	inline static void drawShadow(float darkness = 0.5f) { glexDrawShadow(darkness); }

	#define PIf		3.1415926535897932384f

	/**
	 * Advanced drawing
	 *
	 */
	static void drawSky(float radius = 1000.0f, float y_offset = -100.0f, rgbColor base_color = rgbColor(218.0f/255.0f, 249.0f/255.0f, 252.0f/255.0f), rgbColor top_color = rgbColor(93.0f/255.0f, 115.0f/255.0f, 164.0f/255.0f), float x_res = 10, float y_res = 10);
	

	inline static void getFrameBuffer(unsigned char* data) { glReadPixels(0,0,render_width, render_height, GL_RGBA, GL_UNSIGNED_BYTE, data); }


	//texture managment
	#define MAX_TEXTURES			1000
	static Texture textures[MAX_TEXTURES];
	static int no_texture;
	static bool textures_enabled;

	static int createTexture(std::string file_name);
	static void setTexture(int t_index);
	static void enableTexturing(bool set_enable);

	static int render_width;
	static int render_height;
	//

};

#endif

