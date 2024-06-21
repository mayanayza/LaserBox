#include <ServoEasing.hpp>
ServoEasing servo;

#include <SparkFun_Alphanumeric_Display.h>
HT16K33 display;

#include <Wire.h>

#define servoPin 9
#define laserPin 2
#define vaporPin 5

const char* messages[3] = {"WHAT HAPPENED?", "WHERE AM I?", "THIS ISN'T WHAT I EXPECTED"};
int messageIndex = 0;
char* message = "";

int lastServoUpdateMs = 0;
int servoUpdateWaitMs = 1000;
int ms = 0;
int currentServoPosition = 0;
int nextServoPosition = 0;
int servoSpeed = 100;

void setup() {
  Serial.begin(9600);

  servo.attach(servoPin, nextServoPosition);
  servo.setSpeed(servoSpeed);
  servo.setEasingType(EASE_CUBIC_IN_OUT);
  
  pinMode(laserPin, OUTPUT);
  digitalWrite(laserPin, HIGH);
  
  pinMode(vaporPin, OUTPUT);
  digitalWrite(vaporPin, HIGH);
  
  Wire.begin();

  if (display.begin() == false)
  {
    Serial.println("Device did not acknowledge! Freezing.");
    while (1);
  }
  Serial.println("Display acknowledged.");
}

void loop() {

  display.print( message );

  if (strlen(message) == 0){
    messageIndex = random(0,3);
    message = messages[messageIndex];
    delay(500);
  } else {
    message = message + 1;
  }

  ms = millis();

  if (servoUpdateWaitMs < ms - lastServoUpdateMs && !servo.isMoving()){

    currentServoPosition = servo.getCurrentAngle();
    
    while ( abs(currentServoPosition - nextServoPosition) < 45 ){
      nextServoPosition = random(0,180);
    }

    servoSpeed = random(80,200);
    servo.startEaseTo(nextServoPosition, servoSpeed);
    lastServoUpdateMs = millis();
    servoUpdateWaitMs = random(1000,4000);
  }

  delay( 300 ); 
}