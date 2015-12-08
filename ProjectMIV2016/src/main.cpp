#include <iostream>

#include "AllMath.h"

#include "GUI.h"
#include "Mesh.h"
#include "Simulator.h"
#include "haptic_client.h"

int last_mouse_x;
int last_mouse_y;

HapticClient haptic_client;
RigidSphere manipulator;
Simulator simulator;
Mesh mesh;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Simulation & Display

//draw the world in 3D using the display method of GUI
void Display() {

	//prepare for 3D rendering (the drawing primitives are written on an offscreen buffer)
	GUI::begin3DScene();

	//Defines the color used to draw the mesh
	GUI::setDrawColor(1, 1, 1);

	//draw the body of the scene
	GUI::drawMesh(mesh);

	//Draw the manipulator
	manipulator.Draw();
	
	//terminate the rendering, and display the drawn scene on the main window
	GUI::end3DScene();
}

//main loop of the application (automatically called by the GUI class) coompsed of 2 parts : move the world, and display the world
void app_loop() 
{	
	simulator.Update();
	Display();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Interaction

//function called when the a button has been clicked on the mouse
//x and y represent the current position of the cursor on the window
void mouseButtonClicked(int button_id, int x, int y) 
{
	if (button_id == MOUSE_LEFT_BUTTON) std::cout << "clicked : LEFT" << " - " << " - " << x << " - " << y << std::endl;
	if (button_id == MOUSE_MIDDLE_BUTTON) std::cout << "clicked : MIDDLE" << " - " << " - " << x << " - " << y << std::endl;
	if (button_id == MOUSE_RIGHT_BUTTON) std::cout << "clicked : RIGHT" << " - " << " - " << x << " - " << y << std::endl;

	if (button_id == MOUSE_LEFT_BUTTON) {
		last_mouse_x = x;
		last_mouse_y = y;		
	}
}

//called when the mouse is moved over the window while a button is pressed
//x and y represent the current position of the cursor on the window
//you can call GUI::getMousButtonState(int) to check the state of the buttons
void mouseDragged(int x, int y) 
{
	if (GUI::getMouseButtonState(MOUSE_LEFT_BUTTON)) {
		std::cout << "MouseDrag - left button" << std::endl;

		manipulator.Move( -(last_mouse_x - x)*0.015f, 0.0f, -(last_mouse_y - y)*0.015f);
	}

	last_mouse_x = x;
	last_mouse_y = y;
}

//called when a key of the keyboard has been pressed
void keyPressed(unsigned char key) {

	std::cout << "key pressed: " << key << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Main Function

int main(int argc, char **argv) {

	//Initialize main global variables
	GUI::init(argc, argv);

	//Create the main window of the application. This function must be called only once !
	GUI::createWindow("MIV Project", 800, 800);
	
	//Load the mesh object
	mesh.loadFromFile("./media/meshes/cloth1.obj");

	//Define the shape and the position of the manipulator
	manipulator.setRadius(1.0f);
	manipulator.setPosition(Maths::Vector3(0,-0.25,0));

	//Creates and initalizes the simulator which will update the mesh
	simulator.setMesh(&mesh);
	simulator.setManipulator(&manipulator);
		
	//Start the application loop. This function returns when the main window is closed
	GUI::startApp(app_loop, mouseButtonClicked, mouseDragged, keyPressed);
	
	return 0;
}