// libraries and stuff
#include <Servo.h>

// photoresistor
int LDRpin = A0;

//serial monitor
int baudrate = 9600;

// servo
int servopin = 9;
int defaultangle = 0; // set initial servo position
int pressingangle = 90; // set angle for servo to mash space bar
Servo spacebarmasher; // naming my declared servo object
int pressingTime=100;
int stabilisationTime=50;

// screen brightness calibration
int threshold = 50; // some scaled percentage of the analog read that corresponds to dark screen patch
bool wasAboveThreshold = true; // set initial state of screen to be bright

// timing and speed calibration
unsigned long gameStartTime = 0;
float currentDelay = 500;    // initial delay between detection and jump
float minimumDelay = 50;     // minimum possible delay (game max speed)
float speedIncreaseRate = 0.01; // how quickly delay decreases over time
// gotta fine tune this stuff once we do irl testing

void setup()
{
  spacebarmasher.attach(servopin); 
  Serial.begin(baudrate);
  pinMode(LDRpin, INPUT); // for feedback on serial monitor
  gameStartTime = millis();  // record all game start time 
}


void loop()
{
  int LDRvalue = analogRead(LDRpin);
  float LDRscaledvalue = ((float(LDRvalue)/1023.)*100.); // just to put 0-1023 on a scale from 0-100
  
  float timeElapsed = (millis() - gameStartTime) / 1000.0; //capture elapsed timme
  
  //debugging on serial monitor
  Serial.print("LDR value: ");
  Serial.print(LDRvalue); // to calibrate screen brightness to servo
  Serial.print(" , ");
  Serial.print("scaled LDR value: ");
  Serial.print(LDRscaledvalue);
  Serial.print(" , ");
  Serial.print("raw time elapsed: ");
  Serial.print(timeElapsed); //just for me to debug timer
  Serial.print(" , ");
  Serial.print("current delay value: ");
  Serial.print(currentDelay);
  Serial.println();
  // on Tinkercad, analogRead goes from 6-679 so I'll just calibrate to those values, recalibrate for  irl values
  // assuming a pure white screen gives >500 and a dark patch (obstacle) gives a reading of <500
  
  // calibration of delay between light sensing and motor motion
  currentDelay = max(minimumDelay, 500.0 / (1.0 + speedIncreaseRate * timeElapsed)); // this takes the higher value of the two items between the bracket, as current delay is always decreasing, minimumDelay works

  
  if (LDRscaledvalue<threshold && wasAboveThreshold==true)
  { // Trigger action for the first time below threshold
    wasAboveThreshold = false;  // Mark that we've triggered
    
    // varied scaling delay block:
    delay(currentDelay); // Wait the calculated delay before jumping

    //spacebar pressing
    spacebarmasher.write(pressingangle);
    delay(pressingTime);
    spacebarmasher.write(defaultangle); // presses the space bar on first change in light detection
    delay(stabilisationTime);
    // change delay time to accommodate all game speeds
  }
  
  else if (LDRscaledvalue >= threshold) 
  {
    wasAboveThreshold = true;   // Reset for next detection
    
    spacebarmasher.write(defaultangle); // ensures default position when bright screen
    delay(stabilisationTime);
  }
  
  delay(10);  // delay for the entire loop to prevents excessive analog reads
}