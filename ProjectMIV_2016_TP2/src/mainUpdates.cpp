
#include "Leap.h"
#include "LeapListener.h"

//TODO

//Define Leap motion variables
Leap::Controller controller;
LeapListener listener;

//Forward definition for the new interaction methods
void leapMotionGestures();
void leapPinchGesture();

void Display() {

	//TODO
	//Draw the leap motion skeleton
	listener.Draw(controller);

}

void app_loop() 
{	
	//TODO
	
	//Check if the user has performed a gesture
	leapCheckSwipeGesture();
	leapCheckPinchGesture();

	//TODO
}

void leapCheckPinchGesture()
{
	if (!listener.isHandVisible()) return;

	Vector3 thumb = listener.getFingerTipPosition(0);
	Vector3 index = listener.getFingerTipPosition(1);

	float dist = thumb.distance(index);

	if (dist < 0.1f)
		std::cout << "Pinch detected!!! " << dist << std::endl;	
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
}


//Main Function
int main(int argc, char **argv) {

	//TODO
	
	//Leap motion initialization

	// Have the sample listener receive events from the controller
	controller.addListener(listener);
	
	//The leap motion have his own reference frame, provide a translation and scale 
	//to ensure a common frame of reference
	listener.setTranslationAndRotation(Vector3(0, 250, 150), 0.01f);
	
	//TODO

	// Remove the leap listener when done
	controller.removeListener(listener);

	return 0;
}