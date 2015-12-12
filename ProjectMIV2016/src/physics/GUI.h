
#ifndef GUI_H
#define GUI_H

#include "Display.h"
#include "glut.h"

#include <iostream>

#define MOUSE_LEFT_BUTTON					GLUT_LEFT_BUTTON
#define MOUSE_MIDDLE_BUTTON					GLUT_MIDDLE_BUTTON
#define MOUSE_RIGHT_BUTTON					GLUT_RIGHT_BUTTON

//forward class definitions
struct Mesh;
class UniformGrid;

//main class	
class GUI {

private:

	static int window_size_x;
	static int window_size_y;

	//fuction pointers
	static void (*mouseClickedCallback)(int, int, int);
	static void (*mouseDragCallback)(int, int);
	static void (*keyPressedCallback)(unsigned char);

	//camera spherical position
	static float cam_angle_x;
	static float cam_angle_y;
	static float cam_zoom;
	static Maths::Vector3 cam_target;

	static float zoom_precision;
	static float angle_precision;

	static int last_mouse_x;
	static int last_mouse_y;

	static bool mouse_button_state[3];
		
	static void resize3DDisplay(int new_width, int new_height);

	static void mouseClickEvent(int button, int release, int x, int y);

	static void mouseDragEvent(int x, int y);

	static void keyPressedEvent(unsigned char key_id, int x, int y);

	static void idle();


public:

	static void init(int argc, char **argv);

	static void createWindow(std::string title, int size_x, int size_y);

	static void startApp(void (*loop_function)(void), void (*mouseClickedCallback)(int, int, int), void (*mouseDragCallback)(int, int), void (*keyPressedCallback)(unsigned char));

	//Display functions
	static void begin3DScene();

	static void end3DScene();

	static void beginTransformation(Maths::Vector3 v = Maths::Vector3::ZERO, Maths::Quaternion q = Maths::Quaternion());
	static void endTransformation();

	inline static void drawPoint(Maths::Vector3& pos, float size = 5.0f) { Display::setPointSize(size); Display::drawPoint(pos); }
	inline static void drawLine(Maths::Vector3& start, Maths::Vector3& end) { Display::drawLine(start, end); }
	inline static void drawBox(Maths::Vector3& size) { Display::solidBox(size.x, size.y, size.z); }
	inline static void drawSphere(float radius) { Display::solidSphere(radius*2.0f); }

	static void drawMesh(Mesh& mesh, bool wire_frame = true);
	static void drawGrid(UniformGrid& grid);

	inline static void setDrawColor(float r, float g, float b) { Display::setDrawColor(r, g, b); }

	static bool getMouseButtonState(int button_id) { return mouse_button_state[button_id]; }

};

#endif