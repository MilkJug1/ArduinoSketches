#include <Arduino.h> // this is sketches are made, all .ino files are just a .cpp(or .cc) with an hidden
// include for the Arduino specific things.
/*
  This is heavily modified version of Chaosbuster's sketch for motor control.
  The new features are as listed: Better and probably faster checking for getting data from bluetooth module
  Entire Rewrite of each motor type, and made it simpler to type out.
  Full I2C display, with both a 128x64 display mode along with a 16x2 display mode.
  Includes support for any bluetooth app on the app store.
  Built with PlatformIO on Linux inside of Vim/Visual Studio Code.
  CrabWalk function being a thing for onmiwheels.
*/

#include <Wire.h>                 // I2C communications library
#include <Adafruit_SSD1306.h>     // OLED library
#include <Adafruit_MotorShield.h> // designed for the "V2" Adaifruit (and most knockoff) shields
#include <LiquidCrystal_I2C.h>    // For the 16x2 display.
#include "Char.h"                 //TODO: Fix issue of this not working.

Adafruit_SSD1306 display(128, 64, &Wire, 4);        // Define the 128x64 OLED display that we have
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); // defines motors
LiquidCrystal_I2C lcd(0x27, 16, 2);                 // Define the 16x2 LCD I2C display that I can use for debugging.

Adafruit_DCMotor *BackR = AFMS.getMotor(4); // The Motor on the right from looking at in from the behind.
Adafruit_DCMotor *BackL = AFMS.getMotor(3);
Adafruit_DCMotor *FrontR = AFMS.getMotor(2);
Adafruit_DCMotor *FrontL = AFMS.getMotor(1);

String strength = ""; // String variables get the data from the Bluetooth App
String angle = "";
String button = "";
int intStrength;      // 0 to 100
int intButton;        // 1,2,3,4
int speed = 0;        // variable that holds the converted strength to 0 to 255 for motor speed
int offset = 30;      // differential numbers for slowing a motor in a turn
int PivotSpeed = 60;  // this sets how quickly the robot pivots right or left
float BatteryVoltage; // Variable to hold battery condition
float RawValue;
byte BatteryCal = 120; // Change to 120 if Voltage is measuring too high
char data;             // variable to store the data which is being read

byte RArrow[8] = {
  B00000,
  B00000,
  B00100,
  B11110,
  B11111,
  B11110,
  B00100,
  B00000
};

byte LArrow[8] = {
  B00000,
  B00000,
  B00100,
  B01111,
  B11111,
  B01111,
  B00100,
  B00000
};

byte UArrow[8] = {
  B00100,
  B01110,
  B01110,
  B11111,
  B01110,
  B01110,
  B01110,
  B01110
};

byte DArrow[8] = {
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111,
  B01110,
  B00100
};


void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // this identifies the oled and it's I2C communications address
  display.display();                         // This displays the default memory that is Adafruit's logo
  delay(2000);                               // Pause for 2 seconds for the logo to be displayed
  display.clearDisplay();                    // Clear the buffer
  display.setTextSize(2);                    // set size 0 to 4
  display.setTextColor(WHITE, BLACK);        // first color is the text, the second is the background
  Serial.begin(9600);
  Serial1.begin(9600); // Default communication rate of the Bluetooth module
  AFMS.begin();        // Invokes the library functions
  Serial.print("IF YOU CAN SEE THIS, MAKE SURE TO CHANGE THE MOTOR NUMBER PLEASE!\n");
  lcd.init();      // in order to correctly use the display, we have init the display
  lcd.backlight(); // and then send a signal to use the backlight in order for it to turn on.
  lcd.createChar(0, RArrow);
  lcd.createChar(1, LArrow);
  lcd.createChar(2, UArrow);
  lcd.createChar(3, DArrow);
}



/**
 * @brief 
 *  A function to set the motors for the robot that we have, comes with 4 different params. 

 * @param speed Stores the value on how fast we should go.
 * @param motorType Variable to tell the function which motor we should change.
 * @param isForward  A boolean value to see if we are going forward.
 * @param  isRelease Boolean value to release the motors when we need to stop.
 */
