/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#include "LeapListener.h"
#include "LeapUtilGL.h"

using namespace Leap;

const std::string fingerNames[] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
const std::string boneNames[] = { "Metacarpal", "Proximal", "Middle", "Distal" };
const std::string stateNames[] = { "STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END" };

void LeapListener::onInit(const Controller& controller) {
	std::cout << "[Leap Motion] Initialized" << std::endl;
}

void LeapListener::onConnect(const Controller& controller) {
	std::cout << "[Leap Motion] Connected" << std::endl;
	
	//controller.enableGesture(Gesture::TYPE_CIRCLE);
	//controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
	//controller.enableGesture(Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void LeapListener::onDisconnect(const Controller& controller) {
	// Note: not dispatched when running in a debugger.
	std::cout << "[Leap Motion] Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller& controller) {
	std::cout << "[Leap Motion] Exited" << std::endl;
}

void LeapListener::Draw(const Controller& controller)
{
	//Draw the Leap hand
	const Leap::Frame frame = controller.frame();

	if (frame.hands().count() == 1)
	{
		const Hand hand = *(m_CurrentFrame.hands().begin());

		glMatrixMode(GL_MODELVIEW_MATRIX);
		glPushMatrix();

		glScalef(m_Scale, m_Scale, m_Scale);
		glTranslatef(-m_Translation.x, -m_Translation.y, -m_Translation.z);

		LeapUtilGL::drawSkeletonHand(hand);

		glPopMatrix();

	}
}

void LeapListener::onFrame(const Controller& controller) 
{
	m_CurrentFrame = controller.frame();

	m_HandVisible = m_CurrentFrame.hands().count() > 0;

	const GestureList gestures = m_CurrentFrame.gestures();
	for (int g = 0; g < gestures.count(); ++g)
	{
		if (gestures[g].state() == Leap::Gesture::State::STATE_STOP)
			m_Gestures.push_back(gestures[g]);
	}
}

Vector3 LeapListener::getFingerTipPosition(int fingerId)
{
	HandList hands = m_CurrentFrame.hands();

	if (hands.count() == 0) return Vector3();

	FingerList fingers = hands[0].fingers();
	
	if(fingerId < 0 || fingerId >= fingers.count()) return Vector3();
	
	const Finger& finger = fingers[fingerId];
    const Bone& bone = finger.bone(static_cast<Bone::Type>(Bone::TYPE_DISTAL));
	
	return transformVector(Vector3(bone.nextJoint().x,bone.nextJoint().y,bone.nextJoint().z));
}

bool LeapListener::isScreenTap(Vector3 &position, Vector3 &direction)
{
	// Get gestures
	for (auto it = m_Gestures.begin(); it != m_Gestures.end(); it++)
	{
		Gesture gesture = *it;

		if (gesture.state() == Leap::Gesture::State::STATE_STOP &&
		    gesture.type() == Gesture::TYPE_SCREEN_TAP)
		{
			ScreenTapGesture tap = gesture;

			direction = transformVector(Vector3(tap.direction().x,tap.direction().y,tap.direction().z));
			position = transformVector(Vector3(tap.position().x,tap.position().y,tap.position().z));

			m_Gestures.erase(it);
			return true;
		}

		if (gesture.state() == Leap::Gesture::State::STATE_STOP &&
			gesture.type() == Gesture::TYPE_KEY_TAP)
		{
			KeyTapGesture tap = gesture;

			direction = transformVector(Vector3(tap.direction().x, tap.direction().y, tap.direction().z));
			position = transformVector(Vector3(tap.position().x, tap.position().y, tap.position().z));

			m_Gestures.erase(it);
			return true;
		}
	}
	return false;
}

bool LeapListener::isSwipe(float &speed, Vector3 &direction, int &finger)
{
	// Get gestures
	for (auto it = m_Gestures.begin(); it != m_Gestures.end(); it++)
	{
		Gesture gesture = *it;

		if (gesture.state() == Leap::Gesture::State::STATE_STOP &&
		    gesture.type() == Gesture::TYPE_SWIPE)
		{			
			SwipeGesture swipe = gesture;
			
			direction = Vector3(swipe.direction().x,swipe.direction().y,swipe.direction().z); 
			speed = swipe.speed();

			direction = transformVector(direction);
			finger = -1;

			if (swipe.pointable().isFinger())
			{
				Finger f(swipe.pointable());
				finger = f.type();
			}

			m_Gestures.erase(it);
			return true;
		}
	}	
	return false;
}

bool LeapListener::isCircle(float &radius, bool &clockwise)
{
	for (auto it = m_Gestures.begin(); it != m_Gestures.end(); it++)
	{
		Gesture gesture = *it;

		if (gesture.state() == Leap::Gesture::State::STATE_STOP &&
		    gesture.type() == Gesture::TYPE_CIRCLE)
		{
			CircleGesture circle = gesture;

			if (circle.pointable().direction().angleTo(circle.normal()) <= PI / 2) {
				clockwise = true;
			}
			else {
				clockwise = false;
			}
			
			radius = circle.radius();
			m_Gestures.erase(it);
			return true;
		}
	}
	return false;
}

void LeapListener::onFocusGained(const Controller& controller) {
	std::cout << "[Leap Motion] Focus Gained" << std::endl;
}

void LeapListener::onFocusLost(const Controller& controller) {
	std::cout << "[Leap Motion] Focus Lost" << std::endl;
}

void LeapListener::onDeviceChange(const Controller& controller) {
	std::cout << "[Leap Motion] Device Changed" << std::endl;
	const DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "[Leap Motion] id: " << devices[i].toString() << std::endl;
		std::cout << "[Leap Motion]  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void LeapListener::onServiceConnect(const Controller& controller) {
	std::cout << "[Leap Motion] Service Connected" << std::endl;
}

void LeapListener::onServiceDisconnect(const Controller& controller) {
	std::cout << "[Leap Motion] Service Disconnected" << std::endl;
}


void LeapListener::setTranslationAndRotation(Vector3 t, float scale)
{
	m_Translation = t;
	m_Scale = scale;
}

Vector3 LeapListener::transformVector(Vector3 v)
{
	v = (v - m_Translation) * m_Scale;
	return v;
}
