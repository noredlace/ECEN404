//simple udp client usingn winsockets from the web
#include <iostream>
#include <WS2tcpip.h>
#include <opencv2\/opencv.hpp>

// Include the Winsock library (lib) file
#pragma comment (lib, "ws2_32.lib")

using namespace std;
using namespace cv;

//sample code to test the camera
int attemptWebcam() {
		VideoCapture cap(1);
		if (!cap.isOpened()) {
			return -1;
		}
		
		while (1) {
			Mat frame;

			//capture the frame 
			cap >> frame;

			//if frame is empty we exit
			if (frame.empty()) {
				break;
			}

			//send frame to client

			//display the resulting frame
			imshow("Frame", frame);

			//esc to exit
			char c = (char)waitKey(25);
			if (c == 27) {
				break;
			}

		}

		//release capture object
		cap.release();
		destroyAllWindows();
		return 0;
	
}

//tester to show the camera display and exit the display
bool showCamera(Mat frame) {
	imshow("Frame", frame);

	//esc to exit
	char c = (char)waitKey(25);
	if (c == 27) {
		return true;
	}
	return false;
}

//takes a frame, encodes to jpeg, sends to server
void sendPackets(Mat frame, sockaddr_in server, SOCKET out) {
	//create stream to send packet
	const int updmax = 64000;
	vector<uchar> buff = vector<uchar>(updmax);
	vector<int> param = vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 80;//keep it under 64kb

				  //encode to jpg
	imencode(".jpg", frame, buff, param);
	cout << "client encoded file size jpg " << buff.size() << endl;

	//send packet of bits of encoded jpeg to server
	int testVid = sendto(out, (const char *)&buff[0], buff.size(), 0, (sockaddr*)&server, sizeof(server));
	if (testVid == SOCKET_ERROR)
	{
		cout << "That didn't work! " << WSAGetLastError() << endl;
	}
}



void main(int argc, char* argv[]) // We can pass in a command line option!! 
{
	//attemptWebcam();
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return;
	}

	WSADATA data;

	WORD version = MAKEWORD(2, 2);

	// Start WinSock
	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		// Not ok! Get out quickly
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	// Create a hint structure for the server
	sockaddr_in server;
	server.sin_family = AF_INET; // AF_INET = IPv4 addresses
	server.sin_port = htons(54000); // Little to big endian conversion
	inet_pton(AF_INET, "127.0.0.1", &server.sin_addr); // Convert from string to byte array

													   // Socket creation, note that the socket type is datagram
	SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

	// Write out to that socket
	while (1) {

		Mat frame;
		//capture the frame 
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		
		//display image in window
		//if (showCamera(frame)) break;

		//function that encodes and sends a packet
		sendPackets(frame, server, out);
	}

	// Close the socket
	closesocket(out);

	// Close down Winsock
	WSACleanup();
}