void MSet(int speed, char motorType, bool isForward = true, bool isRelease = false)
{
  // TODO: Rewrite this entire function since its poorly written, I think.
  /* //INFO I wrote this in order to clean up the messy code that is declaring 4 different motors
   INFO and setting them independently from each other. Espically with the CW functions.  */
  // We use this boolean so that we can say if we want to move forward or not.
  switch (motorType) // check to see if the char motorType is...
  {
  case 'A': // if its BR(BackRight), then do the BackRight Motors.
    BackR->setSpeed(speed);
    if (isForward == false)
    {
      if (isRelease == true)
      {
        BackR->run(RELEASE);
      }
      else
      {
      BackR->run(BACKWARD);
      }
    }
    else
    {
      BackR->run(FORWARD);
    }
    break;

  case 'B': // same thing but for the BackLeft motor and so on.
    BackL->setSpeed(speed);
    if (isForward == false)
    {
            if (isRelease == true)
      {
        BackL->run(RELEASE);
      }
      else
      {
      BackL->run(BACKWARD);
      }
    }
    else
    {
      BackL->run(FORWARD);
    }
    break;

  case 'C':
    FrontL->setSpeed(speed);
    if (isForward == false)
    {
            if (isRelease == true)
      {
        FrontL->run(RELEASE);
      }
      else
      {
      FrontL->run(BACKWARD);
      }
    }
    else
    {
      FrontL->run(FORWARD);
    }
    break;

  case 'D':
    FrontR->setSpeed(speed);
    if (isForward == false)
    {
            if (isRelease == true)
      {
        FrontR->run(RELEASE);
      }
      else
      {
        FrontR->run(BACKWARD);
      }
    }
    else
    {
      FrontR->run(FORWARD);
    }
    break;
  }
}

void Forward(int speed)
{
  MSet(speed, 'A', true); // The new function for the motor, first param is speed, 2nd is the motor, and the 3rd is for going forward.
  MSet(speed, 'B', true);
  // BackR->setSpeed(speed);
  // BackR->run(FORWARD);
  // BackL->setSpeed(speed);
  // BackL->run(FORWARD);
  //  Make the robot roll forward
  //  when the function is called
}

void Backward(int speed)
{
  MSet(speed, 'A', false);
  MSet(speed, 'B', false);
  //  BackR->setSpeed(speed);
  //  BackR->run(BACKWARD); // Make the robot roll forward
  //  BackL->setSpeed(speed);
  //  BackL->run(BACKWARD);
}

void TLeft(int speed)
{
  MSet(offset, 'A', true);
  MSet(offset, 'B', true);
  //  BackR->setSpeed(offset);
  //  BackR->run(FORWARD);
  //  BackL->setSpeed(offset);
  //  BackL->run(FORWARD);
  // Make the robot roll forward
}

/**
 * @brief
 * A function to turn right, that has a param of the speed it needs.
 */
void TRight(int speed)
{
  MSet(offset, 'A', true);
  MSet(offset, 'B', true);
  //  BackR->setSpeed(offset);
  //  BackR->run(FORWARD); // Make the robot roll forward
  //  BackL->setSpeed(offset);
  //  BackL->run(FORWARD);
}

/**
 * @brief A function to update a 16x2 display that is connected over the I2C bus on the Arduino.
 * Has 4 different params due to the small screen size that is a 16x2. It was the only way for it to correctly
 * scale like this.
 * @param bshowData A boolean value to show the Data that we are sending it.
 * @param bshowOffset Another boolean value to show the offset of turning.
 * @param bshowSpeed  Boolean value to show the speed that is set.
 * @param bshowDirection  Boolean value to show the direction we are going, display custom chars.
 */
void UpdateDisplay_I2C(bool bshowData, bool bshowOffset, bool bshowSpeed, bool bshowDirection = true)
{
  // TODO: Fix weird display bug that doesn't show the actual value for data and speed.
  RawValue = analogRead(A10);
  BatteryVoltage = RawValue / BatteryCal;
  lcd.clear();               // This command clears the buffer
  lcd.setCursor(0, 0);       // The first number 0-128 pixels horiontal. The second is 0-64 pixels vertical
  lcd.print("Bat:");         // prints to OLED
  lcd.setCursor(4, 0);       // moves the cursor over 50 pixels
  lcd.print(BatteryVoltage); // prints the calcualted voltage
  lcd.setCursor(7, 0);       // moves the cursor over to position 100 pixels
  lcd.print("V");            // prints the "V"
  lcd.setCursor(1, 1);       // Second line moves down 16 pixals
  delay(20);
  if (bshowData == true)
  {
    lcd.setCursor(0, 1); // Second line moves down 16 pixal
    lcd.print("Data ");  // prints to OLED
    lcd.setCursor(3, 1); // moves over om second line
    lcd.print(data);     // prints the data value from the Bluetooth module
                         // Serial.println("bshowData has been set to true!");
  }
  else if (bshowOffset == true)
  {
    lcd.setCursor(0, 32);  // third line
    lcd.print("Offset ");  // prints to OLED
    lcd.setCursor(90, 32); // moves over and down
    lcd.print(offset);
    // Serial.println("bshowOffset has been set to true!");
  }else if (bshowSpeed == true)
  {
    lcd.setCursor(0, 1); // fourth line
    lcd.print("Speed "); // prints to OLED
    lcd.setCursor(3, 1); // moves over and down
    lcd.print(speed);  // prints direction
  }
  else if(bshowDirection == true)
  {
    // We print the arrow char that we have to the display
    lcd.setCursor(0, 1);
    lcd.write((byte)2);
    delay(20);
    switch (data)
    {
    case 'u': // check to see which direction is it going from looking at the data variable.
      lcd.write((byte)2);
      break;
    case 'd':
      lcd.write((byte)3);
      break;

    case 'l':
      lcd.write((byte)1);
      break;

    case 'r':
      lcd.write((byte)0);
      break;
    default:
      lcd.write((byte)3);
      break;
    } 
  }else
  {
    Serial.print("No parameters have been passedthrough.");
  }
}

