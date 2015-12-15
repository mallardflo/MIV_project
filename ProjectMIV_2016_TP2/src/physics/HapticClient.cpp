#include "HapticClient.h"

#include <iostream>
#include <string>
#include <cstdio>

#include <hdl/hdl.h>
#include <hdlu/hdlu.h>

// Continuous servo callback function
HDLServoOpExitCode ContactCB(void* pUserData)
{
    // Get pointer to haptics object
    HapticClient* haptics = static_cast< HapticClient* >( pUserData );

    // Get current state of haptic device
    hdlToolPosition(haptics->m_positionServo);
	hdlToolButtons(&(haptics->m_Buttons));
	hdlToolButton(&(haptics->m_buttonPressed));

	//Send the computed forces to the haptic device
	hdlSetToolForce(haptics->m_force);

    // Make sure to continue processing
    return HDL_SERVOOP_CONTINUE;
}

HapticClient::HapticClient() : m_Init(false)
{
	m_force[0]=m_force[1]=m_force[2]=0;
}

void HapticClient::init()
{
	HDLError error = HDL_NO_ERROR;

    // Passing "DEFAULT" or 0 initializes the default device based on the
    // [DEFAULT] section of HDAL.INI.   The names of other sections of HDAL.INI
    // could be passed instead, allowing run-time control of different devices
    // or the same device with different parameters.  See HDAL.INI for details.

	m_HapticHandle = hdlInitDevice(HDL_DEFAULT_DEVICE_ID);
	testHDLError("hdlInitDevice");

	if (m_HapticHandle == HDL_INVALID_HANDLE)
	{
		std::cerr << "Could not open device - Device Failure" << std::endl;
		return;
	}

	// Now that the device is fully initialized, start the servo thread.
	// Failing to do this will result in a non-funtional haptics application.
	hdlStart();
	testHDLError("hdlStart");
	
	// Set up callback function
    m_servoOp = hdlCreateServoOp(ContactCB, this, false);
    if (m_servoOp == HDL_INVALID_HANDLE)
    {
        std::cerr << "Invalid servo op handle -  Device Failure" << std::endl;
		return;
    }
    testHDLError("hdlCreateServoOp");

    // Make the device current.  All subsequent calls will
    // be directed towards the current device.
    hdlMakeCurrent(m_HapticHandle);
    testHDLError("hdlMakeCurrent");

    // Get the extents of the device workspace.
    // Used to create the mapping between device and application coordinates.
    // Returned dimensions in the array are minx, miny, minz, maxx, maxy, maxz
    //                                      left, bottom, far, right, top, near)
    // Right-handed coordinates:
    //   left-right is the x-axis, right is greater than left
    //   bottom-top is the y-axis, top is greater than bottom
    //   near-far is the z-axis, near is greater than far
    // workspace center is (0,0,0)
    hdlDeviceWorkspace(m_workspaceDims);
    testHDLError("hdlDeviceWorkspace");


    // Establish the transformation from device space to app space
    // To keep things simple, we will define the app space units as
    // inches, and set the workspace to approximate the physical
    // workspace of the Falcon.  That is, a 4" cube centered on the
    // origin.  Note the Z axis values; this has the effect of
    // moving the origin of world coordinates toward the base of the
    // unit.
    double gameWorkspace[] = {-2,-2,-2,2,2,3};
    bool useUniformScale = true;

    hdluGenerateHapticToAppWorkspaceTransform(m_workspaceDims,
                                              gameWorkspace,
                                              useUniformScale,
                                              m_transformMat);

    testHDLError("hdluGenerateHapticToAppWorkspaceTransform");
	
    m_Init = true;
}

bool HapticClient::isButtonPressed(int button)
{	
	//int m_buttonServo;
	return (m_Buttons & (1 << button)) != 0;
}

void HapticClient::testHDLError(const char* str)
{
	HDLError err = hdlGetError();
	if (err != HDL_NO_ERROR)
	{
		std::cout << "[HDAL ERROR]: " << str << std::endl;
	}
}

Maths::Vector3 HapticClient::getPosition()
{
	// Convert from device coordinates to application coordinates.
	double pos[3];
    vecMultMatrix(m_positionServo, m_transformMat, pos);
	
	return Maths::Vector3(pos[0],pos[1],pos[2]) * 2.54f;
}

void HapticClient::setForce(Maths::Vector3& f)
{   
    //Update forces
	m_force[0] = f[0];
	m_force[1] = f[1];
	m_force[2] = f[2];
}

HapticClient::~HapticClient() 
{
    if (m_servoOp != HDL_INVALID_HANDLE)
    {
        hdlDestroyServoOp(m_servoOp);
        m_servoOp = HDL_INVALID_HANDLE;
    }
    hdlStop();
	if (m_HapticHandle != HDL_INVALID_HANDLE)
    {
        hdlUninitDevice(m_HapticHandle);
        m_HapticHandle = HDL_INVALID_HANDLE;
    }
    m_Init = false;
}

// A utility function to handle matrix multiplication.  A production application
// would have a full vector/matrix math library at its disposal, but this is a
// simplified example.
void HapticClient::vecMultMatrix(double srcVec[3], double mat[16], double dstVec[3])
{
    dstVec[0] = mat[0] * srcVec[0] 
        + mat[4] * srcVec[1]
        + mat[8] * srcVec[2]
        + mat[12];
    
    dstVec[1] = mat[1] * srcVec[0]
        + mat[5] * srcVec[1]
        + mat[9] * srcVec[2]
        + mat[13];

    dstVec[2] = mat[2] * srcVec[0]
        + mat[6] * srcVec[1]
        + mat[10] * srcVec[2]
        + mat[14];
}
