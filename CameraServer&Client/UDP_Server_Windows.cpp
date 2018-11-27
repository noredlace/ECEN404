//simple udp server grabbed form the web

#include <iostream>
#include <WS2tcpip.h>
#include <opencv2\opencv.hpp>



// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

// Saves us from typing std::cout << etc. etc. etc.
//using namespace std;

//function to read and construct packets to the video interface
bool showPackets(char* buf, int bufSize) {
	//wrap encoded buffer into opencv raw data to decode and display
	cv::Mat rawData = cv::Mat(1, bufSize, CV_8UC1, buf);
	cv::Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
	cv::imshow("Frame", frame);
	//esc to exit
	char c = (char)cv::waitKey(25);
	if (c == 27) {
		return true;
	}
	return false;

}

// Main entry point into the server
void main()
{
	WSADATA data;

	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		std::cout << "Can't start Winsock! " << wsOk;
		return;
	}


	// Create a socket, notice that it is a user datagram socket (UDP)
	SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

	// Create a server hint structure for the server
	sockaddr_in serverHint;
	//inet_pton(AF_INET, "10.236.17.189", &serverHint.sin_addr.S_un.S_addr);
	serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
	//std::cout << GetAddrInfoA << std::endl;
	serverHint.sin_family = AF_INET; // Address format is IPv4
	serverHint.sin_port = htons(54000); // Convert from little to big endian



	// Try and bind the socket to the IP and port
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR)
	{
		std::cout << "Can't bind socket! " << WSAGetLastError() << std::endl;
		return;
	}


	char buffer[INET_ADDRSTRLEN];
	char portBuf[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &(serverHint.sin_addr), buffer, INET_ADDRSTRLEN);
	printf("address:%s\n", buffer);

	inet_ntop(AF_INET, &(serverHint.sin_port), portBuf, INET_ADDRSTRLEN);
	printf("port:%s\n", portBuf);
	//end of setup

	sockaddr_in client; // Use to hold the client information (port / ip address)
	int clientLength = sizeof(client); // The size of the client information

	const int bufSize = 100000;
	char buf[bufSize];

	while (true)
	{

		ZeroMemory(&client, clientLength); // Clear the client structure
		ZeroMemory(buf, bufSize); // Clear the receive buffer

							   // Wait for message
		int bytesIn = recvfrom(in, buf, bufSize, 0, (sockaddr*)&client, &clientLength);
		if (bytesIn == SOCKET_ERROR)
		{
			std::cout << "Error receiving from client " << WSAGetLastError() << std::endl;
			continue;
		}

		// Display message and client info
		char clientIp[256]; // Create enough space to convert the address byte array
		ZeroMemory(clientIp, 256); // to string of characters

								   // Convert from byte array to chars
		inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);


		// Display the message / who sent it
		//std::cout << "Message recv from " << clientIp << " : " << buf << std::endl;
		
		//shows packets that are read in from buffer, exits loop if esc is pressed
		if (showPackets(buf, bufSize)) break;	
	}

	// Close socket
	closesocket(in);

	// Shutdown winsock
	WSACleanup();
}