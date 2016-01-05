#include <iostream>
#include <cstring>

#include "Leap.h"
#include "AllMath.h"

using namespace Maths;

class LeapListener : public Leap::Listener {
public:
	virtual void onInit(const Leap::Controller&);
	virtual void onConnect(const Leap::Controller&);
	virtual void onDisconnect(const Leap::Controller&);
	virtual void onExit(const Leap::Controller&);
	virtual void onFrame(const Leap::Controller&);
	virtual void onFocusGained(const Leap::Controller&);
	virtual void onFocusLost(const Leap::Controller&);
	virtual void onDeviceChange(const Leap::Controller&);
	virtual void onServiceConnect(const Leap::Controller&);
	virtual void onServiceDisconnect(const Leap::Controller&);

	virtual void Draw(const Leap::Controller&);	

	bool isScreenTap(Vector3 &position, Vector3 &direction);
	bool isSwipe(float &speed, Vector3 &direction, int &finger);
	bool isCircle(float &radius, bool &clockwise);
	
	Vector3 getFingerTipPosition(int id);	

	//Transforms the vector from the leap coordinate system to the application
	Vector3 transformVector(Vector3 v);

	void setTranslationAndRotation(Vector3 pos, float scale);

	bool isHandVisible() { return m_HandVisible; }

private:
	Leap::Frame m_CurrentFrame;
	Vector3 m_Translation;
	float m_Scale;
	bool m_HandVisible;

	std::vector<Leap::Gesture> m_Gestures;
};