#include <iostream>

#include "AllMath.h"

#include "GUI.h"
#include "Mesh.h"
#include "Simulator.h"
#include "HapticClient.h"

#include "Leap.h"
#include "LeapListener.h"

#define HAPTIC 1
#define MOUSE 0

int last_mouse_x;
int last_mouse_y;

float last_haptic_x;
float last_haptic_y;
float last_haptic_z;



int scale_factor = 1;

HapticClient haptic_client;
RigidSphere manipulator;
RigidSphere scalpel;
RigidSphere sphere1;
RigidSphere sphere2;
Simulator simulator;
Mesh mesh;
bool paused = false;
Vector3 thumbPos;
int move_mode = MOUSE;
bool cut_mode = false;
bool stop_manip = false;

//Forward Declarations
void hapticButtonClicked();
void Display();
void app_loop();
void mouseButtonClicked(int button_id, int x, int y);
void mouseDragged(int x, int y) ;
void keyPressed(unsigned char key);

//Define Leap motion variables
Leap::Controller controller;
LeapListener listener;

//Forward definition for the new interaction methods
void leapCheckSwipeGesture();
void leapCheckPinchGesture();

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
	mesh.loadFromFile("./media/meshes/liver1.mshdat");

	//Define the shape and the position of the manipulator
	manipulator.setRadius(1.0f);
	manipulator.setPosition(Maths::Vector3(0,-0.25,0));

	//Define sizes and positions of the fixing spheres
	/*sphere1.setPosition(Maths::Vector3(-0.5,3.5,0));
	sphere2.setPosition(Maths::Vector3(3,4.2,0));
	sphere1.setRadius(0.5);
	sphere2.setRadius(1);*/

	//Creates and initalizes the simulator which will update the mesh
	simulator.setMesh(&mesh);
	//simulator.dropMesh(2.5);
	simulator.setManipulator(&manipulator);
	simulator.restoreFixedParticles();

	//Fix particles inside of the spheres
	//simulator.fixParticlesinSphere(&sphere1);
	//simulator.fixParticlesinSphere(&sphere2);

	//Creates and initalizes the simulator which will update the mesh

	std::cout << "Initializing Haptic Device.... " << std::flush;

	//Haptic
	haptic_client.init();

	//Leap motion initialization

	// Have the sample listener receive events from the controller
	controller.addListener(listener);

	//The leap motion have his own reference frame, provide a translation and scale 
	//to ensure a common frame of reference
	listener.setTranslationAndRotation(Vector3(0, 250, 150), 0.01f);

	//Create and initialize the scalpel to cut flesh
	scalpel.setRadius(0.1f);
	simulator.setScalpel(&scalpel);

	//Start the application loop. This function returns when the main window is closed
	GUI::startApp(app_loop, mouseButtonClicked, mouseDragged, keyPressed);


	// Remove the leap listener when done
	controller.removeListener(listener);
	

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Simulation & Display

//main loop of the application (automatically called by the GUI class) coompsed of 2 parts : move the world, and display the world
void app_loop() 
{	
	if (!paused){
		//Update the simulation
		simulator.Update();

		if (move_mode == HAPTIC && !stop_manip){

			manipulator.Move(last_haptic_x - haptic_client.getPosition().x / scale_factor,
				last_haptic_y - haptic_client.getPosition().y / scale_factor,
				last_haptic_z - haptic_client.getPosition().z / scale_factor);
		}
		last_haptic_x = haptic_client.getPosition()[0];
		last_haptic_y = haptic_client.getPosition()[1];
		last_haptic_z = haptic_client.getPosition()[2];
	}

	Maths::Vector3 retour = Maths::Vector3::ZERO;
	//Update haptic simulation here!!
	for (unsigned int p = 0; p < mesh.particles.size(); p++)
	{
		if (mesh.particles[p].pos.distance(manipulator.getPosition()) < manipulator.getRadius())
		{
			retour += mesh.particles[p].pos - manipulator.getPosition();
		}
	}

	retour = -retour;
	haptic_client.setForce(retour/4);


	//Check the button status of the haptic device
	hapticButtonClicked();
	
	//Gestion des boutons haptiques
	if (haptic_client.isButtonPressed(0)){
		paused = true;
	}
	if (haptic_client.isButtonPressed(1)){
		paused = false;
	}
	if (haptic_client.isButtonPressed(2)){
		stop_manip = true;
	}
	if (haptic_client.isButtonPressed(3)){
		stop_manip = false;
	}

	if (cut_mode){
		// put the scalpel at the tip of the index
		scalpel.setPosition(listener.getFingerTipPosition(1));
		// check if scalpel is cutting flesh
		simulator.checkCut();
	}

	// remove orphaned particles without neighbors from mesh
	simulator.checkOrphans();

	//Check if the user has performed a gesture
	leapCheckSwipeGesture();
	leapCheckPinchGesture();
	
	//Diplay the 3D scene
	Display();
}

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

	//Draw the scalpel 
	if (cut_mode)
	{
		scalpel.Draw();
	}

	//Draw the leap motion skeleton
	listener.Draw(controller);
	
	//terminate the rendering, and display the drawn scene on the main window
	GUI::end3DScene();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Interaction

//Function called by the app_loop which tests if any of the haptic buttons  
//from the haptic device are pressed
void hapticButtonClicked()
{
	if(haptic_client.isButtonPressed())
	{
		std::cout << "Buttons Pressed:";

		for(int i = 0;i<4;i++)
		{
			if(haptic_client.isButtonPressed(i)) std::cout << " " << i;
		}

		std::cout << std::endl;
	}
}


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
	
	if (key == 'p'){
		paused = !paused;
	}
	if (key == 'f'){
		simulator.fixParticles();
	}
	if (key == 's'){
		simulator.saveFixedParticles();
	}
	if (key == 'm'){
		if (move_mode == HAPTIC)
		{
			move_mode = MOUSE;
		}
		else
		{
			move_mode = HAPTIC;
		}
	}
	if (key == 'c'){
		cut_mode = !cut_mode;
		std::cout << "Scalpel enabled: " << cut_mode << std::endl;
	}
	if (key == 'i'){
		scale_factor += 0.2;	
	}
	if (key == 'd'){
		scale_factor -= 0.2;
	}
}

void leapCheckPinchGesture()
{
	if (!listener.isHandVisible()) return;

	Vector3 thumb = listener.getFingerTipPosition(0);
	Vector3 index = listener.getFingerTipPosition(1);
	Vector3 middle = listener.getFingerTipPosition(2);
	
	float dist = thumb.distance(index);
	float dist2 = thumb.distance(middle);

	if (dist < 0.3f)
	{
		std::cout << "Pinch between thumb and index detected!!! " << dist << std::endl;
		Vector3 diff = thumbPos - thumb;
		thumbPos = thumb;
		simulator.translateMesh(diff);
	}
	else if (dist2 < 0.3f)
	{
		std::cout << "Pinch between thumb and middle detected!!! " << dist << std::endl;
		simulator.rotateMesh(thumbPos, thumb);
		thumbPos = thumb;
	}
	else
	{
		thumbPos = thumb;
	}
}

void leapCheckSwipeGesture()
{
	float speed;
	Vector3 direction;
	int finger;

	if (listener.isSwipe(speed, direction, finger))
	{
		std::cout << "[LeapMotion]" << " Swipe gesture(" << finger << ") : " << speed << "," << direction << std::endl;
	}
	if (move_mode == HAPTIC)
	{
		move_mode = MOUSE;
	}
	else
	{
		move_mode = HAPTIC;
	}
}