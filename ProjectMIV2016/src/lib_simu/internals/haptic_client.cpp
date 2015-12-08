#include "haptic_client.h"

#include "StringExtension.h"

#include <iostream>
#include <string>
#include <cstdio>

HapticClient::HapticClient() : m_connected(false)
{

}

bool HapticClient::connect(std::string ip_addr)
{

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return false;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
	iResult = getaddrinfo(ip_addr.c_str(), DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return false;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return false;
        }

        // Connect to server.
		iResult = ::connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }

	std::cout << "connected to haptic server !!!" << std::endl;
	m_connected = true;
	return true;

}

void HapticClient::init()
{

}

void HapticClient::getPosition(Maths::Vector3& pos)
{

	if (!m_connected) {
		std::cout << "connect to server first !!" << std::endl;
		return;
	}

    // Send an initial buffer
	std::string getp;
	getp = "get_position;" + StringExtension::floatToString(force.x) + ";" + StringExtension::floatToString(force.y) + ";" + StringExtension::floatToString(force.z);
	iResult = send( ConnectSocket, getp.c_str(), getp.length() + 1, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

	iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	if ( iResult > 0 ) {
		std::string data(recvbuf);
		//treat input stream
		std::vector<std::string> cmds;
		StringExtension::stringSplit(cmds, data, ";");
		//treat command
		if (cmds[0].compare("position") == 0) {
			pos.x = StringExtension::stringToFloat(cmds[1]);
			pos.y = StringExtension::stringToFloat(cmds[2]);
			pos.z = StringExtension::stringToFloat(cmds[3]);
		}
	} else if ( iResult == 0 ) {
        printf("Connection closed !!\n");
	} else {
        printf("recv failed with error: %d\n", WSAGetLastError());
	}

}

void HapticClient::setForce(Maths::Vector3& force)
{   

	if (!m_connected) {
		std::cout << "connect to server first !!" << std::endl;
		return;
	}

    // Send an initial buffer
	this->force = force;

}

HapticClient::~HapticClient() {

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

}

int main2(int argc, char **argv) 
{
    
	HapticClient client;
	Maths::Vector3 p;
	
	client.init();
	
	if (!client.connect("localhost")) {
		getchar();
		return 0;
	}
	
	Sleep(5000);

	while (1) {
		client.getPosition(p);
		//std::cout << "position: " << p.x << " ; " <<  p.y << " ; " <<  p.z << " ; " << std::endl;
		client.setForce(Maths::Vector3(1000, 0, 0));
		//client.setForce(Maths::Vector3(-100, 0, 0));
	}
    
	return 0;


}