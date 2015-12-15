
#ifndef HAPTIC_CLIENT_H
#define HAPTIC_CLIENT_H

#include "AllMath.h"

#include <string>

#include <hdl/hdl.h>
#include <hdlu/hdlu.h>

class HapticClient
{	
private:

	// Handle to device
	HDLDeviceHandle m_HapticHandle;

	// Handle to Contact Callback 
	HDLServoOpExitCode m_servoOp;	

	// Device workspace dimensions
	double m_workspaceDims[6];

	// Transformation from Device coordinates to Application coordinates
	double m_transformMat[16];

	//Initialization flags
	bool m_Init;
	
public:

	HapticClient();
	~HapticClient();

	//Initializes the haptic device. It has to be called once!
	void init();

	//Returns the position in m of the haptic device
	Maths::Vector3 getPosition();

	//Set the force to be applied by the haptic device in newtons
	void setForce(Maths::Vector3& force);

	//Retruns true if the ith-button is pressed
	bool isButtonPressed(int button);

	//Returns true if any button is pressed
	inline bool isButtonPressed() {return m_buttonPressed;}

	// Variables used only by servo thread
	double m_positionServo[3];	
	int   m_Buttons;
	bool   m_buttonPressed;

	//Current force
	double m_force[3];

private:

	void testHDLError(const char* str);		
	void vecMultMatrix(double srcVec[3], double mat[16], double dstVec[3]);
};

#endif