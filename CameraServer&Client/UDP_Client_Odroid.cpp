#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/opencv_modules.hpp"
//#include "udpclient.h"

#define CV_IMWRITE_JPEG_QUALITY 1

using namespace std;
using namespace cv;

//takes a frame, encodes to jpeg, sends to server
void sendPackets(Mat frame, sockaddr_in server, int out) {
	//create stream to send packet
	const int updmax = 64000;
	vector<uchar> buff = vector<uchar>(updmax);
	vector<int> param = vector<int>(2);
	param[0] = CV_IMWRITE_JPEG_QUALITY;
	param[1] = 20;//keep it under 64kb

				  //encode to jpg
	imencode(".jpg", frame, buff, param);
	cout << "client encoded file size jpg " << buff.size() << endl;

	//send packet of bits of encoded jpeg to server
	int testVid = sendto(out, (const char *)&buff[0], buff.size(), 0, (sockaddr*)&server, sizeof(server));
	if (testVid == -1)
	{
		cout << "That didn't work! " << endl;
	}
}

bool udpSend(const char *msg){
    sockaddr_in servaddr;
    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0){
        perror("cannot open socket");
        return false;
    }
    
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = inet_addr("10.236.17.189");
   
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr); // Convert from string to byte array
    servaddr.sin_port = htons(54000);
    if (sendto(fd, msg, strlen(msg)+1, 0, // +1 to include terminator
               (sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        perror("cannot send message");
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

int main(){
 VideoCapture cap(0);
	if (!cap.isOpened()) {
		return 0;
	}

    sockaddr_in servaddr;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = inet_addr("10.236.17.189");
    inet_pton(AF_INET, "10.236.17.189", &servaddr.sin_addr); // Convert from string to byte array
    servaddr.sin_port = htons(54000);

    int fd = socket(AF_INET,SOCK_DGRAM,0);
    if(fd<0){
        perror("cannot open socket");
        return false;
    }

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
		sendPackets(frame, servaddr, fd);

/*
        imshow("frame",frame);


			//esc to exit
			char c = (char)waitKey(25);
			if (c == 27) {
				break;
			}
*/
	}


		//release capture object
		cap.release();
		destroyAllWindows();
    return 0;
}
