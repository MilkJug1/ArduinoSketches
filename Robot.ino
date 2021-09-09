#include <Wire.h>
#include <Adafruit_SSD1306.h> // OLED library
#include <Adafruit_MotorShield.h>  // designed for the "V2" Adaifruit (and most knockoff) shields


Adafruit_SSD1306 display(128, 64, &Wire, 4);
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // defines motors

Adafruit_DCMotor *myMotor = AFMS.getMotor(1);     // There are 4 (1,2,3,4) motor ports
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(4);   // Adjust these lines for motor names and ports


int speed;
int offset;
int PivotSpeed = 60;
float RawValue; 
float BatteryVoltage;
char data;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// this identifies the oled and it's I2C communications address
  display.display();// This displays the default memory that is Adafruit's logo
  delay(2000); // Pause for 2 seconds for the logo to be displayed
  display.clearDisplay();// Clear the buffer
  display.setTextSize(2); // set size 0 to 4
  display.setTextColor(WHITE, BLACK); // first color is the text, the second is the background
  Serial.begin(9600);
  Serial1.begin(9600); // Default communication rate of the Bluetooth module
  AFMS.begin(); // Invokes the library functions
}

void loop() {
  if (Serial1.available() > 0) {
    data = Serial1.read(); delay(2); // this line gets are data from Bluetooth
    Serial.print ("Data  "); Serial.println (data);
  } // This line prints it to serial monitor on computer
  display.setTextSize(2); // set size 0 to 4
  UpdateDisplay();  // this line runs the function to print our stuff to the OLED

  if (data == 'o' || data == '0') {
    Stop(); 
  }
  if (data == 'u') {
    Forward(speed);  // robot moves forward at speed determined by shape buttons 90 is forward
    delay(2);
  }
  if (data == 'd') {
    Backward(speed);  //robot moves backward at speed determined by shape buttons
    delay(2);
  }
  if (data == 'l') {
    TurnLeft(speed);  // robot veers left
    delay(2);
  }
  if (data == 'r') {
    TurnRight(speed);  //robot veers right
    delay(2);
  }

  switch (data)   // we check if the app sends a character that corrisponses to a letter here to increase the speed
  {
    case 'A':
      speed = 50;
      break;

    case 'B':
      speed = 75;
      break;

    case 'C':
      speed = 100;
      break;

    case 'D':
      speed = 150;
      break;

    case 'E':
      speed = 175;
      break;

    case 'F':
      speed = 200;
      break;

    case 'G':
      speed = 225;
      break;

    case 'H':
      speed = 255;
      break;

    case 'M':
      speed = 75;
      break;

    case 'N':
      speed = 150;
      break;

    default:
      speed = 0;
      break;
  }
  offset = (speed / 2); // this makes the offset for turning one third
  if (data == '2') {
    Dance1(speed);

  }   

  void SetMotor(int speed)    // instead of having the same 2 lines be repeated, we make a fuction where it sets both automatically with a paramater of the direction like FORWARD
  {
    myMotor->setSpeed(speed); // we set the speed for the 1st motor
    myMotor->run();           // -and then run it with the direction as a parameter.
  }

  void SetMotor1(int speed)
  {
    myMotor1->setSpeed(speed);  // Set the speed for the 2nd motor.
    myMotor1->run()             // run the motor with the direction specified 
  }

  if (data == '4')
  {
    PivotRight(speed);
  }

  if (data == '1')
  {
    PivotRight(speed);
  }
}

void Foward(int speed)
{
  SetMotor(FORWARD);
}

void Backward(int speed) {
  myMotor->setSpeed(speed);
  myMotor->run(BACKWARD);       //Make the robot roll forward
  myMotor1->setSpeed(speed);   // speed values are passed to the function
  myMotor1->run(BACKWARD);    // when the function is called
}
void TurnLeft(int speed) {
  myMotor->setSpeed(offset);
  myMotor->run(FORWARD);       //Make the robot roll forward
  myMotor1->setSpeed(speed);   // speed values are passed to the function
  myMotor1->run(FORWARD);     // when the function is called
}

void TurnRight(int speed) {
  myMotor->setSpeed(speed);
  myMotor->run(FORWARD);                //Make the robot roll forward
  myMotor1->setSpeed(offset); // speed values are passed to the function
  myMotor1->run(FORWARD);            // when the function is called
}
void PivotLeft(int PivotSpeed) {
  myMotor->setSpeed(PivotSpeed);
  myMotor->run(BACKWARD);       //Make the robot roll forward
  myMotor1->setSpeed(PivotSpeed);   // speed values are passed to the function
  myMotor1->run(FORWARD);    // when the function is called
}
void PivotRight(int PivotSpeed) {
  myMotor->setSpeed(PivotSpeed);
  myMotor->run(FORWARD);       //Make the robot roll forward
  myMotor1->setSpeed(PivotSpeed);   // speed values are passed to the function
  myMotor1->run(BACKWARD);    // when the function is called
}
void Stop() {           // This is the stop function
  //speed = 0;
  //i = 0;
  myMotor->setSpeed(speed);
  myMotor->run(RELEASE);
  myMotor1->setSpeed(speed);  // speed values are passed to the function
  myMotor1->run(RELEASE);     // when the function is called
}