/**
 * @brief A function to "CrabWalk" with the omni-wheels that are present on the robot.
 *
 * @param PivotSpeed
 */
void CWRight(int PivotSpeed)
{
  MSet(PivotSpeed, 'BL', true);
  MSet(PivotSpeed, 'FR', true);
  //  BackL->setSpeed(PivotSpeed);
  //  FrontR->setSpeed(PivotSpeed);
  //  BackL->run(FORWARD);
  //  FrontR->run(FORWARD); // Make the robot roll forward
}

void CWLeft(int PivotSpeed)
{
  MSet(PivotSpeed, 'BR', true);
  MSet(PivotSpeed, 'FL', true);
  //  BackR->setSpeed(PivotSpeed);  // Allow the omni-wheels to make the robot crab walk for both.
  //  FrontL->setSpeed(PivotSpeed); // TODO: crabwalk should be its own separate function so that pivoting can be done.
  //  BackR->run(FORWARD);
  //  FrontL->run(FORWARD);
}

void UpdateDisplay()
{
  RawValue = analogRead(A10);             // Convert analog reading to Voltage
  BatteryVoltage = RawValue / BatteryCal; // This is a constant based on the resistor voltage divider
  display.clearDisplay();                 // This command clears the buffer
  display.setCursor(0, 0);                // The first number 0-128 pixels horizontal. The second is 0-64 pixels vertical
  display.print("Bat:");                  // prints to OLED
  display.setCursor(50, 0);               // moves the cursor over 50 pixels
  display.print(BatteryVoltage);          // prints the calculated voltage
  display.setCursor(100, 0);              // moves the cursor over to position 100 pixels
  display.print("V");                     // prints the "V"
  display.setCursor(0, 16);               // Second line moves down 16 pixals
  display.print("Data ");                 // prints to OLED
  display.setCursor(90, 16);              // moves over om second line
  display.print(data);                    // prints the data value from the Bluetooth module
  display.setCursor(0, 32);               // third line
  display.print("Offset ");               // prints to OLED
  display.setCursor(90, 32);              // moves over and down
  display.println(offset);                // prints direction
  display.setCursor(0, 50);               // fourth line
  display.print("Speed ");                // prints to OLED
  display.setCursor(80, 50);              // moves over and down
  display.println(speed);                 // prints direction
  display.display();                      // writes all the above changes to the screen
}

void Stop()
{ // This is the stop function
  // speed = 0;
  // i = 0;
  MSet(speed, 'BR', false, true);
  MSet(speed, 'BL', false, true);
 // BackR->setSpeed(speed);
 // BackL->setSpeed(speed);
 // FrontR->setSpeed(speed);
 // FrontL->setSpeed(speed);
 // BackR->run(RELEASE);
 // BackL->run(RELEASE);
 // FrontR->run(RELEASE);
 // FrontL->run(RELEASE);
}

/**
 * @brief 
 * The function that runs all the time, at least for an Arduino.
 */
void loop()
{
  if (Serial1.available() > 0)
  {
    data = Serial1.read(); // TODO: change the way that we get dat so that no warning are produced
    /* ^ Warning that you get is "Check buffer boundaries if used in a loop including recursive loops (CWE-120, CWE-20)."*/
    delay(2);
    Serial.print("Data  ");
    Serial.println(data);
  } // This line prints it to serial monitor on computer
  // UpdateDisplay(); // this line runs the function to print our stuff to the OLED
  UpdateDisplay_I2C(false, false, false); // Uncomment this to get the I2C 16x2 display working, along with some optional parameters.
  //  ^ has 3 boolean values, bshowOffset, bshowData, bshowSpeed due to the small screen size.
  switch (data) // Depending on the data that we get, change the speed and make the robot move.
  {
  case 'o':
    Stop();
    break;

  case '0':
    Stop();
    break;

  case 'u':
    Forward(speed);
    delay(2);
    break;

  case 'd':
    Backward(speed);
    delay(2);
    break;

  case 'l':
    TLeft(speed);
    delay(2);
    break;

  case 'r':
    TRight(speed);
    delay(2);
    break;

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
    speed = 250;
    break;

  case 'M':
    speed = 75;
    break;

  case 'N':
    speed = 150;
    break;
  }
  offset = (speed / 2); // this makes the offset for turning one half
  if (data == '4')
  {
    CWRight(speed); // pivot inplace to the right
  }
  // if(data == '3'){setSpeed = 75;}  // TBD
  if (data == '1')
  {
    CWLeft(speed); // pivot inplace to the left
  }

} // This is the end of the void Loop