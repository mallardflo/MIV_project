#include <iostream>

#include "AllMath.h"

#include "GUI.h"
#include "Mesh.h"
#include "Simulator.h"
#include "HapticClient.h"

int last_mouse_x;
int last_mouse_y;

HapticClient haptic_client;
RigidSphere manipulator;
RigidSphere sphere1;
RigidSphere sphere2;
Simulator simulator;
Mesh mesh;
bool paused = false;

//Forward Declarations
void hapticButtonClicked();
void Display();
void app_loop();
void mouseButtonClicked(int button_id, int x, int y);
void mouseDragged(int x, int y) ;
void keyPressed(unsigned char key);

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
	simulator.setManipulator(&manipulator);

	//Fix particles inside of the spheres
	//simulator.fixParticlesinSphere(&sphere1);
	//simulator.fixParticlesinSphere(&sphere2);

	//Creates and initalizes the simulator which will update the mesh

	std::cout << "Initializing Haptic Device.... " << std::flush;

	//Haptic
	haptic_client.init();
	

	//Start the application loop. This function returns when the main window is closed
	GUI::startApp(app_loop, mouseButtonClicked, mouseDragged, keyPressed);
	
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
		//manipulator.setPosition(haptic_client.getPosition());
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
	haptic_client.setForce(retour);

	//Check the button status of the haptic device
	hapticButtonClicked();
	
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
	
}

