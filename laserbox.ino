#include <ServoEasing.hpp>
ServoEasing servo;

#include <SparkFun_Alphanumeric_Display.h>
HT16K33 display;

#include <Wire.h>

#define servoPin 11
#define laserPin 13
#define vaporPin 12

#define SERVO_START_SPEED 100
#define SERVO_START_POSITION 0

#define TEXT_SCROLL_SPEED 300 

const char* messages[8] = {"WHAT HAPPENED?", "WHERE AM I?", "THIS ISN'T WHAT I EXPECTED", "SOMETHING ISN'T RIGHT", "HOW DO I GET OUT OF HERE?", "PLEASE HELP ME", "I DON'T UNDERSTAND", "I'M SCARED"};

void setup() {
  Serial.begin(9600);

  servo.attach(servoPin, SERVO_START_POSITION);
  servo.setSpeed(SERVO_START_SPEED);
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

int currentServoPosition;
int nextServoPosition;
int nextServoSpeed;

int randomValue;
int lastServoUpdateMs = 0;
int servoUpdateWaitMs = 1000;
int ms = 0;

int lastMessageUpdateMs = 0;
int messageIndex = 0;
char* message = "";


void loop() {

  ms = millis();

  display.print( message );

  if (ms - TEXT_SCROLL_SPEED > lastMessageUpdateMs) {
    
    lastMessageUpdateMs = ms;
    
    if (strlen(message) == 0){
      messageIndex = random(0,3);
      message = messages[messageIndex];
      delay(500);
    } else {
      message = message + 1;
    }
  }

  if (servoUpdateWaitMs < ms - lastServoUpdateMs && !servo.isMoving()){

    currentServoPosition = servo.getCurrentAngle();
    randomValue = random(0,145);

    if (randomValue <= currentServoPosition-22) nextServoPosition = randomValue;
    else nextServoPosition = randomValue + 45;
    
    nextServoSpeed = random(80,200);
    servo.startEaseTo(nextServoPosition, nextServoSpeed);
    lastServoUpdateMs = millis();
    servoUpdateWaitMs = random(1000,4000);
  }
}