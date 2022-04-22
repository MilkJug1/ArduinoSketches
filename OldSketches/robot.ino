/*
  ##################################
  #
  # Project for the High School
  #
  # Made by MilkJug
  #
  # Most of the Code is by Chaos Buster
  #
  # Last Modified on 5/10/21 at 9:50 AM
  #
  ##################################
  Special Thanks to Fundroid 3000 that created the app that drives the program
  Find the App in the playstore --> Arduino Bluetooth controller  by Funduino
  This sketch uses the default setting from the App.
  The arrow buttons are direction, the shapes buttons are speeds
  The Function buttons determine the speed. Uppercase"A" is slowest and "H" is fastest
  Speed determines the magnitude of the motors 0 = off, 255 = full speed
  The left and right keys use an offset "offset1" to slow one of the motors so the robot veers
  in the direction of the slower motor.
  As speed is changed, so is the offset. The default is set as a percentage of speed.
  It is set at one third of the speed setting. This is easily changed
  This is a major area for experimentation.
*/
#include <Wire.h> // I2C communications library
#include <Adafruit_SSD1306.h> // OLED library
#include <Adafruit_MotorShield.h>  // designed for the "V2" Adaifruit (and most knockoff) shields
Adafruit_SSD1306 display(128, 64, &Wire, 4);
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // defines motors

Adafruit_DCMotor *myMotor = AFMS.getMotor(1);     // There are 4 (1,2,3,4) motor ports
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(4);   // Adjust these lines for motor names and ports

//String strength = ""; // String variables get the data from the Bluetooth App
//String angle = "";
//String button = "";
//int intStrength;  //0 to 100
//int intButton;    //1,2,3,4
int speed = 0;    // variable that holds the converted strength to 0 to 255 for motor speed
int offset = 10;   // differential numbers for slowing a motor in a turn
int PivotSpeed = 60; // this sets how quickly the robot pivots right or left
float BatteryVoltage;  // Variable to hold battery condition
float RawValue;
char data; //variable to store the data which is being read
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

  if (data == '4') {
    PivotRight(speed); // pivot inplace to the right
  }
  //if(data == '3'){setSpeed = 75;}  // TBD
  if (data == '1') {
    PivotLeft(speed); // pivot inplace to the left
  }

} // This is the end of the void Loop

// starting here is all of the functions for various directions
void Forward(int speed)      // Forward, Backward, TurnLeft, TurnRight
{ 
  SetMotor(FORWARD);     // when the function is called
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

void Display(int x, int y, char m)
{
  display.setCursor(int x, int y); // Set the cursor position on the oled, then we print the character that is chosen.
  display.print(char y);
}

void UpdateDisplay() {  // This function is where we configure the display
  RawValue = analogRead(A10);  // Convert analog reading to Voltage
  BatteryVoltage = RawValue / 84; // This is a constant based on the resistor voltage divider
  display.clearDisplay();  //This command clears the buffer
  Display(0, 0, "Bat:");
  display.setCursor(50, 0);  
  display.print(BatteryVoltage);  
  display.setCursor(100, 0);   
  display.setCursor(0, 16); // Second line moves down 16 pixels
  display.print("Data "); //prints to OLED
  display.setCursor(90, 16); //moves over om second line
  display.print(data);       // prints the data value from the Bluetooth module
  display.setCursor(0, 32); // third line
  display.print("Offset ");
  display.setCursor(90, 32); // moves over and down
  display.println(offset); // prints direction
  display.setCursor(0, 50); // fourth line
  display.print("Speed "); 
  display.setCursor(80, 50); // moves over and down
  display.println(speed); // prints direction
  display.display(); // writes all the above changes to the screen
}

void OLED(String phrase, int size, int x, int y) { // This is a FUNCTION. It takes the information and displays it at the size we choose and the place we choose
  display.clearDisplay();  //This command clears the buffer
  display.setTextSize(size); // set size 0 to 4
  display.setCursor(x, y); // The first number 0-128 pixels horiontal. The second is 0-64 pixels vertical
  display.print(phrase); //This writes the text in quotes to the to the buffer
  display.display(); // writes the buffer to the screen
}
