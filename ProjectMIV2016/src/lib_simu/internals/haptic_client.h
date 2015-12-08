
#ifndef HAPTIC_CLIENT_H
#define HAPTIC_CLIENT_H

#include "AllMath.h"

#include <string>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

class HapticClient
{
	
	private:

		WSADATA wsaData;
		SOCKET ConnectSocket;
		struct addrinfo *result,
						*ptr,
						hints;

		char recvbuf[DEFAULT_BUFLEN];
		int iResult;
	
		float pos[3];
		
		bool m_connected;
		Maths::Vector3 force;

	public:

		HapticClient();
		~HapticClient();

		void init();

		bool connect(std::string ip_addr);

		void getPosition(Maths::Vector3& pos);
		void setForce(Maths::Vector3& force);
		
		bool isConnected() { return m_connected; }
};

#endif