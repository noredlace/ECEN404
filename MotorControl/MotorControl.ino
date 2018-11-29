#include <Stepper.h>
#include <Servo.h>

//200 steps * 8 microsteps = 1600
//declare other potential variables for moving motors
const int baudRate = 19200;
const int stepsPerRevolution = 1600;  // change this to fit the number of steps per revolution
const double oneDegree = stepsPerRevolution / 360;
const double angleMove = stepsPerRevolution / 16;
const int stepSpeed = 1000;

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;
bool dataToRead = false;

char messageFromPC[32] = {0};
int int1 = 0;
int int2 = 0;
int int3 = 0;
int int4 = 0;

// initialize the stepper library on pins 8 through 11:
Stepper myThree(stepsPerRevolution, 4, 5, 6, 7);
Stepper myTwo(stepsPerRevolution, 28, 26, 24, 22);
Stepper myOne(stepsPerRevolution,9,10,11,12);
Servo myservo;

void setup() {
  myThree.setSpeed(stepSpeed);
  myTwo.setSpeed(stepSpeed);
  myOne.setSpeed(stepSpeed);

  //move wrist pitch to default position
  myThree.step(30000);
  myThree.step(4000);

  //attach servo motor
  myservo.attach(8);

  // initialize the serial port:
  randomSeed(analogRead(0));
  //Serial.begin(9600);

  //serial baud rate from odroid
  Serial.begin(baudRate);
  Serial1.begin(baudRate);
}

//read characters until terminating char into array
void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (Serial1.available() > 0 && newData == false) {
    dataToRead = true;
    rc = Serial1.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

//debug print statement
void showNewData() {
  if (newData == true) {
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    newData = false;

  }
}

int oldServoVal = 0;

void loop() {
  //read in values and create array
  recvWithEndMarker();
  //showNewData();

  if (newData == true) {

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(receivedChars, " ");
    strcpy(messageFromPC, strtokIndx); // copy it to messageFromPC

    //get wrist pitch
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    int1 = atoi(strtokIndx);     // convert this part to an integer

    //get finger pinch
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    int2 = atoi(strtokIndx);     // convert this part to an integer

   //get wrist rotation
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    int3 = atoi(strtokIndx);     // convert this part to an integer

      //get elbow rotation
    strtokIndx = strtok(NULL, " "); // this continues where the previous call left off
    int4 = atoi(strtokIndx);     // convert this part to an integer

    //check if message is valid, move motors accordingly
    if (messageFromPC[0] == 'L') {
      myThree.step(int1);
      myOne.step(int3);
      myTwo.step(int4);

      //write to servo motor if values have changed. could be incorrect and causing issues
      if (oldServoVal != int2) {
        myservo.write(int2);
        oldServoVal = int2;
      }

    }
    newData = false;
  }
}


